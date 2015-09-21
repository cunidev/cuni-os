/*
 * LED tool
 */

#ifndef LED_h

#include "Arduino.h"
#define Bluetooth_h

class LED {
  public:
    LED(int pin);
    void enable(int strength);
    void disable();
    void tick(int milliseconds, int strength = 0);
    void breatheStart(int maxStrength, int stepsPerTick, int cycleEndPauseTicks, int cycleBeginningPauseTicks);
    void breatheStep();
    void breatheStop();
  private:
    int ledPin;
    int breatheMaxStrength;
    int breatheStepsPerTick;
    long breatheCycleEndPauseTicks;
    long breatheCycleBeginningPauseTicks;
    long breatheTickStatus;
};

#endif
// end of file
