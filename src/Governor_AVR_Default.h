#include "Config.h"

#ifndef Governor_AVR_h
#define Governor_AVR_h

#define CUNI_HW_GOVERNOR_NAME Governor_AVR
#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include CUNI_HW_KEYPAD_PATH

class Governor_AVR {
  public:
    Governor_AVR(CUNI_HW_KEYPAD_NAME &keypad);
    void sleepUntilInterrupt();
  private:
    CUNI_HW_KEYPAD_NAME &_keypad;
};

#endif
