#ifndef Config_h
#define Config_h

#define CUNI_OS_VERSION_ID 0 // do not edit this!
#define CUNI_OS_VERSION_LABEL "0.1 alpha"

/* Here you can configure CuniOS for your hardware, by changing the flags below
 *  
 * 
 * Summary of platform IDs:
 * 0 custom (specify flags manually)
 * 1 Arduino Mega (and other ATMega2560 compatible boards) // NOT SUPPORTED!
 * 2 Teensy 3.0/3.1/3.2
 * 
 * Display ID:
 * 0 other (specify U8GLIB constructor)
 * 1 SSD1306 128x64 SPI OLED display
 * 2 SH1106 128x64 SPI OLED display
 * 
 * Bluetooth module ID
 * 0 custom driver (specify driver path with a flag)
 * 1 HC05 module
 * 2 (...) HM-10 module (CC2540, BLE 4.0)
 * 
 * RTC driver ID
 * 0 built-in RTC / other library (specify path)
 * 1 DS1307 I2C
 * 
 * EEPROM driver
 * 0 Platform default / Other (specify path)
 * 1 CuniEEPROM
 * 
 * Keypad drivers:
 * 0 default (hardware pull-up resistor if supported)/custom (specify path)
 * 1 universal Arduino driver (needs external pull-up resistor)
 */

// PLATFORM 1 does NOT WORK NOW!


#define CUNI_OS_PLATFORM_ID 2
#define CUNI_HW_DISPLAY_ID 1
//#define CUNI_HW_DISPLAY_CONSTRUCTOR U8GLIB_SSD1306_128X64 u8g(4, 5, 6, 7)
//#define CUNI_HW_DISPLAY_WIDTH 128
//#define CUNI_HW_DISPLAY_HEIGHT 64
#define CUNI_HW_BLUETOOTH_ID 1
//#define CUNI_HW_BLUETOOTH_PATH "my_bt_driver.h"
#define CUNI_HW_RTC_ID 0
//#define CUNI_HW_RTC_CUSTOM_PATH "my_rtc_driver_.h"
#define CUNI_HW_EEPROM_ID 0
//#define CUNI_HW_EEPROM_CUSTOM_PATH "my_eeprom.h"
#define CUNI_HW_KEYPAD_ID 0
//#define CUNI_HW_KEYPAD_CUSTOM_PATH "my_keypad.h"


// pins
#define CUNI_PIN_BTN_BACK 2;
#define CUNI_PIN_BTN_SELECT 3;
#define CUNI_PIN_BTN_UP 11;
#define CUNI_PIN_BTN_DOWN 8;
#define CUNI_PIN_BUZZER 21;
#define CUNI_PIN_LED_PIN A6;



// DO NOT EDIT BELOW!
#ifndef CUNI_OS_PLATFORM_ID
  #error CuniOS Error: "CUNI_OS_PLATFORM_ID" not specified
#elif (CUNI_OS_PLATFORM_ID == 0)
  #define CUNI_OS_PLATFORM_NAME "Platform Name"
  #define CUNI_HW_RTC_BUILTIN_SUPPORTED false
  //#define CUNI_HW_RTC_PATH "my_rtc.h"
  #define CUNI_HW_KEYPAD_HWPULLUP_SUPPORTED false
  #define CUNI_HW_KEYPAD_PATH "Keypad_Default.h"
  //#define CUNI_HW_KEYPAD_HWPULLUP_PATH "my_keypad_pullup.h"
  #define CUNI_HW_EEPROM_PATH "CuniEEPROM.h" // default
  #define CUNI_HW_GOVERNOR_PATH "my_governor.h"
  #define CUNI_HW_PLL_SUPPORTED false
  #define CUNI_HW_TIMER_INTERRUPT_PATH "custom_path.h"  
  
