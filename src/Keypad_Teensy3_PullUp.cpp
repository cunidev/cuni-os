#include "Config.h"

#include "Keypad_Teensy3_PullUp.h"

Teensy3Keypad::Teensy3Keypad(int back, int select, int up, int down) {
  pin_back = back;
  pin_select = select;
  pin_up = up;
  pin_down = down;
  pinMode(back,INPUT_PULLUP);
  pinMode(select,INPUT_PULLUP);
  pinMode(up,INPUT_PULLUP);
  pinMode(down,INPUT_PULLUP);
}
int Teensy3Keypad::getPressedButton() {
  if(Teensy3Keypad::isButtonPressed(Teensy3Keypad::KEY_BACK)) return pin_back;
  if(Teensy3Keypad::isButtonPressed(Teensy3Keypad::KEY_SELECT)) return pin_select;
  if(Teensy3Keypad::isButtonPressed(Teensy3Keypad::KEY_BUP)) return pin_up;
  if(Teensy3Keypad::isButtonPressed(Teensy3Keypad::KEY_BDOWN)) return pin_down;
  return 0;
}
boolean Teensy3Keypad::isButtonPressed(enum Buttons btn_label) {
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
  return (digitalRead(checkPin) == LOW);
}

// multi-button could be easily implemented, to replace this ugly priority-based system
