#include "Config.h"

#include "Keypad_Default_PullUp.h"

PullUpKeypad::PullUpKeypad(int back, int select, int up, int down) {
  pin_back = back;
  pin_select = select;
  pin_up = up;
  pin_down = down;
  pinMode(back,INPUT_PULLUP);
  pinMode(select,INPUT_PULLUP);
  pinMode(up,INPUT_PULLUP);
  pinMode(down,INPUT_PULLUP);
}
int PullUpKeypad::getPressedButton() {
  int return_pin = 0;
  if(PullUpKeypad::isButtonPressed(PullUpKeypad::KEY_BACK)) {
    return_pin = pin_back;
  }
  if(PullUpKeypad::isButtonPressed(PullUpKeypad::KEY_SELECT)) {
    return_pin = pin_select;
  }
  if(PullUpKeypad::isButtonPressed(PullUpKeypad::KEY_BUP)) {
    return_pin = pin_up;
  }
  if(PullUpKeypad::isButtonPressed(PullUpKeypad::KEY_BDOWN)) {
    return_pin = pin_down;
  }
  if(return_pin != 0) {
    _updateLastButtonTime();
  }
  return return_pin;
}

boolean PullUpKeypad::isButtonPressed(enum Buttons btn_label) {
  int checkPin;
  switch(btn_label) {
    case KEY_BACK:
    checkPin = pin_back;
    break; 
    case KEY_SELECT:
    checkPin = pin_select;
    break;
    case KEY_BUP:
    checkPin = pin_up;
    break;
    case KEY_BDOWN:
    checkPin = pin_down;
    break;
    
  }
  boolean isButtonPressed = (digitalRead(checkPin) == LOW);
  if(isButtonPressed) {
    _updateLastButtonTime();
  }
  return isButtonPressed;
}
unsigned long PullUpKeypad::getButtonIdleTime() {
  return millis() - _lastButtonTime;
}

void PullUpKeypad::_updateLastButtonTime() {
  _lastButtonTime = millis();
}
