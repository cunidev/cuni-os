/*
 * Power saving library, basically handles sleep mode
 * (can be adapted to different processors, like Atmel MCUs, very easily)
 */

#ifndef PowerSaver_h
#define PowerSaver_h
#include <Arduino.h>
#include <Snooze.h>

class PowerSave {
  public:
    void sleepUntilButtonWake(int buttonPin);
  private:
    SnoozeBlock snooze;
};

#endif
