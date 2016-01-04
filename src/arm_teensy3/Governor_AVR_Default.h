#ifndef Governor_AVR_h
#define Governor_AVR_h

#define CUNI_HW_GOVERNOR_NAME Governor_AVR
#include <Arduino.h>

class Governor_AVR {
  public:
    void sleepUntilButtonWake(int buttonPin);
};

#endif
