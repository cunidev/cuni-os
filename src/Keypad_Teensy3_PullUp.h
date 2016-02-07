#ifndef Keypad_h
#define Keypad_h
#define CUNI_HW_KEYPAD_NAME Teensy3Keypad

#include "Arduino.h"

class Teensy3Keypad {
  public:
    Teensy3Keypad(int back, int select, int up, int down);
    enum Buttons { KEY_BACK, KEY_SELECT, KEY_BUP, KEY_BDOWN };
    int getPressedButton();
    boolean isButtonPressed(enum Buttons btn_label);
    
    int pin_back;
    int pin_select;
    int pin_up;
    int pin_down;
};

#endif
