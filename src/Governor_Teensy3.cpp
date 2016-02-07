#include "Config.h"
#if CUNI_OS_PLATFORM_ID == 2
#include "Governor_Teensy3.h"

PowerSave::PowerSave(CUNI_HW_KEYPAD_NAME &keypad) : _keypad(keypad) {
  
}
void PowerSave::sleepUntilInterrupt() {
  snooze.pinMode(_keypad.pin_back, INPUT_PULLUP, RISING);
  snooze.pinMode(_keypad.pin_select, INPUT_PULLUP, RISING);
  snooze.pinMode(_keypad.pin_up, INPUT_PULLUP, RISING);
  snooze.pinMode(_keypad.pin_down, INPUT_PULLUP, RISING);
  Snooze.sleep(snooze);
}

#endif
