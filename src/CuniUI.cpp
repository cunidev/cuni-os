#include "CuniUI.h"
#include <Time.h>
/*
 * Cuni UI Library - first beta version
 * Seems to work properly for the moment... But I haven't done enough tests on it to confirm it does
 * 
 * TODO:
 * - make it work, first ;)
 * - alert(...) should have many constructors!
 * - alert(...): check for events; boolean to be able to use the back key to dismiss; auto-dismiss in N seconds
 * - (maybe) add clicker sound to be enabled or disabled with functions
 */
void CuniUI::alert(char title[], char text[], boolean showButton, char btnText[]) {
  while(true) {
    u8g.firstPage();
    do {
      // todo: must check for events (alarm etc.)!! isAlarm();
      //drawStatusBar();
      u8g.setFont(u8g_font_helvB08);
      u8g.setFontPosTop();
      u8g.drawStr(floor( ( display_width - u8g.getStrWidth(title) ) / 2 ),17,title);
      u8g.setFont(u8g_font_helvR08);
      u8g.setFontPosTop();
      u8g.drawStr(floor( ( display_width - u8g.getStrWidth(text) ) / 2 ),28,text);

      if(showButton) {
        u8g.drawRBox(18, 48, 90, 16, BTN_RADIUS);     
        u8g.setDefaultBackgroundColor();
        u8g.setFont(u8g_font_helvB08);
        u8g.setFontPosTop();
        u8g.drawStr(floor((display_width - u8g.getStrWidth(btnText))/2 ), 51, btnText);
        u8g.setDefaultForegroundColor();
      }
    } while(u8g.nextPage());
    if(keypad.getPressedButton() == keypad.pin_select || (showButton == false && keypad.getPressedButton() != 0)) {
      delay(SW_MENU_DELAY);
      break;
    }
  }

}

int CuniUI::dialog(char text[], char btnYes[], char btnNo[], boolean allowCancel) {
  int cursor = 0;
  while(true) {
    // TODO: check for events
    u8g.firstPage();
    do {
      u8g.setFont(u8g_font_helvR08);
      u8g.setFontPosTop();
      int x = floor( ( display_width - u8g.getStrWidth(text) ) / 2 );
      u8g.drawStr(x,20,text);

      u8g.setFont(u8g_font_helvR08);
      x = floor(2 + ((57 - u8g.getStrWidth(btnYes))/2 ));
      u8g.setFontPosTop();
      if(cursor == 0) {
        u8g.drawRBox(2, 48, 60, 16, BTN_RADIUS); // X pixel range for font: 2->59
        
        u8g.setDefaultBackgroundColor();
        u8g.setFont(u8g_font_helvB08);
        u8g.setFontPosTop();
        u8g.drawStr(x, 51, btnYes);
        u8g.setFont(u8g_font_helvR08);
        u8g.setDefaultForegroundColor();
      } else {
        u8g.drawRFrame(2, 48, 60, 16, BTN_RADIUS); // X pixel range for font: 2->59
        
        u8g.drawStr(x, 51, btnYes);
      }
      
      x = floor(66 + ((57 - u8g.getStrWidth(btnNo))/2 ));
      u8g.setFontPosTop();
      if(cursor == 1) {
        u8g.drawRBox(66, 48, 60, 16, BTN_RADIUS);  // X pixel range for font: 66->123
        
        u8g.setDefaultBackgroundColor();
        u8g.setFont(u8g_font_helvB08);
        u8g.setFontPosTop();
        u8g.drawStr(x, 51, btnNo);
        u8g.setFont(u8g_font_helvR08);
        u8g.setDefaultForegroundColor();
      } else {
        u8g.drawRFrame(66, 48, 60, 16, BTN_RADIUS);  // X pixel range for font: 66->123
        u8g.drawStr(x, 51, btnNo);
      }
      u8g.setDefaultForegroundColor();
    } while(u8g.nextPage());
    int btnID = keypad.getPressedButton();
    if(btnID != 0) {
      if(btnID == keypad.pin_back && allowCancel) {
        delay(SW_MENU_DELAY);
        return 0;
      } else if(btnID == keypad.pin_down || btnID == keypad.pin_up) {
        delay(SW_MENU_DELAY/2);
        if(cursor == 0) cursor = 1;
        else cursor = 0;
      } else if(btnID == keypad.pin_select) {
        delay(SW_MENU_DELAY);
        cursor++;
        return cursor;
      }
    }
  }
  return 0;
}

