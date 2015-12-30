/*
 * Power saving library, basically handles sleep mode
 * (can be adapted to different processors, like Atmel MCUs, very easily)
 */

#ifndef PowerSave_h
#define PowerSave_h
#define CUNI_HW_GOVERNOR_NAME PowerSave
#include <Arduino.h>
#include <Snooze.h>

class PowerSave {
  public:
    void sleepUntilButtonWake(int buttonPin);
  private:
    SnoozeBlock snooze;
};

#endif
