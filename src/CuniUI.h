#include "Arduino.h"
#include "U8glib.h"
#include "Buttons.h"

class CuniUI {
  public:
    CuniUI(U8GLIB &u8gl, ModKeypad &keys, int display_w, int display_h) : u8g(u8gl), keypad(keys) {   display_width = display_w; display_height = display_h; };
    void drawTest();
    void alert(char title[], char text[], boolean showButton, char btnText[]);
    int dialog(char text[], char btnYes[], char btnNo[], boolean allowCancel);
    boolean confirm(char text[], char btnYes[], char btnNo[]);
  private:
    U8GLIB &u8g;
    ModKeypad &keypad;
    int display_width;
    int display_height;
    const int BTN_RADIUS = 1; // you can edit this to edit button radius
    const int SW_MENU_DELAY = 200; // delay in menus
    
};
