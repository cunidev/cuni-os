#include "Config.h"

#include "Keypad_Serial.h"

SerialKeypad::SerialKeypad(int back, int select, int up, int down) {
  pin_back = back;
  pin_select = select;
  pin_up = up;
  pin_down = down;
  pinMode(back,INPUT);
  pinMode(select,INPUT);
  pinMode(up,INPUT);
  pinMode(down,INPUT);
}
int SerialKeypad::getPressedButton() {
  if(SerialKeypad::isButtonPressed(SerialKeypad::KEY_BACK)) return pin_back;
  if(SerialKeypad::isButtonPressed(SerialKeypad::KEY_SELECT)) return pin_select;
  if(SerialKeypad::isButtonPressed(SerialKeypad::KEY_BUP)) return pin_up;
  if(SerialKeypad::isButtonPressed(SerialKeypad::KEY_BDOWN)) return pin_down;
  return 0;
}
boolean SerialKeypad::isButtonPressed(enum Buttons btn_label) {
  if(Serial.available()>0) {
    _updateLastButtonTime();
    char in;
    switch(btn_label) {
      case KEY_BACK:
      in = 'b';
      break; 
      case KEY_SELECT:
      in = 's';
      break;
      case KEY_BUP:
      in = 'u';
      break;
      case KEY_BDOWN:
      in = 'd';
      break;
      Serial:
      in = 'n';
    }
    if(Serial.read() == in) return true;
  }
  return false;
}
unsigned long SerialKeypad::getButtonIdleTime() {
  return millis() - _lastButtonTime;
}
void SerialKeypad::_updateLastButtonTime() {
  _lastButtonTime = millis();
}
