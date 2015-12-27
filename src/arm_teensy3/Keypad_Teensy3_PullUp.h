// TODO: handle buttons here
// NOTE: the migration to this library is still a work-in-progress! For instance, after migration getPressedButton should return an enum
#ifndef Keypad_h

#include "Arduino.h"
#define Keypad_h

class ModKeypad {
  public:
    enum Buttons { KEY_BACK, KEY_SELECT, KEY_BUP, KEY_BDOWN };
    ModKeypad(int back, int select, int up, int down);
    int getPressedButton();
    boolean isButtonPressed(enum Buttons btn_label);
    
    int pin_back;
    int pin_select;
    int pin_up;
    int pin_down;
};

#endif