#elif (CUNI_OS_PLATFORM_ID == 1)
  #define CUNI_OS_PLATFORM_NAME "ATMEGA2560 (Arduino)"
  #define CUNI_HW_RTC_BUILTIN_SUPPORTED false
  #define CUNI_HW_KEYPAD_HWPULLUP_SUPPORTED false
  #define CUNI_HW_KEYPAD_PATH "Keypad_Default.h"
  #define CUNI_HW_EEPROM_PATH "CuniEEPROM.h"
  #define CUNI_HW_GOVERNOR_PATH "Governor_AVR_default.h"
  #define CUNI_HW_PLL_SUPPORTED true
  #define CUNI_HW_TIMER_INTERRUPT_PATH "TimerInterrupt_AVR.h"  
  
#elif (CUNI_OS_PLATFORM_ID == 2)
  #define CUNI_OS_PLATFORM_NAME "Teensy 3.1"
  #define CUNI_HW_RTC_BUILTIN_SUPPORTED true
  #define CUNI_HW_RTC_PATH "RTC_Teensy3.h"
  #define CUNI_HW_KEYPAD_HWPULLUP_SUPPORTED true
  #define CUNI_HW_KEYPAD_PATH "Keypad_Default.h"
  #define CUNI_HW_KEYPAD_HWPULLUP_PATH "Keypad_Teensy3_PullUp.h"
  #define CUNI_HW_EEPROM_PATH "CuniEEPROM.h"
  #define CUNI_HW_GOVERNOR_PATH "Governor_Teensy3.h"
  #define CUNI_HW_PLL_SUPPORTED false
  #define CUNI_HW_TIMER_INTERRUPT_PATH "TimerInterrupt_Teensy.h"  
  
#else
  #error CuniOS Error: Invalid platform ID specified
#endif

// validation of platform profile
#ifndef CUNI_OS_PLATFORM_NAME
  #error CuniOS Error: platform name not specified
#endif
#ifndef CUNI_HW_RTC_BUILTIN_SUPPORTED
  #error CuniOS Error: "CUNI_HW_RTC_BUILTIN_SUPPORTED" not specified
#elif CUNI_HW_RTC_BUILTIN_SUPPORTED == true
  #ifndef CUNI_HW_RTC_PATH
    #error CuniOS Error: no driver path for built-in RTC specified
  #endif
#endif
#ifndef CUNI_HW_KEYPAD_PATH
  #error CuniOS Error: no keypad driver path specified
#endif
#ifndef CUNI_HW_KEYPAD_HWPULLUP_SUPPORTED
  #error CuniOS Error: "CUNI_HW_KEYPAD_HWPULLUP_SUPPORTED" not specified
#elif CUNI_HW_KEYPAD_HWPULLUP_SUPPORTED == true
  #ifndef CUNI_HW_KEYPAD_HWPULLUP_PATH
    #error CuniOS Error: no driver path for keypad (internal pull-up resistor) specified
  #endif
#endif
#ifndef CUNI_HW_EEPROM_PATH
  #error CuniOS Error: no EEPROM driver path specified
#endif
#ifndef CUNI_HW_GOVERNOR_PATH
  #error CuniOS Error: no CPU governor driver path specified
#endif
#ifndef CUNI_HW_PLL_SUPPORTED
  #error CuniOS Error: "CUNI_HW_PLL_SUPPORTED" not specified
#endif
#ifndef CUNI_HW_TIMER_INTERRUPT_PATH
  #error CuniOS Error: "CUNI_HW_TIMER_INTERRUPT_PATH" not specified
#endif
// end of platform validation


#ifndef CUNI_HW_DISPLAY_ID
  #error CuniOS Error: "CUNI_HW_DISPLAY_ID" not specified
#elif CUNI_HW_DISPLAY_ID == 0
  #ifndef CUNI_HW_DISPLAY_CONSTRUCTOR
    #error CuniOS Error: "CUNI_HW_DISPLAY_CONSTRUCTOR" not specified
  #endif
  #ifndef CUNI_HW_DISPLAY_WIDTH
    #error CuniOS Error: "CUNI_HW_DISPLAY_WIDTH" not specified
  #endif
  #ifndef CUNI_HW_DISPLAY_HEIGHT
    #error CuniOS Error: "CUNI_HW_DISPLAY_HEIGHT" not specified
  #endif
