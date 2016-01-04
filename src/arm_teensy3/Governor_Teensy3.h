/*
 * Power saving library, basically handles sleep mode
 * (can be adapted to different processors, like Atmel MCUs, very easily)
 */

#if CUNI_OS_PLATFORM_ID == 2 // Teensy only!
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
#endif
