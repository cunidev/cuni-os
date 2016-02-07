// handle eeprom functions here: more versatility on all platforms, without sticking to Arduino libraries
// this class is fully compatible with Arduino EEPROM Library: on Arduino, it simply links to EEPROM.h functions
#ifndef CuniEEPROM_h
#define CuniEEPROM_h
#define CUNI_HW_EEPROM_NAME CuniEEPROM

#include "Arduino.h"
#include <EEPROM.h>

class CuniEEPROM {
  public:
    CuniEEPROM();
    byte read(int address);
    uint16_t length();
    void write(int address, uint8_t value);
    void update(int address, uint8_t value);
};


#endif
