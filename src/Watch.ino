#include <Time.h>
#include <Snooze.h>
#include <EEPROM.h>
#include "U8glib.h"
#include "Watchface.h"
#include "CuniUI.h"
#include "Bluetooth.h"
#include "Buttons.h"
#include "LED.h"
#include "StopWatch.h"
#include "PowerSaver.h"
#include "Bitmaps.h"

/* 
 * The BIG ToDo List 
 * - fix some horrible bugs: the Timer bug, and the Snooze RTC bug
 * - fix another (easy to solve) bug: laps have a reset when putting the app in background (while status and stopwatch run properly in background)
 * - (maybe) save stopwatch data and latest watchface in EEPROM
 * - use Bluetooth menu (now empty) for Notifications and device info (BT firmware, module name...)
 * - add some apps to the Extras menu (it'd be cool to be able to install apps via BT, but EEPROM is a little tiny...)
 * 
 * Settings (many, many changes):
 * - as the option number is going to grow, first of all a decent menu support on the Cuni UI must be added
 * - set time and date (and function to set them automatically on BT connection)
 * - boot-to-clock: direct boot to your favourite watchface
 * - quick boot: don't init bluetooth (or init it when enabling it)
 * - serial console: display log with a tiny, tiny font on the display, and receive commands for almost all settings from serial port
 * - watch and serial password: 4-digit password as a nice (very very basic) security function
 * - quick action on press of up+down keys together (well, other combinations are still possible, like up+select, down+select, select+back etc.)
 * - factory reset: reset all EEPROM data (set all addresses to 0)
 * - set button radius (and other CuniUI settings maybe?) in Settings
 * 
 * 
 * Cuni UI library:
 * - add MENUS!!!
 * - add DatePicker, TimePicker, ProgressWindow (dialog with progress bar, for example for EEPROM reset or other long tasks
 * - add nice Android-style dialog with radio buttons (and/or checkboxes)
 * - add progress screen (with a nice progress bar, like for Factory Reset)
 * - add alert screens with auto dismiss after X seconds
 * 
 * Once all the things on this list (which is going to grow) are done, CuniOS will be a nice DIY smartwatch OS. Don't worry, it'll take less than 2 centuries ;)
 * 
 * Happy hacking!
 */

// Set pins below, but make sure they are supported
const int BTN_BACK = 2;
const int BTN_SELECT = 3;
const int BTN_UP = 11;
const int BTN_DOWN = 8;
const int BUZZER = 21;
const int LED_PIN = A6;
const int EEPROM_ALARM_HOUR = 0;
const int EEPROM_ALARM_MINUTE = 1;
const int EEPROM_ALARM_ENABLED = 2;
const int EEPROM_BT_ENABLED = 3;
const int SW_MENU_DELAY = 200; // in milliseconds, reducing it will reduce lag but make some actions, like menu navigation, more difficult
const int BTN_RADIUS = 1; // for CuniUILib
const int DISPLAY_WIDTH = 128;
const int DISPLAY_HEIGHT = 64;

boolean bluetooth_available = false;
boolean bluetoothOn = false;
boolean lostAlarm = false;
boolean ignoreAlarm = false;
boolean alarmEnabled = false;

char formattedHour[5];
char fullFormattedHour[9];
char fullFormattedDate[11];
int intCounter = 0;
int statusBarCounter = 0;
int alarmHour = 0;
int alarmMinute = 0;

int swHour = 0;
int swMinute = 0;
int swSecond = 0;
int swMillis = 0;

boolean timerSet = false;
long timerSeconds = 0;

// Chess variables
uint8_t uiKeyCodeFirst = CHESS_KEY_NONE;
uint8_t uiKeyCodeSecond = CHESS_KEY_NONE;
uint8_t uiKeyCode = CHESS_KEY_NONE;

U8GLIB_SSD1306_128X64 u8g(4, 5, 6, 7);
//U8GLIB_SH1106_128X64 u8g(4, 5, 6, 7); // replace with this or another constructor if you notice pixels shifted by 2 positions on Y axis... 

StopWatch sw;    // MILLIS (default)
StopWatch timerSW(StopWatch::SECONDS);
Bluetooth bt(9600);
LED notificationLED(LED_PIN);
PowerSave pwrsave;
ModKeypad keypad(BTN_BACK, BTN_SELECT, BTN_UP, BTN_DOWN);
CuniUI ui(u8g, keypad, 128, 64);

