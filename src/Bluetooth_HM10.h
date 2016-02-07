#ifndef Bluetooth_h
#define Bluetooth_h
#define CUNI_HW_BLUETOOTH_NAME HM10

#include "Arduino.h"

class HM10 {
  public:
    HM10(int speed);
    boolean isReady();
    // boolean setName(char *name);
    // boolean setPin(unsigned int pin);
    // boolean setBaudRate(int speed);
    char *getString();
    char *waitString(char *text, char *timeout_ms);
    void write(char *text);
    char *getFirmwareVersion();
  private:
    boolean baudRate;
};

#endif
// end of file
