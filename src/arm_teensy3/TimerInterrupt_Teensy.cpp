#include "Config.h"
#if CUNI_OS_PLATFORM_ID == 2
#include "TimerInterrupt_Teensy.h"

TeensyTimerInterrupt::TeensyTimerInterrupt() {
}
void TeensyTimerInterrupt::start(void (*function)()) {
  enable();
  setCallback(function);
  _timer.begin(_callbackFunction,_interruptFrequency);
}
void TeensyTimerInterrupt::setCallback(void (*function)()) {
  _callbackFunction = function;
}
void TeensyTimerInterrupt::enable() {
  interrupts();
  // not using interrupts() and noInterrupts() as they seem to make some troubles...
  _interruptEnabled = true;
}
void TeensyTimerInterrupt::disable() {
  noInterrupts();
  _interruptEnabled = false;
}
void TeensyTimerInterrupt::setFrequency(unsigned long freq) {
  _interruptFrequency = freq; // doesn't work now
}
void TeensyTimerInterrupt::_callback() { // TODO: FIX HERE
  if(_interruptEnabled) {
    //_callbackFunction();
  }
}

#endif
