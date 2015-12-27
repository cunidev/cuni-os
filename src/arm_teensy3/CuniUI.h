#ifndef CuniUI_h
#define CuniUI_h

#include "Arduino.h"
#include "U8glib.h"
#include "Keypad_Teensy3_PullUp.h"

class CuniUI {
  public:
    CuniUI(U8GLIB &u8gl, ModKeypad &keys, int display_w, int display_h) : u8g(u8gl), keypad(keys) {   display_width = display_w; display_height = display_h; };
    void alert(char title[], char text[], boolean showButton, char btnText[]);
    int dialog(char text[], char btnYes[], char btnNo[], boolean allowCancel);
    boolean confirm(char text[], char btnYes[], char btnNo[]);
    
    int numberPicker(int minValue, int maxValue, int offset, boolean showZeros, char label[]); // todo: implement default parameters DECENTLY!
    int numberPicker(int minValue, int maxValue, int offset, boolean showZeros);
    int numberPicker(int minValue, int maxValue, boolean showZeroes, char label[]);
    int numberPicker(int minValue, int maxValue, char label[]);
    
    int hourPicker(int offset, char label[]);    
    int hourPicker(char label[]);
    int hourPicker();
    int minutePicker(char label[]);
    int minutePicker(int offset, char label[]);
    int minutePicker();
    void timePicker(int *hour, int *minute, int offset_hour, int offset_minute, char label_hour[], char label_minute[]);
    void timePicker(int *hour, int *minute, char label_hour[], char label_minute[]);
    void timePicker(int *hour, int *minute, int offset_hour, int offset_minute);
    void timePicker(int *hour, int *minute);
    
  private:
    U8GLIB &u8g;
    ModKeypad &keypad;
    int display_width;
    int display_height;
    const int BTN_RADIUS = 1; // you can edit this to edit button radius
    const int SW_MENU_DELAY = 200; // delay in menus
    
};

#endif