time_t getTeensy3Time() { return Teensy3Clock.get(); }

void setup() {
  Serial.begin(9600);
  setSyncProvider(getTeensy3Time);
  pinMode(BUZZER,OUTPUT);
  u8g.setColorIndex(1);
  boot();

}

void loop() {
  homeScreen();
}
  
void boot() {
  int progress = 0;
  while(true) {
    int x = 31 + (31 * progress);
    u8g.firstPage();
    do {
      u8g.drawXBMP(0,0,BMP_BOOT_SIZE[0],BMP_BOOT_SIZE[1],BMP_BOOT);
      u8g.drawBox(0,58,x,6);
    } while(u8g.nextPage());

    if(progress == 3) break;
    progress++;

    switch(progress) {
      case 0:
      // init various things
      break;

      case 1:
      // copy EEPROM to RAM
      alarmHour = EEPROM.read(EEPROM_ALARM_HOUR);
      alarmMinute = EEPROM.read(EEPROM_ALARM_MINUTE);
      bluetoothOn = (EEPROM.read(EEPROM_BT_ENABLED) == 1);
      updateAlarmEnabled();
      break;
      

      case 2:
      // init Bluetooth
      if(!bt.isReady()) { 
        ui.alert("No Bluetooth found","Timeout on Serial2", true, "Proceed");
        bluetooth_available = false; // NO hotplug support, needs reboot if not inserted
      } else {
        bluetooth_available = true;
      }
      break;

      case 3:
      break;
      
      default:
      break;
    }
  }
  delay(2 * SW_MENU_DELAY);
}
void homeScreen() {
  boolean home = true;
  int homeCursor = 0;
  while(home) {
    isAlarm();  
    int oldHomeCursor = homeCursor;
    int btn = keypad.getPressedButton();
    if(btn == BTN_DOWN) {
      if(homeCursor == 3)
        homeCursor = 0;
      else
        homeCursor++;
    } else if(btn == BTN_UP) {
      if(homeCursor == 0)
        homeCursor = 3;
      else
        homeCursor--;
    } else if(btn == BTN_SELECT) {
      delay(SW_MENU_DELAY);
      switch(homeCursor) {
        case 0:
        watch();
        break;
        case 1:
        break;
        case 2:
        chess(); // uhm... quite disappointing for someone looking for something more useful (TODO!)
        break;
        case 3:
        settingsScreen();
        break;
        default:
        break;
      }
    } else if(btn == BTN_BACK){
      delay(SW_MENU_DELAY);
      clock();
    }
    u8g.firstPage();
    do {  
      drawStatusBar();
      drawLauncher(homeCursor);
    } while( u8g.nextPage());
    if(homeCursor != oldHomeCursor) delay(SW_MENU_DELAY);
  }
}
void drawStatusBar() {
  u8g.setFont(u8g_font_helvB08);
  u8g.setFontPosTop();
  updateFormattedHour();
  u8g.drawStr(50,-1,formattedHour);
  u8g.drawBox(112,0,12,6);
  u8g.drawBox(124,1,2,4);
  if(alarmEnabled) {
    u8g.drawXBMP(0,0,BMP_STATUS_ALARMLOST_SIZE[0],BMP_STATUS_ALARMLOST_SIZE[1], BMP_STATUS_ALARMLOST);
  }
  // TODO: draw BT broken, BT enabled, stopwatch, timer icons etc.

}
void drawLauncher(int choice) {
  u8g.setFontPosTop();
  u8g.setFont(u8g_font_helvR08);
  if(choice == 0) {
    u8g.setFont(u8g_font_helvB08);
    u8g.drawStr(64,22,"Watch");
    u8g.setFont(u8g_font_helvR08);

    u8g.drawXBMP( 11, 18, BMP_HOME_CLOCK_SIZE[0], BMP_HOME_CLOCK_SIZE[1], BMP_HOME_CLOCK);
  } else {
    u8g.drawStr(64,22,"Watch");
  }
  if(choice==1) {
    u8g.setFont(u8g_font_helvB08);
    u8g.drawStr(64,34,"Bluetooth");
    u8g.setFont(u8g_font_helvR08);

    u8g.drawXBMP( 11, 18, BMP_HOME_BLUETOOTH_SIZE[0], BMP_HOME_BLUETOOTH_SIZE[1], BMP_HOME_BLUETOOTH);
  } else {
    u8g.drawStr(64,34,"Bluetooth");
  }
  if(choice==2) {
    u8g.setFont(u8g_font_helvB08);
    u8g.drawStr(64,46,"Extras");
    u8g.setFont(u8g_font_helvR08);
    u8g.drawXBMP( 11, 18, BMP_HOME_EXTRAS_SIZE[0], BMP_HOME_EXTRAS_SIZE[1], BMP_HOME_EXTRAS);
  } else {
    u8g.drawStr(64,46,"Extras");
  }
  if(choice==3) {
    u8g.setFont(u8g_font_helvB08);
    u8g.drawStr(64,58,"Settings");
    u8g.setFont(u8g_font_helvR08);

    u8g.drawXBMP( 11, 18, BMP_HOME_SETTINGS_SIZE[0], BMP_HOME_SETTINGS_SIZE[1], BMP_HOME_SETTINGS);
  } else {
    u8g.drawStr(64,58,"Settings");
  }
 
}
void settingsScreen() {
  char *bluetoothStatus;
  int menuCursor = 0;
  boolean settings = true;
  while(settings) {
    isAlarm();  
    int oldMenuCursor = menuCursor;
    int btn = keypad.getPressedButton();
    if(btn != 0) {
      clicker();
      if(btn == BTN_DOWN) {
        if(menuCursor == 3)
          menuCursor = 0;
        else
          menuCursor++;
      } else if(btn == BTN_UP) {
        if(menuCursor == 0)
          menuCursor = 3;
        else
          menuCursor--;
      } else if(btn == BTN_SELECT) {
        delay(SW_MENU_DELAY);
        switch(menuCursor) {
          case 0:
          toggleBluetoothStatus();
          break;
          case 1:
          serialConsole();
          break;
          case 2:
          break;
          case 3:
          softwareUpdate();
          break;
          default:
          break;
        }
      } else if(btn == BTN_BACK){
        delay(SW_MENU_DELAY);
        settings = false;
      } else {
        
      }
    }
    if(bluetoothOn) {
      bluetoothStatus = "Bluetooth ON";
    } else {
      bluetoothStatus = "Bluetooth OFF";
    }
    u8g.firstPage();
    do {
      drawStatusBar();
      
      u8g.setFontPosTop();
      u8g.setFont(u8g_font_helvR08);
      if(menuCursor == 0) {
        u8g.setFont(u8g_font_helvB08);
        u8g.drawStr(10,22,bluetoothStatus);
        u8g.setFont(u8g_font_helvR08);
      } else {
        u8g.drawStr(10,22,bluetoothStatus);
      }
      if(menuCursor==1) {
        u8g.setFont(u8g_font_helvB08);
        u8g.drawStr(10,34,"Serial console");
        u8g.setFont(u8g_font_helvR08);
      } else {
        u8g.drawStr(10,34,"Serial console");
      }
      if(menuCursor==2) {
        u8g.setFont(u8g_font_helvB08);
        u8g.drawStr(10,46,"System info");
        u8g.setFont(u8g_font_helvR08);
      } else {
        u8g.drawStr(10,46,"System info");
      }
      if(menuCursor==3) {
        u8g.setFont(u8g_font_helvB08);
        u8g.drawStr(10,58,"Factory reset");
        u8g.setFont(u8g_font_helvR08);
      } else {
        u8g.drawStr(10,58,"Factory reset");
      }
    } while( u8g.nextPage());
    if(menuCursor != oldMenuCursor) delay(SW_MENU_DELAY);
  }
}
void serialConsole() {
  boolean serial = true;
  Serial.println("READY");
  while(serial) {
    isAlarm();  
    u8g.firstPage();
    do {  
      drawStatusBar();
      u8g.setFontPosTop();
      u8g.setFont(u8g_font_helvB08);
      u8g.drawStr(5,12,"Serial console");
      u8g.setFont(u8g_font_helvR08);
      u8g.setFontPosTop();
      u8g.drawStr(5,24,"Speed: 9600 baud");
      u8g.drawStr(5,36,"Waiting for commands...");
      if(keypad.getPressedButton() == BTN_BACK) {
        clicker();
        delay(SW_MENU_DELAY);
        serial = false;
      }
    } while( u8g.nextPage());
    //if(Serial.available() > 0) {
    //}
  }
}
void toggleBluetoothStatus() {
  // TODO: handle in EEPROM!
  if(bluetooth_available) {
    if(bluetoothOn) {
      if(ui.confirm("Disable Bluetooth?","OK","Cancel")) bluetoothOn = false;
    } else {
      if(ui.confirm("Enable Bluetooth?","OK","Cancel")) bluetoothOn = true;
    }
    EEPROM.write(EEPROM_BT_ENABLED,bluetoothOn);
  } else {
    ui.alert("No Bluetooth found","Please insert BT",true,"OK");
  }
}
void clock() {
  boolean clock = true;
  while(clock) {
    isAlarm();
    updateFormattedHour();  
    u8g.firstPage();
    do {  
        drawWatchFace(formattedHour, fullFormattedHour, fullFormattedDate, u8g);
    } while( u8g.nextPage());
    int btn = keypad.getPressedButton();
    if(btn == BTN_SELECT) {
      clicker();
      delay(SW_MENU_DELAY);
      clock = false;
    } else if(btn == BTN_DOWN) {
      switchWatchFacePrevious();
      delay(SW_MENU_DELAY);
    } else if(btn == BTN_UP) {
      switchWatchFace();
      delay(SW_MENU_DELAY);
    } else if(btn == BTN_BACK) {
      u8g.firstPage();
      do {
        // clear screen
      } while(u8g.nextPage());
      pwrsave.sleepUntilButtonWake(BTN_UP); // TODO/BUG: RTC NOT working while sleeping!!
    }
    delay(1);
  }
}
void watch() {
  boolean watch = true;
  int menuCursor = 0;
  while(watch) {
    isAlarm();  
    int oldMenuCursor = menuCursor;
    int btn = keypad.getPressedButton();
    if(btn != 0) {
      clicker();
      if(btn == BTN_DOWN) {
        if(menuCursor == 2)
          menuCursor = 0;
        else
          menuCursor++;
      } else if(btn == BTN_UP) {
        if(menuCursor == 0)
          menuCursor = 2;
        else
          menuCursor--;
      } else if(btn == BTN_SELECT) {
        switch(menuCursor) {
          case 0:
          delay(SW_MENU_DELAY);
          stopwatch();
          break;
          case 1:
          delay(SW_MENU_DELAY);
          Serial.print("entering timer; menuCursor #");
          Serial.println(menuCursor);
          timer(); // TODO: SOLVE POINTER BUG!
          Serial.print("exit timer; menuCursor #");
          Serial.println(menuCursor);
          break;
          case 2:
          delay(SW_MENU_DELAY);
          char *txt;
          if(alarmEnabled) {
            txt = "Disable";
          } else {
            txt = "Enable";
          }
          int result;
          result = ui.dialog("Alarm","Set time",txt,true);
          if(result == 1) {
            setAlarm();
          } else if(result == 2) {
            toggleAlarmEnabled();
          } else {
            // operation cancelled
          }
          break;
          default:
          break;
        }
      } else if(btn == BTN_BACK){
        delay(SW_MENU_DELAY);
        watch = false;
      } else {
        
      }
    }
    u8g.firstPage();
    do {
      drawStatusBar();
      u8g.drawXBMP( 11, 18, 40, 40, BMP_HOME_CLOCK);
      u8g.setFontPosTop();
      u8g.setFont(u8g_font_helvR08);
      
      if(menuCursor == 0) {
        u8g.setFont(u8g_font_helvB08);
        u8g.drawStr(64,30,"Stopwatch");
        u8g.setFont(u8g_font_helvR08);
      } else {
        u8g.drawStr(64,30,"Stopwatch");
      }
      if(menuCursor==1) {
        u8g.setFont(u8g_font_helvB08);
        u8g.drawStr(64,42, "Timer");
        u8g.setFont(u8g_font_helvR08);
      } else {
        u8g.drawStr(64,42, "Timer");
      }
      if(menuCursor==2) {
        u8g.setFont(u8g_font_helvB08);
        u8g.drawStr(64,54,"Alarm");
        u8g.setFont(u8g_font_helvR08);
      } else {
        u8g.drawStr(64,54,"Alarm");
      }
    } while( u8g.nextPage());
    if(menuCursor != oldMenuCursor) delay(SW_MENU_DELAY);
  }
}
void softwareUpdate() {
  if(ui.confirm("This will delete all data","Proceed","Cancel")) {
    // delete EEPROM data (with progress bar)
    ui.alert("","Reset successful",true,"Reboot");
    // reboot
  }
}
void setAlarm() {
  boolean alarm = true;
  char text[30];
  int alHour = alarmHour;
  int alMinute = alarmMinute;
  while(alarm) {
    boolean setAlarmHour = true;
    while(setAlarmHour) {
      char hourStr[3];
      u8g.firstPage();
      sprintf(hourStr,"%02d",alHour);
      do {
        u8g.setFont(u8g_font_fub49n);
        u8g.setFontPosTop();
        int x = floor( ( DISPLAY_WIDTH - u8g.getStrWidth(hourStr) ) / 2 );
        u8g.drawStr(x,5,hourStr);
        
        u8g.setFont(u8g_font_helvR08);
        u8g.setFontPosTop();
        u8g.drawStr90(10,22,"hour");
      } while(u8g.nextPage());
      int btnID = keypad.getPressedButton();
      if(btnID != 0) {
        switch(btnID) {
          case BTN_UP:
          alHour++;
          break;
          case BTN_DOWN:
          alHour--;
          break;
          case BTN_BACK:
          // TODO: cancel operation
          break;
          case BTN_SELECT:
          delay(SW_MENU_DELAY);
          setAlarmHour = false;
          break;
          default:
          break;
        }
        delay(SW_MENU_DELAY/2);
      }
      if(alHour > 23) alHour = 0;
      if(alHour < 0) alHour = 23;
    }
    boolean setAlarmMinute = true;
    while(setAlarmMinute) {
      char minuteStr[3];
      u8g.firstPage();
      sprintf(minuteStr,"%02d",alMinute);
      do {
        u8g.setFont(u8g_font_fub49n);
        u8g.setFontPosTop();
        int x = floor( ( DISPLAY_WIDTH - u8g.getStrWidth(minuteStr) ) / 2 );
        u8g.drawStr(x,5,minuteStr);

        u8g.setFont(u8g_font_helvR08);
        u8g.setFontPosTop();
        u8g.drawStr90(10,18,"minute");
      } while(u8g.nextPage());
      int btnID = keypad.getPressedButton();
      if(btnID != 0) {
        switch(btnID) {
          case BTN_UP:
          alMinute++;
          break;
          case BTN_DOWN:
          alMinute--;
          break;
          case BTN_BACK:
          // TODO: cancel operation
          break;
          case BTN_SELECT:
          delay(SW_MENU_DELAY/2);
          setAlarmMinute = false;
          break;
          default:
          break;
        }
        delay(SW_MENU_DELAY/2);
      }
      if(alMinute > 59) alMinute = 0;
      if(alMinute < 0) alMinute = 59;
    }
    sprintf(text,"Set alarm to %02d:%02d?", alHour,alMinute);
    if(ui.confirm(text,"OK","Cancel")) {
      setAlarmTime(alHour,alMinute);
      alarm = false;
    } else {
      alarm = false;
    }
  }
  
}
void alarmLoop() {
  int x = 7;
  int y = 0;
  while(!ignoreAlarm) {
    u8g.firstPage();
    do {
      drawStatusBar();
      u8g.setFont(u8g_font_helvB08);
      u8g.setFontPosTop();
      u8g.drawStr(47,25,"ALARM");
      u8g.setFont(u8g_font_helvR08);
      u8g.setFontPosTop();
      u8g.drawStr(35,40,"Press any key...");
    } while(u8g.nextPage());
    if(x == 7) {
      notificationLED.enable(255);
      buzzerTone(1000,50);
      delay(50);
      notificationLED.disable();
      buzzerTone(1000,50);
      delay(50);
      notificationLED.enable(255);
      buzzerTone(1000,50);
      delay(50);
      buzzerTone(1000,50);
      notificationLED.disable();
      x = 0;
    }
    if(y == 350) {
      lostAlarm = true;
      ignoreAlarm = true;
      break;
    }
    if(keypad.getPressedButton() != 0) {
      delay(SW_MENU_DELAY);
      ignoreAlarm = true;
      break;
    }
    x++;
    y++;
  }
}
void stopwatch() {
  boolean stopwatch = true;
  boolean lapButtonPressed = false;
  
  char text[12];
  char lap1[15] = "#1 --:--:--:--";
  char lap2[15] = "#2 --:--:--:--";
  char lap3[15] = "#3 --:--:--:--";
  char lap4[15] = "#4 --:--:--:--";
  int lapCounter = 0;
  
  while(stopwatch) {
    u8g.firstPage();
    do {
      swMillis = sw.elapsed();
      swSecond = ((swMillis/1000) % 60);
      swMinute = ((swMillis / 60000) % 60);
      swHour = ((swMillis / 3600000) % 24);
      sprintf(text,"%02d:%02d:%02d:%02d",swHour,swMinute,swSecond,(swMillis / 10) % 100);
      
      int btn = keypad.getPressedButton();
      if(btn != 0) {
        if(btn == BTN_BACK) {
          delay(SW_MENU_DELAY);
          stopwatch = false;
        }
        if(btn == BTN_UP && !sw.isRunning()) {
          sw.start();
        }
        if(btn == BTN_DOWN && sw.isRunning()) {
          sw.stop();
          // TODO: maybe write stopwatch data on EEPROM?
        }
        if(btn == BTN_SELECT && sw.isRunning()) {
          if(!lapButtonPressed) {
            switch(lapCounter) {
              case 0:
              sprintf(lap1,"#1 %s",text);
              break;
              case 1:
              sprintf(lap2,"#2 %s",text);
              break;
              case 2:
              sprintf(lap3,"#3 %s",text);
              break;
              case 3:
              sprintf(lap4,"#4 %s",text);
              break;
              default:
              // do nothing
              break;
            }
            lapCounter++;
            lapButtonPressed = true;
          }
        } else if(btn != BTN_SELECT && sw.isRunning()) {
          lapButtonPressed = false;
        }
        if(btn == BTN_SELECT && !sw.isRunning()) {
          sw.reset();
          sprintf(lap1,"#1 --:--:--:--");
          sprintf(lap2,"#2 --:--:--:--");
          sprintf(lap3,"#3 --:--:--:--");
          sprintf(lap4,"#4 --:--:--:--");
          lapCounter = 0;
        }
      } else {
        lapButtonPressed = false;      
      }
      u8g.drawFrame(0,5,84,18);
      u8g.setFont(u8g_font_freedoomr10r);
      u8g.setFontPosTop();
      u8g.drawStr(4,8,text);

      u8g.setFont(u8g_font_profont10);
      u8g.drawStr(6,33,lap1);
      u8g.drawStr(6,42,lap2);
      u8g.drawStr(6,51,lap3);
      u8g.drawStr(6,60,lap4);
      
      u8g.drawLine(88,0,88,64);
      u8g.setFont(u8g_font_orgv01);
      u8g.setFontPosTop();
      u8g.drawStr(95,8,"START");
      u8g.setFontPosTop();
      if(sw.isRunning()) {
        sprintf(text,"LAP");
      } else {
        sprintf(text,"RESET");
      }
      u8g.drawStr(95,30,text);
      u8g.setFontPosTop();
      u8g.drawStr(95,51,"STOP");
    } while(u8g.nextPage());
  }
}
void timer() {
  boolean timerOn = true;
  int remainingSeconds = 0;
  char text[9];
  int timHour = 0;
  int timMinute = 0;
  int timSecond = 0;
  if(!timerSet) {
      setTimer();
      timerSW.start();
      timerSet = true;
  }
  while(timerSet && timerOn) {
    u8g.firstPage();
    do {
      int btnId = keypad.getPressedButton();
      if(btnId != 0) {
        if(btnId == BTN_BACK) {
          delay(2*SW_MENU_DELAY);
          timerOn = false;
          break;
        } else if(btnId == BTN_SELECT && !timerSW.isRunning()) {
          timerSW.start();
          delay(SW_MENU_DELAY);
        } else if(btnId == BTN_SELECT && timerSW.isRunning()) {
          timerSW.stop(); // THIS function must be the generator of the bug!
          delay(SW_MENU_DELAY);
        }
        else if(btnId == BTN_DOWN && !timerSW.isRunning()) { // URGENT DEBUG: freeze when reset or close while not running!
          timerSW.stop();
          timerSW.reset();
          timerSeconds = 0;
          timSecond = 0;
          delay(SW_MENU_DELAY);
          timerOn = false;
          timerSet = false;
          break;
        }
      }
      remainingSeconds = timerSeconds - timerSW.elapsed();
      timSecond = (remainingSeconds % 60);
      timMinute = ((remainingSeconds / 60) % 60);
      timHour = ((remainingSeconds / 3600) % 24);
      sprintf(text,"%02d:%02d:%02d",timHour,timMinute,timSecond);

      u8g.setFont(u8g_font_profont22);
      u8g.setFontPosTop();
      u8g.drawStr(((DISPLAY_WIDTH - u8g.getStrWidth(text))/2),19,text);

      u8g.setFont(u8g_font_chikita);
      u8g.setFontPosTop();
      if(timerSW.isRunning()) {
        sprintf(text,"Select: Pause");
      } else {
        sprintf(text,"Select: Start | Down: Reset");
      }
      u8g.drawStr(((DISPLAY_WIDTH - u8g.getStrWidth(text))/2),52,text);
      
    } while(u8g.nextPage());
    isAlarm(); // triggers both isAlarm and isTimer
  }
}
void setTimer() {
  int timerHour = 0;
  int timerMinute = 0;
    if(timerSW.isRunning()) {
      timerSW.stop();
      timerSW.reset();
    }
    boolean setTimerHour = true;
    while(setTimerHour) {
      char hourStr[3];
      u8g.firstPage();
      sprintf(hourStr,"%02d",timerHour);
      do {
        u8g.setFont(u8g_font_fub49n);
        u8g.setFontPosTop();
        int x = floor( ( DISPLAY_WIDTH - u8g.getStrWidth(hourStr) ) / 2 );
        u8g.drawStr(x,5,hourStr);
        
        u8g.setFont(u8g_font_helvR08);
        u8g.setFontPosTop();
        u8g.drawStr90(10,22,"hour");
      } while(u8g.nextPage());
      int btnID = keypad.getPressedButton();
      if(btnID != 0) {
        switch(btnID) {
          case BTN_UP:
          timerHour++;
          break;
          case BTN_DOWN:
          timerHour--;
          break;
          case BTN_BACK:
          // TODO: cancel operation
          break;
          case BTN_SELECT:
          delay(SW_MENU_DELAY);
          setTimerHour = false;
          break;
          default:
          break;
        }
        delay(SW_MENU_DELAY/2);
      }
      if(timerHour > 23) timerHour = 0;
      if(timerHour < 0) timerHour = 23;
    }
    boolean setTimerMinute = true;
    if(timerHour == 0) timerMinute = 1;
    while(setTimerMinute) {
      char minuteStr[3];
      u8g.firstPage();
      sprintf(minuteStr,"%02d",timerMinute);
      do {
        u8g.setFont(u8g_font_fub49n);
        u8g.setFontPosTop();
        int x = floor( ( DISPLAY_WIDTH - u8g.getStrWidth(minuteStr) ) / 2 );
        u8g.drawStr(x,5,minuteStr);

        u8g.setFont(u8g_font_helvR08);
        u8g.setFontPosTop();
        u8g.drawStr90(10,18,"minute");
      } while(u8g.nextPage());
      int btnID = keypad.getPressedButton();
      if(btnID != 0) {
        switch(btnID) {
          case BTN_UP:
          timerMinute++;
          break;
          case BTN_DOWN:
          timerMinute--;
          break;
          case BTN_BACK:
          // TODO: cancel operation
          break;
          case BTN_SELECT:
          delay(SW_MENU_DELAY/2);
          setTimerMinute = false;
          break;
          default:
          break;
        }
        delay(SW_MENU_DELAY/2);
      }
      if(timerMinute > 59 && timerHour == 0) timerMinute = 1;
      if(timerMinute > 59 && timerHour != 0) timerMinute = 0;
      if(timerMinute < 0) timerMinute = 59;
      if(timerMinute < 1 && timerHour == 0) timerMinute = 59;
    }
    timerSeconds = (timerHour * 3600) + (timerMinute * 60);
}

