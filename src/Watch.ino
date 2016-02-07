/* 
 * The BIG ToDo List 
 * - fix some horrible bugs: the Alarm bug (no sound)
 * - (maybe) save stopwatch data in EEPROM
 * - use Bluetooth menu (now empty) for Notifications and device info (BT firmware, module name...)
 * - add some apps (tiny games, MIDI player...) to the Extras menu (it'd be cool to be able to install apps via BT, but EEPROM is a little tiny...)
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
 * - add DatePicker, ProgressWindow (dialog with progress bar, for example for EEPROM reset or other long tasks)
 * - add nice Android-style dialog with radio buttons (and/or checkboxes)
 * - add progress screen (with a nice progress bar, like for Factory Reset)
 * - add alert screens with auto dismiss after X seconds
 * 
 * Once all the things on this list (which is going to grow) are done, CuniOS will be a nice DIY smartwatch OS. Don't worry, it'll take less than 2 centuries ;)
 * 
 * Happy hacking!
 */

// before compiling, please set your platform parameters parameters here
#include "Config.h"
 

// beginning of the REAL software, finally! :)

#if CUNI_OS_PLATFORM_ID == 2
  // Teensy-only stuff
  #include <Time.h>
  #include <Snooze.h>
#endif

#include <EEPROM.h>

#include CUNI_HW_BLUETOOTH_PATH
#include CUNI_HW_KEYPAD_PATH
#include CUNI_HW_RTC_PATH
#include CUNI_HW_GOVERNOR_PATH
#include CUNI_HW_EEPROM_PATH
#include CUNI_HW_TIMER_INTERRUPT_PATH

#include "LED.h"
// coming soon: Buzzer.h

#include "U8glib.h"
#include "StopWatch.h"
#include "CuniUI.h"
#include "Watchface.h"
#include "Bitmaps.h"




// uses both preprocessor macros and "compiler-side" integers to save space
const int BTN_BACK = CUNI_PIN_BTN_BACK;
const int BTN_SELECT = CUNI_PIN_BTN_SELECT;
const int BTN_UP = CUNI_PIN_BTN_UP;
const int BTN_DOWN = CUNI_PIN_BTN_DOWN;
const int BUZZER = CUNI_PIN_BUZZER;
const int LED_PIN = CUNI_PIN_LED_PIN;

const int EEPROM_ADDR_OS_VERSION = 0;
const int EEPROM_ADDR_ALARM_HOUR = 1;
const int EEPROM_ADDR_ALARM_MINUTE = 2;
const int EEPROM_ADDR_ALARM_ENABLED = 3;
const int EEPROM_ADDR_BT_ENABLED = 4;
const int EEPROM_ADDR_LATEST_WATCHFACE = 5;

const int SW_MENU_DELAY = CUNI_OS_MENU_DELAY; // in milliseconds, reducing it will reduce lag but make some actions, like menu navigation, more difficult
const int BUTTON_IDLE_TIMEOUT = CUNI_OS_TIMEOUT_BUTTONS; // in milliseconds, time after which the timer alert automatically dismisses
const unsigned long DISPLAY_TIMEOUT = 10000;
const int BTN_RADIUS = 1; // for CuniUILib
const int DISPLAY_WIDTH = CUNI_HW_DISPLAY_WIDTH;
const int DISPLAY_HEIGHT = CUNI_HW_DISPLAY_HEIGHT;
const bool PLL_SUPPORTED = CUNI_HW_PLL_SUPPORTED;
const int SERIAL_BAUD = CUNI_OS_SERIAL_SPEED;

boolean bluetooth_available = false;
boolean bluetoothOn = false;
boolean lostAlarm = false;
boolean ignoreAlarm = false;
boolean alarmEnabled = false;

char stopwatch_lap1[15] = "#1 --:--:--:--";
char stopwatch_lap2[15] = "#2 --:--:--:--";
char stopwatch_lap3[15] = "#3 --:--:--:--";
char stopwatch_lap4[15] = "#4 --:--:--:--"; // obviously, this could be managed much better!

int statusBarCounter = 0;
int alarmHour = 0;
int alarmMinute = 0;

int swHour = 0;
int swMinute = 0;
int swSecond = 0;
int swMillis = 0;

boolean timerSet = false;
unsigned long timerSeconds = 0;

