#ifndef Bitmaps_h
#define Bitmaps_h

#include <Arduino.h>

extern const uint8_t BMP_BOOT[] PROGMEM;

extern const uint8_t BMP_HOME_CLOCK[] PROGMEM;
extern const uint8_t BMP_HOME_BLUETOOTH[] PROGMEM;
extern const uint8_t BMP_HOME_EXTRAS[] PROGMEM;
extern const uint8_t BMP_HOME_SETTINGS[] PROGMEM;

extern const uint8_t BMP_STATUS_ALARMLOST[] PROGMEM;
extern const uint8_t BMP_STATUS_BTENABLED[] PROGMEM;
extern const uint8_t BMP_STATUS_BTBROKEN[] PROGMEM;

extern const int BMP_BOOT_SIZE[2];
extern const int BMP_HOME_CLOCK_SIZE[2];
extern const int BMP_HOME_BLUETOOTH_SIZE[2];
extern const int BMP_HOME_EXTRAS_SIZE[2];
extern const int BMP_HOME_SETTINGS_SIZE[2];
extern const int BMP_STATUS_ALARMLOST_SIZE[2];
extern const int BMP_STATUS_BTENABLED_SIZE[2];
extern const int BMP_STATUS_BTBROKEN_SIZE[2];

#endif
