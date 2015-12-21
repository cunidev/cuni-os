#include "CuniEEPROM.h"

CuniEEPROM::CuniEEPROM() {
  // init eeprom if necessary
}
byte CuniEEPROM::read(int address) {
  return EEPROM.read(address);
}
uint16_t CuniEEPROM::length() {
  return EEPROM.length();
}
void CuniEEPROM::write(int address, uint8_t value) {
  EEPROM.write(address,value);
}
void CuniEEPROM::update(int address, uint8_t value) {
  EEPROM.update(address,value);
}