void buzzerTone(int toneValue, int time) {
  tone(BUZZER,toneValue);
  delay(time);
  noTone(BUZZER);
  digitalWrite(BUZZER,LOW);
}
void clicker() {
  buzzerTone(70,15);
}



void isAlarm() {
  isTimer();
  if(lostAlarm) {
    lostAlarm = false;
  }
  if(alarmEnabled && hour() == alarmHour && minute() == alarmMinute)
    alarmLoop();
}
void isTimer() {
  int x = 0;
  if(timerSW.isRunning()) {
    if(timerSW.elapsed() >= timerSeconds) {
      timerSW.stop();
      timerSW.reset();
      timerSeconds = 0;
      timerSet = false;
      while(true) {
        u8g.firstPage();
        do {
          drawStatusBar();
          u8g.setFont(u8g_font_helvB08);
          u8g.setFontPosTop();
          u8g.drawStr(35,25,"Time is up!");
          u8g.setFont(u8g_font_helvR08);
          u8g.setFontPosTop();
          Serial.println((DISPLAY_WIDTH - u8g.getStrWidth("Press any key..."))/2);
          u8g.drawStr(27,40,"Press any key...");
        } while(u8g.nextPage());
        if((x % 7) == 0) {
          notificationLED.enable(255);
          buzzerTone(300,50);
          delay(80);
          notificationLED.disable();
          buzzerTone(300,50);
        }
        if(x == 100) {
          break;
        }
        if(keypad.getPressedButton() != 0) {
          delay(SW_MENU_DELAY);
          break;
        }
        x++;
      }
    }
  }
}
void updateFormattedHour() {
  intCounter++;
  sprintf(formattedHour,"%02d:%02d", hour(), minute());
  sprintf(fullFormattedHour,"%02d:%02d:%02d",hour(), minute(), second());
  sprintf(fullFormattedDate,"%02d/%02d/%02d", day(), month(), year() % 100);
  intCounter = 0;
}

