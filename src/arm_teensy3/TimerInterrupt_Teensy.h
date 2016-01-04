#include "Config.h"

#if CUNI_OS_PLATFORM_ID == 2 // Teensy only!
#ifndef TimerInterrupt_h
#define TimerInterrupt_h
#define CUNI_HW_TIMER_INTERRUPT_NAME TeensyTimerInterrupt

#include <Arduino.h>

class TeensyTimerInterrupt {
  public:
    TeensyTimerInterrupt();
    void start(void (*function)());
    void setCallback(void (*function)());
    void enable();
    void disable();
    void setFrequency(unsigned long freq);
    void _callback();
  private:
    void (*_callbackFunction)(void);
    IntervalTimer _timer;
    boolean _interruptEnabled;
    unsigned long _interruptFrequency = 1000000;
    
};

#endif
#endif