boolean CuniUI::confirm(char text[], char btnYes[], char btnNo[]) {
  boolean waitingConfirm = true;
  boolean result;
  int cursor = 0;
  while(waitingConfirm) {
    // TODO: check for events! isAlarm();
    u8g.firstPage();
    do {
      u8g.setFont(u8g_font_helvR08);
      u8g.setFontPosTop();
      int x = floor( ( display_width - u8g.getStrWidth(text) ) / 2 );
      u8g.drawStr(x,20,text);

      u8g.setFont(u8g_font_helvR08);
      x = floor(2 + ((57 - u8g.getStrWidth(btnYes))/2 ));
      u8g.setFontPosTop();
      if(cursor == 0) {
        u8g.drawRBox(2, 48, 60, 16, BTN_RADIUS); // X pixel range for font: 2->59
        
        u8g.setDefaultBackgroundColor();
        u8g.setFont(u8g_font_helvB08);
        u8g.setFontPosTop();
        u8g.drawStr(x, 51, btnYes);
        u8g.setFont(u8g_font_helvR08);
        u8g.setDefaultForegroundColor();
      } else {
        u8g.drawRFrame(2, 48, 60, 16, BTN_RADIUS); // X pixel range for font: 2->59
        
        u8g.drawStr(x, 51, btnYes);
      }
      
      x = floor(66 + ((57 - u8g.getStrWidth(btnNo))/2 ));
      u8g.setFontPosTop();
      if(cursor == 1) {
        u8g.drawRBox(66, 48, 60, 16, BTN_RADIUS);  // X pixel range for font: 66->123
        
        u8g.setDefaultBackgroundColor();
        u8g.setFont(u8g_font_helvB08);
        u8g.setFontPosTop();
        u8g.drawStr(x, 51, btnNo);
        u8g.setFont(u8g_font_helvR08);
        u8g.setDefaultForegroundColor();
      } else {
        u8g.drawRFrame(66, 48, 60, 16, BTN_RADIUS);  // X pixel range for font: 66->123
        u8g.drawStr(x, 51, btnNo);
      }
      u8g.setDefaultForegroundColor();
    } while(u8g.nextPage());
    int btnID = keypad.getPressedButton();
    if(btnID != 0) {
      if(btnID == keypad.pin_back) {
        delay(SW_MENU_DELAY);
        result = false;
        waitingConfirm = false;
      } else if(btnID == keypad.pin_down || btnID == keypad.pin_up) {
        delay(SW_MENU_DELAY/2);
        if(cursor == 0) cursor = 1;
        else cursor = 0;
      } else if(btnID == keypad.pin_select) {
        delay(SW_MENU_DELAY);
        if(cursor == 0) result = true;
        else result = false;
        waitingConfirm = false;
      }
    }
  }
  return result;
}

int CuniUI::numberPicker(int minValue, int maxValue, int offsetValue, boolean showZeros, char label[]) {
  int value = offsetValue;
  // todo: set up a decent leading zero algorithm!!! (now it's fixed to 2 leading zeros, while it should count maxValue size and translate it in leading zeros)
  while(true) {
    int maxValueLength = ceil(8*sizeof(maxValue) * log10(2)); // to set up the number of zeroes
    char numStr[maxValueLength+1];
    u8g.firstPage();
    if(showZeros) {
      sprintf(numStr,"%02d",value);
    } else {
      sprintf(numStr,"%d",value);
    }
    
    do {
      u8g.setFont(u8g_font_fub49n);
      u8g.setFontPosTop();
      int x = floor( ( display_width - u8g.getStrWidth(numStr) ) / 2 );
      u8g.drawStr(x,5,numStr);
      
      u8g.setFont(u8g_font_helvR08);
      u8g.setFontPosTop();
      int y = floor( ( display_height - u8g.getStrWidth(label) ) / 2 );
      u8g.drawStr90(10,y,label);
    } while(u8g.nextPage());
    int btnID = keypad.getPressedButton();
    if(btnID == 0) {
      // no keys pressed
    } else if(btnID == keypad.pin_up) {
      value++;
    } else if(btnID == keypad.pin_down) {
      value--;
    } else if(btnID == keypad.pin_select) {
        delay(SW_MENU_DELAY);
        return value;
        break;
    } else {
      // should theorically NEVER go here
    }
    delay(SW_MENU_DELAY/2);

    if(value > maxValue) value = minValue;
    if(value < minValue) value = maxValue;
  }
  return value;
}
int CuniUI::numberPicker(int minValue, int maxValue, int offset, boolean showZeros) {
  return numberPicker(minValue, maxValue, offset, showZeros, "");
}
int CuniUI::numberPicker(int minValue, int maxValue, boolean showZeros, char label[]) {
  return numberPicker(minValue, maxValue, minValue, showZeros, label);
}
int CuniUI::numberPicker(int minValue, int maxValue, char label[]) {
  return numberPicker(minValue, maxValue, minValue, false, label);
}

int CuniUI::hourPicker(int offset, char label[]) {
  return numberPicker(0, 23, offset, true, label);
}
int CuniUI::hourPicker(char label[]) {
  return numberPicker(0, 23, 0, true, label);
}
int CuniUI::hourPicker() {
  return numberPicker(0, 23, 0, true, "hour");
}

int CuniUI::minutePicker(int offset, char label[]) {
  return numberPicker(0, 59, offset, true, label);
}
int CuniUI::minutePicker(char label[]) {
  return numberPicker(0, 59, 0, true, label);
}
int CuniUI::minutePicker() {
  return numberPicker(0, 59, 0, true, "minute");
}
void CuniUI::timePicker(int *hour, int *minute, int offset_hour, int offset_minute, char label_hour[], char label_minute[]) {
  *hour = hourPicker(offset_hour, label_hour);
  *minute = minutePicker(offset_minute, label_minute);
}

void CuniUI::timePicker(int *hour, int *minute, char label_hour[], char label_minute[]) {
  *hour = hourPicker(label_hour);
  *minute = minutePicker(label_minute);
}
void CuniUI::timePicker(int *hour, int *minute, int offset_hour, int offset_minute) {
  *hour = hourPicker(offset_hour, "hour");
  *minute = minutePicker(offset_minute, "minute");
}
void CuniUI::timePicker(int *hour, int *minute) {
  (*hour) = hourPicker();
  (*minute) = minutePicker();
}

