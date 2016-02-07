#include "Config.h"
#ifndef TimerInterrupt_h
#define TimerInterrupt_h
#define CUNI_HW_TIMER_INTERRUPT_NAME AVRTimerInterrupt

#include <Arduino.h>

class AVRTimerInterrupt {
  public:
    AVRTimerInterrupt();
    void start(void (*function)());
    void setCallback(void (*function)());
    void enable();
    void disable();
    void setFrequency(unsigned long freq);
    void _callback();
  private:
    void (*_callbackFunction)(void);
    boolean _interruptEnabled;
    unsigned long _interruptFrequency = 1000000;
    
};

#endif
