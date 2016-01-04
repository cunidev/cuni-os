#include "Config.h"
#if CUNI_OS_PLATFORM_ID == 2
#include "Governor_Teensy3.h"

void PowerSave::sleepUntilButtonWake(int buttonPin) {
  snooze.pinMode(buttonPin, INPUT_PULLUP, RISING);
  Snooze.deepSleep(snooze);
}

#endif
