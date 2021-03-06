#ifndef CuniUI_h
#define CuniUI_h

#include "Arduino.h"
#include "U8glib.h"
#include "Config.h"
#include CUNI_HW_KEYPAD_PATH

class CuniUI {
  public:
    CuniUI(U8GLIB &u8gl, CUNI_HW_KEYPAD_NAME &keys, int display_w, int display_h);
    void alert(char title[], char text[], boolean showButton, char btnText[], unsigned long dismissTimeout);
    void alert(char title[], char text[], boolean showButton, char btnText[]);
    void alert(char title[], char text[], boolean showButton);
    void alert(char title[], char text[], char btnText[]);
    void alert(char title[], char text[]);
    void alert(char text[]);
    
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
    CUNI_HW_KEYPAD_NAME &keypad;
    int display_width;
    int display_height;
    const int BTN_RADIUS = 1; // you can edit this to edit button radius
    const int SW_MENU_DELAY = 200; // delay in menus
    
};

#endif
