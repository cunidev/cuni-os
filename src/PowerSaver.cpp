#include "PowerSaver.h"

void PowerSave::sleepUntilButtonWake(int buttonPin) {
  snooze.pinMode(buttonPin, INPUT_PULLUP, RISING);
  Snooze.deepSleep(snooze);
}

