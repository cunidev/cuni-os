#include "Config.h"
#if CUNI_OS_PLATFORM_ID == 1
#include "TimerInterrupt_AVR.h"

AVRTimerInterrupt::AVRTimerInterrupt() {
}
void AVRTimerInterrupt::start(void (*function)()) {
  // nothing
}
void AVRTimerInterrupt::setCallback(void (*function)()) {
  _callbackFunction = function;
}
void AVRTimerInterrupt::enable() {
  interrupts();
  // not using interrupts() and noInterrupts() as they seem to make some troubles...
  _interruptEnabled = true;
}
void AVRTimerInterrupt::disable() {
  noInterrupts();
  _interruptEnabled = false;
}
void AVRTimerInterrupt::setFrequency(unsigned long freq) {
  _interruptFrequency = freq; // doesn't work now
}
void AVRTimerInterrupt::_callback() { // TODO: FIX HERE
  if(_interruptEnabled) {
    //_callbackFunction();
  }
}

#endif
