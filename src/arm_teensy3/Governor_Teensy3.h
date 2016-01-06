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
#include <Time.h>
#include CUNI_HW_KEYPAD_PATH

class PowerSave {
  public:
    PowerSave(CUNI_HW_KEYPAD_NAME &keypad);
    void sleepUntilInterrupt();
  private:
    SnoozeBlock snooze;
    CUNI_HW_KEYPAD_NAME &_keypad;
};

#endif
#endif
