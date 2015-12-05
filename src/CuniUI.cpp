#include "CuniUI.h"

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

void CuniUI::drawTest() {
  u8g.firstPage();
  do {
    u8g.drawBox(10,10,20,30);
  } while(u8g.nextPage());
}
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
