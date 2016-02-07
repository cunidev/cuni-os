#ifndef Keypad_h
#define Keypad_h
#define CUNI_HW_KEYPAD_NAME SerialKeypad

#include "Arduino.h"

class SerialKeypad {
  public:
    SerialKeypad(int back, int select, int up, int down);
    enum Buttons { KEY_BACK, KEY_SELECT, KEY_BUP, KEY_BDOWN };
    int getPressedButton();
    boolean isButtonPressed(enum Buttons btn_label);
    unsigned long getButtonIdleTime();
    
    int pin_back;
    int pin_select;
    int pin_up;
    int pin_down;
  private:
    void _updateLastButtonTime();
    unsigned long _lastButtonTime = 0;
};

#endif