#elif CUNI_HW_DISPLAY_ID == 1
  #define CUNI_HW_DISPLAY_CONSTRUCTOR U8GLIB_SSD1306_128X64 u8g(4, 5, 6, 7)
  #define CUNI_HW_DISPLAY_WIDTH 128
  #define CUNI_HW_DISPLAY_HEIGHT 64
#elif CUNI_HW_DISPLAY_ID == 2
  #define CUNI_HW_DISPLAY_CONSTRUCTOR U8GLIB_SH1106_128X64 u8g(4, 5, 6, 7)
  #define CUNI_HW_DISPLAY_WIDTH 128
  #define CUNI_HW_DISPLAY_HEIGHT 64
#else
  #error CuniOS Error: unknown display driver ID
#endif

#ifndef CUNI_HW_BLUETOOTH_ID
  #error CuniOS Error: "CUNI_HW_BLUETOOTH_ID" not specified
#elif CUNI_HW_BLUETOOTH_ID == 0
  #ifndef CUNI_HW_BLUETOOTH_PATH
    #error CuniOS Error: "CUNI_HW_BLUETOOTH_PATH" not specified
  #endif
#elif CUNI_HW_BLUETOOTH_ID == 1
  #define CUNI_HW_BLUETOOTH_PATH "Bluetooth_HC05.h"
#elif CUNI_HW_BLUETOOTH_ID == 2
  #define CUNI_HW_BLUETOOTH_PATH "Bluetooth_HM10.h"
#else
  #error CuniOS Error: unknown Bluetooth driver ID
#endif

#ifndef CUNI_HW_RTC_ID
  #error CuniOS Error: "CUNI_HW_RTC_ID" not specified
#elif CUNI_HW_RTC_ID == 0
  #ifndef CUNI_HW_RTC_CUSTOM_PATH
    #if CUNI_HW_RTC_BUILTIN_SUPPORTED != true      
      #error CuniOS Error: RTC custom driver path not specified
    #endif
  #else
    #ifndef CUNI_HW_RTC_PATH
      #undef CUNI_HW_RTC_PATH
    #endif
    #define CUNI_HW_RTC_PATH  CUNI_HW_RTC_CUSTOM_PATH
  #endif
#elif CUNI_HW_RTC_ID == 1
  #define CUNI_HW_RTC_PATH "RTC_DS1307.h"
#else
  #error CuniOS Error: unknown RTC driver ID
#endif

#ifndef CUNI_HW_EEPROM_ID
  #error CuniOS Error: "CUNI_HW_EEPROM_ID" not specified
#elif CUNI_HW_EEPROM_ID == 0
  #ifdef CUNI_HW_EEPROM_CUSTOM_PATH
    #undef CUNI_HW_EEPROM_PATH
    #define CUNI_HW_EEPROM_PATH CUNI_HW_EEPROM_CUSTOM_PATH
  #endif
#elif CUNI_HW_EEPROM_ID == 1
  #undef CUNI_HW_EEPROM_PATH
  #define CUNI_HW_EEPROM_PATH "CuniEEPROM.h"
#else
  #error CuniOS Error: unknown EEPROM driver ID
#endif

#ifndef CUNI_HW_KEYPAD_ID
  #error CuniOS Error: "CUNI_HW_KEYPAD_ID" not specified
#elif CUNI_HW_KEYPAD_ID == 0
  #ifndef CUNI_HW_CUSTOM_KEYPAD_PATH
    #if CUNI_HW_KEYPAD_HWPULLUP_SUPPORTED == true
      #undef CUNI_HW_KEYPAD_PATH
      #define CUNI_HW_KEYPAD_PATH CUNI_HW_KEYPAD_HWPULLUP_PATH
    #else
      #error CuniOS Error: "CUNI_HW_KEYPAD_ID" not specified
    #endif
  #else
    #undef CUNI_HW_KEYPAD_PATH
    #define CUNI_HW_KEYPAD_PATH CUNI_HW_CUSTOM_KEYPAD_PATH
  #endif
  
#elif CUNI_HW_KEYPAD_ID == 1
  #undef CUNI_HW_KEYPAD_PATH
  #define CUNI_HW_KEYPAD_PATH "Keypad_Default.h"
#else
  #error CuniOS Error: unknown keypad driver ID
#endif

#endif