// Chess variables
uint8_t uiKeyCodeFirst = CHESS_KEY_NONE;
uint8_t uiKeyCodeSecond = CHESS_KEY_NONE;
uint8_t uiKeyCode = CHESS_KEY_NONE;

CUNI_HW_DISPLAY_CONSTRUCTOR; // preprocessor macro, specified above

StopWatch sw;    // MILLIS (default)
StopWatch timerSW(StopWatch::SECONDS);

CUNI_HW_BLUETOOTH_NAME bt(9600);
CUNI_HW_KEYPAD_NAME keypad(BTN_BACK, BTN_SELECT, BTN_UP, BTN_DOWN);
CUNI_HW_RTC_NAME rtc;
CUNI_HW_EEPROM_NAME eeprom;
CUNI_HW_TIMER_INTERRUPT_NAME interruptTimer;
CUNI_HW_GOVERNOR_NAME pwrsave(keypad);

CuniUI ui(u8g, keypad, 128, 64);
LED notificationLED(LED_PIN);

Watchface watchface(u8g, rtc, eeprom, EEPROM_ADDR_LATEST_WATCHFACE);

void setup() {
  #if (CUNI_OS_PLATFORM_ID == 2)
    setSyncProvider(getTeensy3Time);
  #endif
  Serial.begin(SERIAL_BAUD);
  pinMode(BUZZER,OUTPUT);
  u8g.setColorIndex(1);
  
  clearScreen();
  
  if(keypad.isButtonPressed(CUNI_HW_KEYPAD_NAME::KEY_BUP) && keypad.isButtonPressed(CUNI_HW_KEYPAD_NAME::KEY_BDOWN)) {
    buzzerTone(350,200);
    buzzerTone(450,200);
    buzzerTone(550,200);

    recoveryMode(); // No, the code execution doesn't necessarily stop here...
  }
  boot();
  interruptTimer.start(interruptLoop);

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


    switch(progress) {
      case 0:
      // init various things
      if(eeprom.read(EEPROM_ADDR_OS_VERSION) != CUNI_OS_VERSION_ID) {
        // prevents incompatible systems, which may cause, for example, problems with different EEPROM addresses
        u8g.firstPage();
        do {
          u8g.setFont(u8g_font_helvB08);
          u8g.setFontPosTop();
          u8g.drawStr(0,2,"Fatal error");
          u8g.setFont(u8g_font_profont10);
          u8g.setFontPosTop();
          u8g.drawStr(0,12,"Incompatible system found");
          u8g.drawStr(0,20,"Please update the system");
          u8g.drawStr(0,28,"properly to continue.");
          u8g.drawStr(0,44,"Press UP+DOWN on boot");
          u8g.drawStr(0,52,"to enter Recovery Mode.");
          
        } while(u8g.nextPage());
        buzzerTone(300,1000);
        while(true) {
          // stop system
        }
      }
      
      break;

      case 1:
      // copy EEPROM to RAM
      alarmHour = eeprom.read(EEPROM_ADDR_ALARM_HOUR);
      alarmMinute = eeprom.read(EEPROM_ADDR_ALARM_MINUTE);
      bluetoothOn = (eeprom.read(EEPROM_ADDR_BT_ENABLED) == 1);
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
    if(progress == 3) break;
    progress++;
  }
  delay(2 * SW_MENU_DELAY);
}
void homeScreen() {
  boolean home = true;
  int homeCursor = 0;
  while(home) {
    int oldHomeCursor = homeCursor;
    int btn = keypad.getPressedButton();
    if(btn == BTN_DOWN) {
      if(homeCursor >= 3)
        homeCursor = 0;
      else
        homeCursor++;
    } else if(btn == BTN_UP) {
      if(homeCursor <= 0)
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
        ui.alert("", "Not supported yet :(", true, "Return", 5000);
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
  u8g.drawStr(50,0,rtc.getFormattedHour());
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
  if(bluetooth_available) {
    if(bluetoothOn) {
      if(ui.confirm("Disable Bluetooth?","OK","Cancel")) bluetoothOn = false;
    } else {
      if(ui.confirm("Enable Bluetooth?","OK","Cancel")) bluetoothOn = true;
    }
    eeprom.update(EEPROM_ADDR_BT_ENABLED,bluetoothOn);
  } else {
    ui.alert("No Bluetooth found","Please insert BT",true,"OK");
  }
}
void clock() {
  boolean clock = true;
  while(clock) {
    u8g.firstPage();
    do {  
        watchface.drawWatchFace();
    } while( u8g.nextPage());
    int btn = keypad.getPressedButton();
    if(btn == BTN_SELECT) {
      clicker();
      delay(SW_MENU_DELAY);
      clock = false;
    } else if(btn == BTN_DOWN) {
      watchface.previousWatchFace();
      delay(SW_MENU_DELAY);
    } else if(btn == BTN_UP) {
      watchface.nextWatchFace();
      delay(SW_MENU_DELAY);
    } else if(btn == BTN_BACK) {
      u8g.firstPage();
      clearScreen();
      delay(SW_MENU_DELAY * 2);
      pwrsave.sleepUntilInterrupt();
      #if CUNI_OS_PLATFORM_ID == 2
        setSyncProvider(Teensy3Time.get); // should solve the "frozen RTC" bug on Teensy
      #endif
    }
    delay(1);
  }
}
void watch() {
  boolean watch = true;
  int menuCursor = 0;
  while(watch) {
    int oldMenuCursor = menuCursor;
    int btn = keypad.getPressedButton();
    if(btn != 0) {
      clicker();
      if(btn == BTN_DOWN) {
        if(menuCursor >= 2)
          menuCursor = 0;
        else
          menuCursor++;
      } else if(btn == BTN_UP) {
        if(menuCursor <= 0)
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
          timer();
          break;
          case 2:
          delay(SW_MENU_DELAY);
          char txt[8];
          char txt2[19];
          sprintf(txt2,"Alarm set to %02d:%02d",alarmHour,alarmMinute);
          if(alarmEnabled) {
            sprintf(txt,"Disable");
          } else {
            sprintf(txt,"Enable");
          }
          int result;
          
          result = ui.dialog(txt2,"Set time",txt,true);
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
  char text[30];
  int alHour = alarmHour;
  int alMinute = alarmMinute;

  ui.timePicker(&alHour, &alMinute, alHour, alMinute);
  sprintf(text,"Set alarm to %02d:%02d?", alHour, alMinute);
  if(ui.confirm(text,"OK","Cancel")) {
    setAlarmTime(alHour, alMinute);
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
              sprintf(stopwatch_lap1,"#1 %s",text);
              break;
              case 1:
              sprintf(stopwatch_lap2,"#2 %s",text);
              break;
              case 2:
              sprintf(stopwatch_lap3,"#3 %s",text);
              break;
              case 3:
              sprintf(stopwatch_lap4,"#4 %s",text);
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
          sprintf(stopwatch_lap1,"#1 --:--:--:--");
          sprintf(stopwatch_lap2,"#2 --:--:--:--");
          sprintf(stopwatch_lap3,"#3 --:--:--:--");
          sprintf(stopwatch_lap4,"#4 --:--:--:--");
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
      u8g.drawStr(6,33,stopwatch_lap1);
      u8g.drawStr(6,42,stopwatch_lap2);
      u8g.drawStr(6,51,stopwatch_lap3);
      u8g.drawStr(6,60,stopwatch_lap4);
      
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
  unsigned long remainingSeconds = 0;
  char text[30];
  int timHour = 0;
  int timMinute = 0;
  int timSecond = 0;
  if(!timerSet) {
    int timerHour = 0;
    int timerMinute = 0;
    if(timerSW.isRunning()) {
      timerSW.stop();
      timerSW.reset();
    }
    timerHour = ui.hourPicker();
    int timerMinuteMin = 0;
    if(timerHour == 0) { 
      timerMinuteMin = 1;
    }
    timerMinute = ui.numberPicker(timerMinuteMin, 59, true, "minute");
    
    timerSeconds = (timerHour * 3600) + (timerMinute * 60);
    timerSW.start();
    timerSet = true;
  }
  while(timerSet && timerOn) {
    u8g.firstPage();
    do {
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
    
    int btnId = keypad.getPressedButton();
    if(btnId != 0) {
      if(btnId == BTN_BACK) {
        delay(SW_MENU_DELAY);
        timerOn = false;
      } else if(btnId == BTN_SELECT && !timerSW.isRunning()) {
        timerSW.start();
        delay(SW_MENU_DELAY);
      } else if(btnId == BTN_SELECT && timerSW.isRunning()) {
        timerSW.stop();
        delay(SW_MENU_DELAY);
      }
      else if(btnId == BTN_DOWN && !timerSW.isRunning()) {
        timerSW.reset();
        delay(SW_MENU_DELAY);
        timerSeconds = 0;
        timSecond = 0;
        timerOn = false;
        timerSet = false;
      }
    }
  }
}
void recoveryMode() {
  u8g.firstPage();
  do {
    u8g.drawXBMP(0,0,BMP_BOOT_SIZE[0],BMP_BOOT_SIZE[1],BMP_BOOT);
    u8g.setFont(u8g_font_helvB08);
    u8g.setFontPosTop();
    u8g.drawStr(((DISPLAY_WIDTH - u8g.getStrWidth("Recovery Mode"))/2),50,"Recovery Mode");
    
  } while(u8g.nextPage());
  delay(2000);
  
  while(true) {
    u8g.firstPage();
    do {
      u8g.setFont(u8g_font_helvB08);
      u8g.setFontPosTop();
      u8g.drawStr(0,3,"Recovery not available");
      u8g.setFont(u8g_font_helvR08);
      u8g.setFontPosTop();
      u8g.drawStr(0,17,"Press Back to quit");
      
    } while(u8g.nextPage());
    if(keypad.isButtonPressed(CUNI_HW_KEYPAD_NAME::KEY_BACK)) break;
  } // can be easily quit using break;
}



void buzzerTone(int toneValue, int time) { // todo: implement as a class
  tone(BUZZER,toneValue, time);
}
void clicker() {
  buzzerTone(70,15);
}



void interruptLoop() {
  Serial.println(keypad.getButtonIdleTime());
  if(keypad.getButtonIdleTime() > DISPLAY_TIMEOUT) {
    clearScreen();
    pwrsave.sleepUntilInterrupt(); // TODO: screen fails here after waking up!!!
  }
  isTimer();
  if(alarmEnabled && rtc.getHour() == alarmHour && rtc.getMinute() == alarmMinute) {
    if(lostAlarm) {
      lostAlarm = false;
    }
    alarmLoop();
  }

}
void isTimer() {
  if(timerSW.isRunning()) {
    if(timerSW.elapsed() >= timerSeconds) {
      timerSW.stop();
      timerSW.reset();
      timerSeconds = 0;
      timerSet = false;
      unsigned long timeoutOffset = millis();
      while(true) {
        u8g.firstPage();
        do {
          drawStatusBar();
          u8g.setFont(u8g_font_helvB08);
          u8g.setFontPosTop();
          u8g.drawStr(35,25,"Time is up!");
          u8g.setFont(u8g_font_helvR08);
          u8g.setFontPosTop();
          u8g.drawStr(28,40,"Press any key...");
        } while(u8g.nextPage());
        if((millis() - timeoutOffset % 2000) < 50) {
          notificationLED.enable(255);
          buzzerTone(300,50);
          delay(80);
          notificationLED.disable();
          buzzerTone(300,50);
        }
        if(millis() - timeoutOffset > BUTTON_IDLE_TIMEOUT) {
          break;
        }
        if(keypad.getPressedButton() != 0) {
          delay(SW_MENU_DELAY);
          break;
        }
      }
    }
  }
}

void clearScreen() {
  u8g.firstPage();
  do {
    // clear screen
  } while(u8g.nextPage());
}

void updateAlarmEnabled() {
  if(eeprom.read(EEPROM_ADDR_ALARM_ENABLED) == 1) {
    alarmEnabled = true;
  } else {
    alarmEnabled = false;
  }  
}
void toggleAlarmEnabled() {
  ignoreAlarm = false;
  if(eeprom.read(EEPROM_ADDR_ALARM_ENABLED) == 1) {
    alarmEnabled = false;
    eeprom.update(EEPROM_ADDR_ALARM_ENABLED,0);
  } else {
    eeprom.update(EEPROM_ADDR_ALARM_ENABLED,1);
    alarmEnabled = true;
  }  
}
void setAlarmTime(int alHour, int alMinute) {
  eeprom.update(EEPROM_ADDR_ALARM_HOUR,alHour);
  eeprom.update(EEPROM_ADDR_ALARM_MINUTE,alMinute);
  alarmHour = eeprom.read(EEPROM_ADDR_ALARM_HOUR);
  alarmMinute = eeprom.read(EEPROM_ADDR_ALARM_MINUTE);
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
