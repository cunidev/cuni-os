#ifndef Governor_AVR_h
#define Governor_AVR_h

#define CUNI_HW_GOVERNOR_NAME Governor_AVR
#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/power.h>

class Governor_AVR {
  public:
    void sleepUntilButtonWake();
};

#endif
