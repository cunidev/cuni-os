/*
 * Tiny Teensy library for the HC-06 Bluetooth module
 */
#ifndef Bluetooth_h
#include "Arduino.h"

#define Bluetooth_h

class Bluetooth {
  public:
    Bluetooth(int speed);
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