void updateAlarmEnabled() {
  if(EEPROM.read(EEPROM_ALARM_ENABLED) == 1) {
    alarmEnabled = true;
  } else {
    alarmEnabled = false;
  }  
}
void toggleAlarmEnabled() {
  ignoreAlarm = false;
  if(EEPROM.read(EEPROM_ALARM_ENABLED) == 1) {
    alarmEnabled = false;
    EEPROM.write(EEPROM_ALARM_ENABLED,0);
  } else {
    EEPROM.write(EEPROM_ALARM_ENABLED,1);
    alarmEnabled = true;
  }  
}
void setAlarmTime(int alHour, int alMinute) {
  EEPROM.write(EEPROM_ALARM_HOUR,alHour);
  EEPROM.write(EEPROM_ALARM_MINUTE,alMinute);
  alarmHour = EEPROM.read(EEPROM_ALARM_HOUR);
  alarmMinute = EEPROM.read(EEPROM_ALARM_MINUTE);
}

/* CHESS */
void chess() {
  chess_Init(u8g.getU8g(), 0);
  boolean chLoop = true;
  while(chLoop) {
    chLoop = chessLoop();
  }
}
boolean chessLoop() {
  uint8_t keyCode = CHESS_KEY_NONE;
  
  u8g.firstPage();
  do {
    chess_Draw();
    uiStep();
    if ( uiKeyCode != CHESS_KEY_NONE )
      keyCode = uiKeyCode;
  } while( u8g.nextPage() );

  if(keyCode == CHESS_KEY_BACK) {
    delay(SW_MENU_DELAY);
    if(ui.confirm("Close without saving?","Quit game","Continue")) {
      delay(SW_MENU_DELAY);
      return false;
    }
  }

  //u8g_Delay(10);
  chess_Step(keyCode);
  uiStep();
  keyCode = uiKeyCode;
  return true;
}
void uiStep(void)
{
  uiKeyCodeSecond = uiKeyCodeFirst;
  int btn = keypad.getPressedButton();
  if ( btn == BTN_DOWN )
    uiKeyCodeFirst = CHESS_KEY_NEXT; // bugfix: inverted keys on menu
  else if ( btn == BTN_UP )
    uiKeyCodeFirst = CHESS_KEY_PREV;
  else if ( btn == BTN_SELECT )
    uiKeyCodeFirst = CHESS_KEY_SELECT;
  else if ( btn == BTN_BACK  )
    uiKeyCodeFirst = CHESS_KEY_BACK;
  else 
    uiKeyCodeFirst = CHESS_KEY_NONE;
  
  if ( uiKeyCodeSecond == uiKeyCodeFirst )
    uiKeyCode = uiKeyCodeFirst;
  else
    uiKeyCode = CHESS_KEY_NONE;
}
