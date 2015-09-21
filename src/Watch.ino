#include <Snooze.h>
#include <Time.h>
#include <EEPROM.h>
#include "U8glib.h"
#include "Watchface.h"
#include "CuniUI.h"
#include "Bluetooth.h"
#include "Buttons.h"
#include "LED.h"
#include "StopWatch.h"

// PINS
// 0,1 bluetooth?
// 2,3,8,9 buttons
// 4,5,6,7 SPI
// 21 buzzer
// 12 empty
// 11 vibration
// A6 LED
// A0-A5 empty

const int BTN_BACK = 2;
const int BTN_SELECT = 3;
const int BTN_UP = 11;
const int BTN_DOWN = 8;
const int BUZZER = 21;
const int LED_PIN = A6;
const int LM35 = A0;
const int EEPROM_ALARM_HOUR = 0;
const int EEPROM_ALARM_MINUTE = 1;
const int EEPROM_ALARM_ENABLED = 2;
const int EEPROM_BT_ENABLED = 3;
const int SW_MENU_DELAY = 200;
const int BTN_RADIUS = 1;

boolean bluetooth_available = false;
boolean bluetoothOn = false;
boolean lostAlarm = false;
boolean ignoreAlarm = false;
boolean alarmEnabled = false;

char formattedHour[5];
char fullFormattedHour[9];
char fullFormattedDate[11];
int intCounter = 0;
int statusBarCounter = 0;
int alarmHour = 0;
int alarmMinute = 0;

int swHour = 0;
int swMinute = 0;
int swSecond = 0;
int swMillis = 0;

boolean timerSet = false;
long timerSeconds = 0;

// Chess variables
uint8_t uiKeyCodeFirst = CHESS_KEY_NONE;
uint8_t uiKeyCodeSecond = CHESS_KEY_NONE;
uint8_t uiKeyCode = CHESS_KEY_NONE;

// These bitmaps could be easily optimized

const uint8_t boot_bitmap[] PROGMEM = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0xe0, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07,
   0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x07, 0x00, 0xe0, 0x00, 0x00, 0x00, 0xf8, 0x70, 0x38,
   0xdc, 0x03, 0x77, 0xf0, 0xe0, 0xf8, 0xc0, 0x3f, 0xf0, 0xe1, 0x1e, 0x00,
   0x00, 0xfe, 0x71, 0x38, 0xfc, 0x07, 0xe7, 0xf0, 0x70, 0xfe, 0xc3, 0x3f,
   0xfc, 0xe3, 0x3f, 0x00, 0x00, 0xff, 0x73, 0x38, 0x3c, 0x0f, 0xe7, 0xf0,
   0x70, 0x87, 0xc7, 0x3f, 0xfe, 0xe7, 0x7f, 0x00, 0x00, 0x8f, 0x70, 0x38,
   0x1c, 0x0e, 0xe7, 0xf8, 0x71, 0x00, 0x07, 0x07, 0x1e, 0xe1, 0x79, 0x00,
   0x80, 0x03, 0x70, 0x38, 0x1c, 0x0e, 0xc7, 0xf9, 0x39, 0x00, 0x07, 0x07,
   0x07, 0xe0, 0x70, 0x00, 0x80, 0x03, 0x70, 0x38, 0x1c, 0x0e, 0xc7, 0x99,
   0x39, 0xf8, 0x07, 0x07, 0x07, 0xe0, 0x70, 0x00, 0x80, 0x03, 0x70, 0x38,
   0x1c, 0x0e, 0xc7, 0x99, 0x3b, 0xfc, 0x07, 0x07, 0x07, 0xe0, 0x70, 0x00,
   0x80, 0x03, 0x70, 0x38, 0x1c, 0x0e, 0x87, 0x9f, 0x1b, 0x1e, 0x07, 0x07,
   0x07, 0xe0, 0x70, 0x00, 0x00, 0x8f, 0xf0, 0x3c, 0x1c, 0x0e, 0x87, 0x0f,
   0x1f, 0x0e, 0x07, 0x07, 0x1e, 0xe1, 0x70, 0x00, 0x00, 0xff, 0xf1, 0x3f,
   0x1c, 0x0e, 0x87, 0x0f, 0x1f, 0x8e, 0x07, 0x3f, 0xfe, 0xe3, 0x70, 0x00,
   0x00, 0xfe, 0xe3, 0x3f, 0x1c, 0x0e, 0x07, 0x0f, 0x0f, 0xfe, 0x07, 0x3f,
   0xfc, 0xe7, 0x70, 0x00, 0x00, 0xf8, 0xc0, 0x3b, 0x1c, 0x0e, 0x07, 0x0f,
   0x0e, 0x78, 0x07, 0x3e, 0xf0, 0xe1, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0xe0, 0x03, 0x00, 0x00,
   0xaa, 0x40, 0x30, 0x86, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22,
   0x80, 0x00, 0x00, 0x00, 0x28, 0x00, 0x48, 0xc9, 0x08, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x22, 0x80, 0xfc, 0x1d, 0x97, 0xaa, 0x4e, 0x40, 0x89,
   0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x80, 0x04, 0xa5, 0x94,
   0xaa, 0x52, 0x20, 0x89, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12,
   0x80, 0xc4, 0xa5, 0x94, 0xaa, 0x52, 0x20, 0x89, 0x20, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x22, 0x80, 0x24, 0xa5, 0x94, 0xaa, 0x52, 0x10, 0x89,
   0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x81, 0xe4, 0x25, 0xe7,
   0xaa, 0x52, 0x78, 0x87, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00 };
   
const uint8_t clock_bitmap[] PROGMEM = {
   0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x07, 0x00, 0x00, 0xf8,
   0xff, 0x1f, 0x00, 0x00, 0xfe, 0xff, 0x7f, 0x00, 0x00, 0xff, 0x81, 0xff,
   0x00, 0x80, 0x3f, 0x00, 0xfc, 0x01, 0xc0, 0x0f, 0x00, 0xf0, 0x03, 0xe0,
   0x03, 0x00, 0xc0, 0x07, 0xf0, 0x01, 0x00, 0x80, 0x0f, 0xf8, 0x00, 0x18,
   0x00, 0x1f, 0x78, 0x00, 0x18, 0x00, 0x1e, 0x7c, 0x00, 0x18, 0x00, 0x3e,
   0x3c, 0x00, 0x18, 0x00, 0x3c, 0x3e, 0x00, 0x18, 0x00, 0x3c, 0x1e, 0x00,
   0x18, 0x00, 0x78, 0x1e, 0x00, 0x18, 0x00, 0x78, 0x1e, 0x00, 0x18, 0x00,
   0x78, 0x0f, 0x00, 0x18, 0x00, 0x70, 0x0f, 0x00, 0x18, 0x00, 0xf0, 0x0f,
   0x00, 0x18, 0x00, 0xf0, 0x0f, 0x00, 0x38, 0x00, 0xf0, 0x0f, 0x00, 0x70,
   0x00, 0xf0, 0x0e, 0x00, 0xe0, 0x00, 0x70, 0x1f, 0x00, 0xc0, 0x01, 0x78,
   0x1e, 0x00, 0x80, 0x03, 0x78, 0x1e, 0x00, 0x00, 0x07, 0x78, 0x1e, 0x00,
   0x00, 0x06, 0x7c, 0x3c, 0x00, 0x00, 0x00, 0x3c, 0x7c, 0x00, 0x00, 0x00,
   0x3e, 0x78, 0x00, 0x00, 0x00, 0x1e, 0xf8, 0x00, 0x00, 0x00, 0x1f, 0xf0,
   0x01, 0x00, 0x80, 0x0f, 0xe0, 0x03, 0x00, 0xc0, 0x07, 0xc0, 0x0f, 0x00,
   0xf0, 0x03, 0x80, 0x1f, 0x00, 0xf8, 0x03, 0x00, 0xff, 0x00, 0xff, 0x00,
   0x00, 0xfe, 0xff, 0x7f, 0x00, 0x00, 0xf8, 0xff, 0x1f, 0x00, 0x00, 0xe0,
   0xff, 0x07, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x00
};

const uint8_t bluetooth_bitmap[] PROGMEM = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x03, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00,
   0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00,
   0x00, 0x7f, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x0e, 0xff,
   0x01, 0x00, 0x00, 0x1e, 0xef, 0x03, 0x00, 0x00, 0x3e, 0xcf, 0x07, 0x00,
   0x00, 0xfc, 0xcf, 0x0f, 0x00, 0x00, 0xf8, 0x0f, 0x1f, 0x00, 0x00, 0xf0,
   0x8f, 0x0f, 0x00, 0x00, 0xf0, 0xcf, 0x0f, 0x00, 0x00, 0xc0, 0xef, 0x03,
   0x00, 0x00, 0xc0, 0xff, 0x03, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00,
   0x00, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x00, 0xff,
   0x00, 0x00, 0x00, 0x80, 0xff, 0x01, 0x00, 0x00, 0xc0, 0xef, 0x03, 0x00,
   0x00, 0xe0, 0xcf, 0x07, 0x00, 0x00, 0xf0, 0xcf, 0x0f, 0x00, 0x00, 0xf8,
   0x0f, 0x1f, 0x00, 0x00, 0xfc, 0x8f, 0x0f, 0x00, 0x00, 0x3e, 0xcf, 0x0f,
   0x00, 0x00, 0x1e, 0xef, 0x03, 0x00, 0x00, 0x0e, 0xff, 0x03, 0x00, 0x00,
   0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x7f,
   0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00,
   0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
};

const uint8_t extras_bitmap[] PROGMEM = {
   0xfe, 0xff, 0xff, 0xff, 0xff, 0x02, 0x00, 0x00, 0x00, 0x80, 0x02, 0x00,
   0x00, 0x00, 0x80, 0x02, 0x00, 0x00, 0x00, 0x80, 0xc2, 0x1f, 0xfe, 0xf0,
   0x87, 0xe2, 0x3f, 0xff, 0xf9, 0x8f, 0xe2, 0x3f, 0xff, 0xf9, 0x8f, 0xe2,
   0x3f, 0xff, 0xf9, 0x8f, 0xe2, 0x3f, 0xff, 0xf9, 0x8f, 0xe2, 0x3f, 0xff,
   0xf9, 0x8f, 0xe2, 0x3f, 0xff, 0xf9, 0x8f, 0xe2, 0x3f, 0xff, 0xf9, 0x8f,
   0xc2, 0x1f, 0xfe, 0xf0, 0x87, 0x02, 0x00, 0x00, 0x00, 0x80, 0x02, 0x00,
   0x00, 0x00, 0x80, 0xc2, 0x1f, 0xfe, 0xf0, 0x87, 0xe2, 0x3f, 0xff, 0xf9,
   0x8f, 0xe2, 0x3f, 0xff, 0xf9, 0x8f, 0xe2, 0x3f, 0xff, 0xf9, 0x8f, 0xe2,
   0x3f, 0xff, 0xf9, 0x8f, 0xe2, 0x3f, 0xff, 0xf9, 0x8f, 0xe2, 0x3f, 0xff,
   0xf9, 0x8f, 0xe2, 0x3f, 0xff, 0xf9, 0x8f, 0xc2, 0x1f, 0xfe, 0xf0, 0x87,
   0x02, 0x00, 0x00, 0x00, 0x80, 0x02, 0x00, 0x00, 0x00, 0x80, 0xc2, 0x1f,
   0xfe, 0xf0, 0x87, 0xe2, 0x3f, 0xff, 0xf9, 0x8f, 0xe2, 0x3f, 0xff, 0xf9,
   0x8f, 0xe2, 0x3f, 0xff, 0xf9, 0x8f, 0xe2, 0x3f, 0xff, 0xf9, 0x8f, 0xe2,
   0x3f, 0xff, 0xf9, 0x8f, 0xe2, 0x3f, 0xff, 0xf9, 0x8f, 0xe2, 0x3f, 0xff,
   0xf9, 0x8f, 0xc2, 0x1f, 0xfe, 0xf0, 0x87, 0x02, 0x00, 0x00, 0x00, 0x80,
   0x02, 0x00, 0x00, 0x00, 0x80, 0x02, 0x00, 0x00, 0x00, 0x80, 0xfe, 0xff,
   0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 };
   
const uint8_t settings_bitmap[] PROGMEM = {
   0x00, 0x80, 0xff, 0x01, 0x00, 0x00, 0xf0, 0xff, 0x0f, 0x00, 0x00, 0xfc,
   0xff, 0x3f, 0x00, 0x00, 0xfe, 0xff, 0x7f, 0x00, 0x80, 0xff, 0xff, 0xff,
   0x01, 0xc0, 0xff, 0xff, 0xff, 0x03, 0xe0, 0xff, 0xff, 0xff, 0x07, 0xf0,
   0xff, 0xff, 0xff, 0x0f, 0xf0, 0xff, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff,
   0xff, 0x1f, 0xfc, 0xff, 0xff, 0xff, 0x3f, 0xfc, 0xe7, 0xe7, 0xe7, 0x3f,
   0xfe, 0xe7, 0xe7, 0xe7, 0x7f, 0xfe, 0xe7, 0xe7, 0xe7, 0x7f, 0xfe, 0xe7,
   0xe7, 0xe7, 0x7f, 0xff, 0x81, 0xe7, 0xe7, 0xff, 0xff, 0x81, 0xe7, 0xe7,
   0xff, 0xff, 0x81, 0xe7, 0xe7, 0xff, 0xff, 0x81, 0xe7, 0x81, 0xff, 0xff,
   0xe7, 0xe7, 0x81, 0xff, 0xff, 0xe7, 0xe7, 0x81, 0xff, 0xff, 0xe7, 0x81,
   0x81, 0xff, 0xff, 0xe7, 0x81, 0xe7, 0xff, 0xff, 0xe7, 0x81, 0xe7, 0xff,
   0xff, 0xe7, 0x81, 0xe7, 0xff, 0xfe, 0xe7, 0xe7, 0xe7, 0x7f, 0xfe, 0xe7,
   0xe7, 0xe7, 0x7f, 0xfe, 0xe7, 0xe7, 0xe7, 0x7f, 0xfc, 0xe7, 0xe7, 0xe7,
   0x3f, 0xfc, 0xff, 0xff, 0xff, 0x3f, 0xf8, 0xff, 0xff, 0xff, 0x1f, 0xf0,
   0xff, 0xff, 0xff, 0x0f, 0xf0, 0xff, 0xff, 0xff, 0x0f, 0xe0, 0xff, 0xff,
   0xff, 0x07, 0xc0, 0xff, 0xff, 0xff, 0x03, 0x80, 0xff, 0xff, 0xff, 0x01,
   0x00, 0xfe, 0xff, 0x7f, 0x00, 0x00, 0xfc, 0xff, 0x3f, 0x00, 0x00, 0xf0,
   0xff, 0x0f, 0x00, 0x00, 0x80, 0xff, 0x01, 0x00 
};

const uint8_t alarm_lost_bitmap[] PROGMEM = {
   0xfe, 0x00, 0x83, 0x01, 0x11, 0x01, 0x11, 0x01, 0x31, 0x01, 0x01, 0x01,
   0x01, 0x01, 0x83, 0x01, 0xfe, 0x00 };
   
const uint8_t bluetooth_enabled_bitmap[] PROGMEM = {
   0x03, 0x00, 0x05, 0x00, 0x03, 0x00, 0xc5, 0x00, 0xc3, 0x00, 0xd8, 0x00,
   0xd8, 0x00, 0xdb, 0x00, 0xdb, 0x00 };
   
const uint8_t bluetooth_broken_bitmap[] PROGMEM = {
   0x83, 0x01, 0xc7, 0x01, 0xee, 0x00, 0x7c, 0x00, 0x38, 0x00, 0x7c, 0x00,
   0xee, 0x00, 0xc7, 0x01, 0x83, 0x01 };

U8GLIB_SH1106_128X64 u8g(4, 5, 6, 7);
u8g_t vdisplay;
StopWatch sw;    // MILLIS (default)
StopWatch timerSW(StopWatch::SECONDS);
Bluetooth bt(9600);
LED notificationLED(LED_PIN);
ModKeypad keypad(BTN_BACK, BTN_SELECT, BTN_UP, BTN_DOWN);
SnoozeBlock snooze;


void setup() {
  pinMode(BUZZER,OUTPUT);
  u8g.setColorIndex(1);
  boot();

}

void loop() {
  homeScreen();
}
  
void boot() {
  int progress = 0;
  while(true) {
    int x = 31 + (31 * progress);
    u8g.firstPage();
    do {
      u8g.drawXBMP(0,0,126,64,boot_bitmap);
      u8g.drawBox(0,58,x,6);
    } while(u8g.nextPage());

    if(progress == 3) break;
    progress++;

    switch(progress) {
      case 0:
      analogWrite(LED_PIN,5);
      // init various things
      u8g_SetVirtualScreenDimension(&vdisplay, 128, 64);
      Serial.begin(9600);
      break;

      case 1:
      analogWrite(LED_PIN,15);
      // copy EEPROM to RAM
      alarmHour = EEPROM.read(EEPROM_ALARM_HOUR);
      alarmMinute = EEPROM.read(EEPROM_ALARM_MINUTE);
      bluetoothOn = (EEPROM.read(EEPROM_BT_ENABLED) == 1);
      updateAlarmEnabled();
      break;
      

      case 2:
      analogWrite(LED_PIN,50); 
      // init Bluetooth
      if(!bt.isReady()) { 
        cuni_ui_alert("No Bluetooth found","Timeout on Serial2", true, "Proceed");
        bluetooth_available = false; // NO hotplug support, needs reboot if not inserted
      } else {
        bluetooth_available = true;
      }
      break;

      case 3:
      break;
      
      default:
      break;
    }
  }
  analogWrite(LED_PIN,0); 
  delay(2 * SW_MENU_DELAY);
}
void homeScreen() {
  boolean home = true;
  int homeCursor = 0;
  while(home) {
    isAlarm();  
    int oldHomeCursor = homeCursor;
    int btn = keypad.getPressedButton();
    if(btn == BTN_DOWN) {
      if(homeCursor == 3)
        homeCursor = 0;
      else
        homeCursor++;
    } else if(btn == BTN_UP) {
      if(homeCursor == 0)
        homeCursor = 3;
      else
        homeCursor--;
    } else if(btn == BTN_SELECT) {
      delay(SW_MENU_DELAY);
      switch(homeCursor) {
        case 0:
        watch();
        break;
        case 1:
        break;
        case 2:
        chess();
        break;
        case 3:
        settingsScreen();
        break;
        default:
        break;
      }
    } else if(btn == BTN_BACK){
      delay(SW_MENU_DELAY);
      clock();
    }
    u8g.firstPage();
    do {  
      drawStatusBar();
      drawLauncher(homeCursor);
    } while( u8g.nextPage());
    if(homeCursor != oldHomeCursor) delay(SW_MENU_DELAY);
  }
}
void drawStatusBar() {
  u8g.setFont(u8g_font_helvB08);
  u8g.setFontPosTop();
  updateFormattedHour();
  u8g.drawStr(50,-1,formattedHour);
  u8g.drawBox(112,0,12,6);
  u8g.drawBox(124,1,2,4);
  if(alarmEnabled) {
    u8g.drawXBMP(0,0,9,9, alarm_lost_bitmap);
  }
  // TODO: draw BT broken, BT enabled, stopwatch, timer icons etc.

}
void drawLauncher(int choice) {
  u8g.setFontPosTop();
  u8g.setFont(u8g_font_helvR08);
  if(choice == 0) {
    u8g.setFont(u8g_font_helvB08);
    u8g.drawStr(64,22,"Watch");
    u8g.setFont(u8g_font_helvR08);

    u8g.drawXBMP( 11, 18, 40, 40, clock_bitmap);
  } else {
    u8g.drawStr(64,22,"Watch");
  }
  if(choice==1) {
    u8g.setFont(u8g_font_helvB08);
    u8g.drawStr(64,34,"Bluetooth");
    u8g.setFont(u8g_font_helvR08);

    u8g.drawXBMP( 11, 18, 40, 40, bluetooth_bitmap);
  } else {
    u8g.drawStr(64,34,"Bluetooth");
  }
  if(choice==2) {
    u8g.setFont(u8g_font_helvB08);
    u8g.drawStr(64,46,"Extras");
    u8g.setFont(u8g_font_helvR08);
    u8g.drawXBMP( 11, 18, 40, 40, extras_bitmap);
  } else {
    u8g.drawStr(64,46,"Extras");
  }
  if(choice==3) {
    u8g.setFont(u8g_font_helvB08);
    u8g.drawStr(64,58,"Settings");
    u8g.setFont(u8g_font_helvR08);

    u8g.drawXBMP( 11, 18, 40, 40, settings_bitmap);
  } else {
    u8g.drawStr(64,58,"Settings");
  }
 
}
void settingsScreen() {
  char* bluetoothStatus;
  int menuCursor = 0;
  boolean settings = true;
  while(settings) {
    isAlarm();  
    int oldMenuCursor = menuCursor;
    int btn = keypad.getPressedButton();
    if(btn != 0) {
      clicker();
      if(btn == BTN_DOWN) {
        if(menuCursor == 3)
          menuCursor = 0;
        else
          menuCursor++;
      } else if(btn == BTN_UP) {
        if(menuCursor == 0)
          menuCursor = 3;
        else
          menuCursor--;
      } else if(btn == BTN_SELECT) {
        delay(SW_MENU_DELAY);
        switch(menuCursor) {
          case 0:
          toggleBluetoothStatus();
          break;
          case 1:
          serialConsole();
          break;
          case 2:
          break;
          case 3:
          softwareUpdate();
          break;
          default:
          break;
        }
      } else if(btn == BTN_BACK){
        delay(SW_MENU_DELAY);
        settings = false;
      } else {
        
      }
    }
    if(bluetoothOn) {
      bluetoothStatus = "Bluetooth ON";
    } else {
      bluetoothStatus = "Bluetooth OFF";
    }
    u8g.firstPage();
    do {
      drawStatusBar();
      
      u8g.setFontPosTop();
      u8g.setFont(u8g_font_helvR08);
      if(menuCursor == 0) {
        u8g.setFont(u8g_font_helvB08);
        u8g.drawStr(10,22,bluetoothStatus);
        u8g.setFont(u8g_font_helvR08);
      } else {
        u8g.drawStr(10,22,bluetoothStatus);
      }
      if(menuCursor==1) {
        u8g.setFont(u8g_font_helvB08);
        u8g.drawStr(10,34,"Serial console");
        u8g.setFont(u8g_font_helvR08);
      } else {
        u8g.drawStr(10,34,"Serial console");
      }
      if(menuCursor==2) {
        u8g.setFont(u8g_font_helvB08);
        u8g.drawStr(10,46,"System info");
        u8g.setFont(u8g_font_helvR08);
      } else {
        u8g.drawStr(10,46,"System info");
      }
      if(menuCursor==3) {
        u8g.setFont(u8g_font_helvB08);
        u8g.drawStr(10,58,"Factory reset");
        u8g.setFont(u8g_font_helvR08);
      } else {
        u8g.drawStr(10,58,"Factory reset");
      }
    } while( u8g.nextPage());
    if(menuCursor != oldMenuCursor) delay(SW_MENU_DELAY);
  }
}
void serialConsole() {
  boolean serial = true;
  Serial.println("READY");
  while(serial) {
    isAlarm();  
    u8g.firstPage();
    do {  
      drawStatusBar();
      u8g.setFontPosTop();
      u8g.setFont(u8g_font_helvB08);
      u8g.drawStr(5,12,"Serial console");
      u8g.setFont(u8g_font_helvR08);
      u8g.setFontPosTop();
      u8g.drawStr(5,24,"Speed: 9600 baud");
      u8g.drawStr(5,36,"Waiting for commands...");
      if(keypad.getPressedButton() == BTN_BACK) {
        clicker();
        delay(SW_MENU_DELAY);
        serial = false;
      }
    } while( u8g.nextPage());
    //if(Serial.available() > 0) {
    //}
  }
}
void toggleBluetoothStatus() {
  // TODO: handle in EEPROM!
  if(bluetooth_available) {
    if(bluetoothOn) {
      if(cuni_ui_confirm("Disable Bluetooth?","OK","Cancel")) bluetoothOn = false;
    } else {
      if(cuni_ui_confirm("Enable Bluetooth?","OK","Cancel")) bluetoothOn = true;
    }
    EEPROM.write(EEPROM_BT_ENABLED,bluetoothOn);
  } else {
    cuni_ui_alert("No Bluetooth found","Please insert BT",true,"OK");
  }
}
void clock() {
  boolean clock = true;
  while(clock) {
    isAlarm();
    updateFormattedHour();  
    u8g.firstPage();
    do {  
        drawWatchFace(formattedHour, fullFormattedHour, fullFormattedDate, u8g);
    } while( u8g.nextPage());
    int btn = keypad.getPressedButton();
    if(btn == BTN_SELECT) {
      clicker();
      delay(SW_MENU_DELAY);
      clock = false;
    } else if(btn == BTN_DOWN) {
      switchWatchFacePrevious();
      delay(SW_MENU_DELAY);
    } else if(btn == BTN_UP) {
      switchWatchFace();
      delay(SW_MENU_DELAY);
    } else if(btn == BTN_BACK) {
      u8g.firstPage();
      do {
        // clear screen
      } while(u8g.nextPage());
      sleepUntilButtonPressed(BTN_UP);
      Serial.println("RELEASED!");
    }
    delay(1);
  }
}
void watch() {
  boolean watch = true;
  int menuCursor = 0;
  while(watch) {
    isAlarm();  
    int oldMenuCursor = menuCursor;
    int btn = keypad.getPressedButton();
    if(btn != 0) {
      clicker();
      if(btn == BTN_DOWN) {
        if(menuCursor == 2)
          menuCursor = 0;
        else
          menuCursor++;
      } else if(btn == BTN_UP) {
        if(menuCursor == 0)
          menuCursor = 2;
        else
          menuCursor--;
      } else if(btn == BTN_SELECT) {
        switch(menuCursor) {
          case 0:
          delay(SW_MENU_DELAY);
          stopwatch();
          break;
          case 1:
          delay(SW_MENU_DELAY);
          timer();
          Serial.println("EXIT TIMER");
          break;
          case 2:
          delay(SW_MENU_DELAY);
          char* txt;
          if(alarmEnabled) {
            txt = "Disable";
          } else {
            txt = "Enable";
          }
          int result;
          result = cuni_ui_dialog("Alarm","Set time",txt,true);
          if(result == 1) {
            setAlarm();
          } else if(result == 2) {
            toggleAlarmEnabled();
          } else {
            // operation cancelled
          }
          break;
          default:
          break;
        }
      } else if(btn == BTN_BACK){
        delay(SW_MENU_DELAY);
        watch = false;
      } else {
        
      }
    }
    u8g.firstPage();
    do {
      drawStatusBar();
      u8g.drawXBMP( 11, 18, 40, 40, clock_bitmap);
      u8g.setFontPosTop();
      u8g.setFont(u8g_font_helvR08);
      
      if(menuCursor == 0) {
        u8g.setFont(u8g_font_helvB08);
        u8g.drawStr(64,30,"Stopwatch");
        u8g.setFont(u8g_font_helvR08);
      } else {
        u8g.drawStr(64,30,"Stopwatch");
      }
      if(menuCursor==1) {
        u8g.setFont(u8g_font_helvB08);
        u8g.drawStr(64,42, "Timer");
        u8g.setFont(u8g_font_helvR08);
      } else {
        u8g.drawStr(64,42, "Timer");
      }
      if(menuCursor==2) {
        u8g.setFont(u8g_font_helvB08);
        u8g.drawStr(64,54,"Alarm");
        u8g.setFont(u8g_font_helvR08);
      } else {
        u8g.drawStr(64,54,"Alarm");
      }
    } while( u8g.nextPage());
    if(menuCursor != oldMenuCursor) delay(SW_MENU_DELAY);
  }
}
void softwareUpdate() {
  if(cuni_ui_confirm("This will delete all data","Proceed","Cancel")) {
    // delete EEPROM data (with progress bar)
    cuni_ui_alert("","Reset successful",true,"Reboot");
    // reboot
  }
}
void setAlarm() {
  boolean alarm = true;
  char text[30];
  int alHour = alarmHour;
  int alMinute = alarmMinute;
  while(alarm) {
    boolean setAlarmHour = true;
    while(setAlarmHour) {
      char hourStr[3];
      u8g.firstPage();
      sprintf(hourStr,"%02d",alHour);
      do {
        u8g.setFont(u8g_font_fub49n);
        u8g.setFontPosTop();
        int x = floor( ( 128 - u8g.getStrWidth(hourStr) ) / 2 );
        u8g.drawStr(x,5,hourStr);
        
        u8g.setFont(u8g_font_helvR08);
        u8g.setFontPosTop();
        u8g.drawStr90(10,22,"hour");
      } while(u8g.nextPage());
      int btnID = keypad.getPressedButton();
      if(btnID != 0) {
        switch(btnID) {
          case BTN_UP:
          alHour++;
          break;
          case BTN_DOWN:
          alHour--;
          break;
          case BTN_BACK:
          // TODO: cancel operation
          break;
          case BTN_SELECT:
          delay(SW_MENU_DELAY);
          setAlarmHour = false;
          break;
          default:
          break;
        }
        delay(SW_MENU_DELAY/2);
      }
      if(alHour > 23) alHour = 0;
      if(alHour < 0) alHour = 23;
    }
    boolean setAlarmMinute = true;
    while(setAlarmMinute) {
      char minuteStr[3];
      u8g.firstPage();
      sprintf(minuteStr,"%02d",alMinute);
      do {
        u8g.setFont(u8g_font_fub49n);
        u8g.setFontPosTop();
        int x = floor( ( 128 - u8g.getStrWidth(minuteStr) ) / 2 );
        u8g.drawStr(x,5,minuteStr);

        u8g.setFont(u8g_font_helvR08);
        u8g.setFontPosTop();
        u8g.drawStr90(10,18,"minute");
      } while(u8g.nextPage());
      int btnID = keypad.getPressedButton();
      if(btnID != 0) {
        switch(btnID) {
          case BTN_UP:
          alMinute++;
          break;
          case BTN_DOWN:
          alMinute--;
          break;
          case BTN_BACK:
          // TODO: cancel operation
          break;
          case BTN_SELECT:
          delay(SW_MENU_DELAY/2);
          setAlarmMinute = false;
          break;
          default:
          break;
        }
        delay(SW_MENU_DELAY/2);
      }
      if(alMinute > 59) alMinute = 0;
      if(alMinute < 0) alMinute = 59;
    }
    sprintf(text,"Set alarm to %02d:%02d?", alHour,alMinute);
    if(cuni_ui_confirm(text,"OK","Cancel")) {
      setAlarmTime(alHour,alMinute);
      alarm = false;
    } else {
      alarm = false;
    }
  }
  
}
void alarmLoop() {
  int x = 7;
  int y = 0;
  while(!ignoreAlarm) {
    u8g.firstPage();
    do {
      drawStatusBar();
      u8g.setFont(u8g_font_helvB08);
      u8g.setFontPosTop();
      u8g.drawStr(47,25,"ALARM");
      u8g.setFont(u8g_font_helvR08);
      u8g.setFontPosTop();
      u8g.drawStr(35,40,"Press any key...");
    } while(u8g.nextPage());
    if(x == 7) {
      analogWrite(LED_PIN, 255); // TODO: handle these with LED.cpp
      buzzerTone(1000,50);
      delay(50);
      analogWrite(LED_PIN, 0);
      buzzerTone(1000,50);
      delay(50);
      analogWrite(LED_PIN, 255);
      buzzerTone(1000,50);
      delay(50);
      buzzerTone(1000,50);
      analogWrite(LED_PIN, 0);
      x = 0;
    }
    if(y == 350) {
      lostAlarm = true;
      ignoreAlarm = true;
      break;
    }
    if(keypad.getPressedButton() != 0) {
      delay(SW_MENU_DELAY);
      ignoreAlarm = true;
      break;
    }
    x++;
    y++;
  }
}
void stopwatch() {
  boolean stopwatch = true;
  boolean lapButtonPressed = false;
  
  char text[12];
  char lap1[15] = "#1 --:--:--:--";
  char lap2[15] = "#2 --:--:--:--";
  char lap3[15] = "#3 --:--:--:--";
  char lap4[15] = "#4 --:--:--:--";
  int lapCounter = 0;
  
  while(stopwatch) {
    u8g.firstPage();
    do {
      swMillis = sw.elapsed();
      swSecond = ((swMillis/1000) % 60);
      swMinute = ((swMillis / 60000) % 60);
      swHour = ((swMillis / 3600000) % 24);
      sprintf(text,"%02d:%02d:%02d:%02d",swHour,swMinute,swSecond,(swMillis / 10) % 100);
      
      int btn = keypad.getPressedButton();
      if(btn != 0) {
        if(btn == BTN_BACK) {
          delay(SW_MENU_DELAY);
          stopwatch = false;
        }
        if(btn == BTN_UP && !sw.isRunning()) {
          sw.start();
        }
        if(btn == BTN_DOWN && sw.isRunning()) {
          sw.stop();
          // TODO: maybe write data on EEPROM?
        }
        if(btn == BTN_SELECT && sw.isRunning()) {
          if(!lapButtonPressed) {
            switch(lapCounter) {
              case 0:
              sprintf(lap1,"#1 %s",text);
              break;
              case 1:
              sprintf(lap2,"#2 %s",text);
              break;
              case 2:
              sprintf(lap3,"#3 %s",text);
              break;
              case 3:
              sprintf(lap4,"#4 %s",text);
              break;
              default:
              // do nothing
              break;
            }
            lapCounter++;
            lapButtonPressed = true;
          }
        } else if(btn != BTN_SELECT && sw.isRunning()) {
          lapButtonPressed = false;
        }
        if(btn == BTN_SELECT && !sw.isRunning()) {
          sw.reset();
          sprintf(lap1,"#1 --:--:--:--");
          sprintf(lap2,"#2 --:--:--:--");
          sprintf(lap3,"#3 --:--:--:--");
          sprintf(lap4,"#4 --:--:--:--");
          lapCounter = 0;
        }
      } else {
        lapButtonPressed = false;      
      }
      u8g.drawFrame(0,5,84,18);
      u8g.setFont(u8g_font_freedoomr10r);
      u8g.setFontPosTop();
      u8g.drawStr(4,8,text);

      u8g.setFont(u8g_font_profont10);
      u8g.drawStr(6,33,lap1);
      u8g.drawStr(6,42,lap2);
      u8g.drawStr(6,51,lap3);
      u8g.drawStr(6,60,lap4);
      
      u8g.drawLine(88,0,88,64);
      u8g.setFont(u8g_font_orgv01);
      u8g.setFontPosTop();
      u8g.drawStr(95,8,"START");
      u8g.setFontPosTop();
      if(sw.isRunning()) {
        sprintf(text,"LAP");
      } else {
        sprintf(text,"RESET");
      }
      u8g.drawStr(95,30,text);
      u8g.setFontPosTop();
      u8g.drawStr(95,51,"STOP");
    } while(u8g.nextPage());
  }
}
void timer() {
  boolean timerOn = true;
  int remainingSeconds = 0;
  char text[9];
  int timHour = 0;
  int timMinute = 0;
  int timSecond = 0;
  if(!timerSet) {
      setTimer();
      timerSW.start();
      timerSet = true;
  }
  while(timerSet && timerOn) {
          Serial.print("DEBUG: cycle ");
          Serial.println(random(0,10000));
    u8g.firstPage();
    do {
      int btnId = keypad.getPressedButton();
      if(btnId != 0) {
        if(btnId == BTN_BACK) {
          delay(2*SW_MENU_DELAY);
          timerOn = false;
          break;
        } else if(btnId == BTN_SELECT && !timerSW.isRunning()) {
          timerSW.start();
          delay(SW_MENU_DELAY);
        } else if(btnId == BTN_SELECT && timerSW.isRunning()) {
          timerSW.stop();
          delay(SW_MENU_DELAY);
        }
        else if(btnId == BTN_DOWN && !timerSW.isRunning()) { // URGENT DEBUG: freeze when reset or close while not running!
          timerSW.stop();
          timerSW.reset();
          timerSeconds = 0;
          timSecond = 0;
          delay(SW_MENU_DELAY);
          timerOn = false;
          timerSet = false;
          break;
        }
      }
      remainingSeconds = timerSeconds - timerSW.elapsed();
      timSecond = (remainingSeconds % 60);
      timMinute = ((remainingSeconds / 60) % 60);
      timHour = ((remainingSeconds / 3600) % 24);
      sprintf(text,"%02d:%02d:%02d",timHour,timMinute,timSecond);

      u8g.setFont(u8g_font_profont22);
      u8g.setFontPosTop();
      u8g.drawStr(((126 - u8g.getStrWidth(text))/2),19,text);

      u8g.setFont(u8g_font_chikita);
      u8g.setFontPosTop();
      if(timerSW.isRunning()) {
        sprintf(text,"Select: Pause");
      } else {
        sprintf(text,"Select: Start | Down: Reset");
      }
      u8g.drawStr(((126 - u8g.getStrWidth(text))/2),52,text);
      
    } while(u8g.nextPage());
    isAlarm(); // triggers both isAlarm and isTimer
  } Serial.println("EXIT CYCLE");
}
void setTimer() {
  int timerHour = 0;
  int timerMinute = 0;
    if(timerSW.isRunning()) {
      timerSW.stop();
      timerSW.reset();
    }
    boolean setTimerHour = true;
    while(setTimerHour) {
      char hourStr[3];
      u8g.firstPage();
      sprintf(hourStr,"%02d",timerHour);
      do {
        u8g.setFont(u8g_font_fub49n);
        u8g.setFontPosTop();
        int x = floor( ( 128 - u8g.getStrWidth(hourStr) ) / 2 );
        u8g.drawStr(x,5,hourStr);
        
        u8g.setFont(u8g_font_helvR08);
        u8g.setFontPosTop();
        u8g.drawStr90(10,22,"hour");
      } while(u8g.nextPage());
      int btnID = keypad.getPressedButton();
      if(btnID != 0) {
        switch(btnID) {
          case BTN_UP:
          timerHour++;
          break;
          case BTN_DOWN:
          timerHour--;
          break;
          case BTN_BACK:
          // TODO: cancel operation
          break;
          case BTN_SELECT:
          delay(SW_MENU_DELAY);
          setTimerHour = false;
          break;
          default:
          break;
        }
        delay(SW_MENU_DELAY/2);
      }
      if(timerHour > 23) timerHour = 0;
      if(timerHour < 0) timerHour = 23;
    }
    boolean setTimerMinute = true;
    if(timerHour == 0) timerMinute = 1;
    while(setTimerMinute) {
      char minuteStr[3];
      u8g.firstPage();
      sprintf(minuteStr,"%02d",timerMinute);
      do {
        u8g.setFont(u8g_font_fub49n);
        u8g.setFontPosTop();
        int x = floor( ( 128 - u8g.getStrWidth(minuteStr) ) / 2 );
        u8g.drawStr(x,5,minuteStr);

        u8g.setFont(u8g_font_helvR08);
        u8g.setFontPosTop();
        u8g.drawStr90(10,18,"minute");
      } while(u8g.nextPage());
      int btnID = keypad.getPressedButton();
      if(btnID != 0) {
        switch(btnID) {
          case BTN_UP:
          timerMinute++;
          break;
          case BTN_DOWN:
          timerMinute--;
          break;
          case BTN_BACK:
          // TODO: cancel operation
          break;
          case BTN_SELECT:
          delay(SW_MENU_DELAY/2);
          setTimerMinute = false;
          break;
          default:
          break;
        }
        delay(SW_MENU_DELAY/2);
      }
      if(timerMinute > 59 && timerHour == 0) timerMinute = 1;
      if(timerMinute > 59 && timerHour != 0) timerMinute = 0;
      if(timerMinute < 0) timerMinute = 59;
      if(timerMinute < 1 && timerHour == 0) timerMinute = 59;
    }
    timerSeconds = (timerHour * 3600) + (timerMinute * 60);
}

void buzzerTone(int toneValue, int time) {
  tone(BUZZER,toneValue);
  delay(time);
  noTone(BUZZER);
  digitalWrite(BUZZER,LOW);
}
void clicker() {
  buzzerTone(70,15);
}



void isAlarm() {
  isTimer();
  if(lostAlarm) {
    lostAlarm = false;
  }
  if(alarmEnabled && hour() == alarmHour && minute() == alarmMinute)
    alarmLoop();
}
void isTimer() {
  int x = 0;
  if(timerSW.isRunning()) {
    if(timerSW.elapsed() >= timerSeconds) {
      timerSW.stop();
      timerSW.reset();
      timerSeconds = 0;
      timerSet = false;
      while(true) {
        u8g.firstPage();
        do {
          drawStatusBar();
          u8g.setFont(u8g_font_helvB08);
          u8g.setFontPosTop();
          u8g.drawStr(35,25,"Time is up!");
          u8g.setFont(u8g_font_helvR08);
          u8g.setFontPosTop();
          Serial.println((126 - u8g.getStrWidth("Press any key..."))/2);
          u8g.drawStr(27,40,"Press any key...");
        } while(u8g.nextPage());
        if((x % 7) == 0) {
          analogWrite(LED_PIN, 255); //TODO: handle this with LED library
          buzzerTone(300,50);
          delay(80);
          analogWrite(LED_PIN, 0);
          buzzerTone(300,50);
        }
        if(x == 100) {
          break;
        }
        if(keypad.getPressedButton() != 0) {
          delay(SW_MENU_DELAY);
          break;
        }
        x++;
      }
    }
  }
}
void updateFormattedHour() {
  intCounter++;
  sprintf(formattedHour,"%02d:%02d", hour(), minute());
  sprintf(fullFormattedHour,"%02d:%02d:%02d",hour(), minute(), second());
  sprintf(fullFormattedDate,"%02d/%02d/%02d", day(), month(), year() % 100);
  intCounter = 0;
}

void updateAlarmEnabled() {
  if(EEPROM.read(EEPROM_ALARM_ENABLED) == 1) {
    alarmEnabled = true;
  } else {
    alarmEnabled = false;
  }  
}
void toggleAlarmEnabled() {
  ignoreAlarm = false;
  if(EEPROM.read(EEPROM_ALARM_ENABLED) == 1) {
    alarmEnabled = false;
    EEPROM.write(EEPROM_ALARM_ENABLED,0);
  } else {
    EEPROM.write(EEPROM_ALARM_ENABLED,1);
    alarmEnabled = true;
  }  
}
void setAlarmTime(int alHour, int alMinute) {
  EEPROM.write(EEPROM_ALARM_HOUR,alHour);
  EEPROM.write(EEPROM_ALARM_MINUTE,alMinute);
  alarmHour = EEPROM.read(EEPROM_ALARM_HOUR);
  alarmMinute = EEPROM.read(EEPROM_ALARM_MINUTE);
}

/* CuniUI Functions */

boolean cuni_ui_confirm(char text[], char btnYes[], char btnNo[]) {
  isAlarm();
  boolean waitingConfirm = true;
  boolean result;
  int cursor = 0;
  while(waitingConfirm) {
    u8g.firstPage();
    do {
      u8g.setFont(u8g_font_helvR08);
      u8g.setFontPosTop();
      int x = floor( ( 128 - u8g.getStrWidth(text) ) / 2 );
      u8g.drawStr(x,20,text);

      u8g.setFont(u8g_font_helvR08);
      x = floor(2 + ((57 - u8g.getStrWidth(btnYes))/2 ));
      u8g.setFontPosTop();
      if(cursor == 0) {
        u8g.drawRBox(2, 48, 60, 16, BTN_RADIUS); // X pixel range for font: 2->59
        
        u8g.setDefaultBackgroundColor();
        u8g.setFont(u8g_font_helvB08);
        u8g.setFontPosTop();
        u8g.drawStr(x, 51, btnYes);
        u8g.setFont(u8g_font_helvR08);
        u8g.setDefaultForegroundColor();
      } else {
        u8g.drawRFrame(2, 48, 60, 16, BTN_RADIUS); // X pixel range for font: 2->59
        
        u8g.drawStr(x, 51, btnYes);
      }
      
      x = floor(66 + ((57 - u8g.getStrWidth(btnNo))/2 ));
      u8g.setFontPosTop();
      if(cursor == 1) {
        u8g.drawRBox(66, 48, 60, 16, BTN_RADIUS);  // X pixel range for font: 66->123
        
        u8g.setDefaultBackgroundColor();
        u8g.setFont(u8g_font_helvB08);
        u8g.setFontPosTop();
        u8g.drawStr(x, 51, btnNo);
        u8g.setFont(u8g_font_helvR08);
        u8g.setDefaultForegroundColor();
      } else {
        u8g.drawRFrame(66, 48, 60, 16, BTN_RADIUS);  // X pixel range for font: 66->123
        u8g.drawStr(x, 51, btnNo);
      }
      u8g.setDefaultForegroundColor();
    } while(u8g.nextPage());
    int btnID = keypad.getPressedButton();
    if(btnID != 0) {
      if(btnID == BTN_BACK) {
        delay(SW_MENU_DELAY);
        result = false;
        waitingConfirm = false;
      } else if(btnID == BTN_DOWN || btnID == BTN_UP) {
        delay(SW_MENU_DELAY/2);
        if(cursor == 0) cursor = 1;
        else cursor = 0;
      } else if(btnID == BTN_SELECT) {
        delay(SW_MENU_DELAY);
        if(cursor == 0) result = true;
        else result = false;
        waitingConfirm = false;
      }
    }
  }
  return result;
}
int cuni_ui_dialog(char text[], char btnYes[], char btnNo[], boolean allowCancel) {
  isAlarm();
  int cursor = 0;
  while(true) {
    u8g.firstPage();
    do {
      u8g.setFont(u8g_font_helvR08);
      u8g.setFontPosTop();
      int x = floor( ( 128 - u8g.getStrWidth(text) ) / 2 );
      u8g.drawStr(x,20,text);

      u8g.setFont(u8g_font_helvR08);
      x = floor(2 + ((57 - u8g.getStrWidth(btnYes))/2 ));
      u8g.setFontPosTop();
      if(cursor == 0) {
        u8g.drawRBox(2, 48, 60, 16, BTN_RADIUS); // X pixel range for font: 2->59
        
        u8g.setDefaultBackgroundColor();
        u8g.setFont(u8g_font_helvB08);
        u8g.setFontPosTop();
        u8g.drawStr(x, 51, btnYes);
        u8g.setFont(u8g_font_helvR08);
        u8g.setDefaultForegroundColor();
      } else {
        u8g.drawRFrame(2, 48, 60, 16, BTN_RADIUS); // X pixel range for font: 2->59
        
        u8g.drawStr(x, 51, btnYes);
      }
      
      x = floor(66 + ((57 - u8g.getStrWidth(btnNo))/2 ));
      u8g.setFontPosTop();
      if(cursor == 1) {
        u8g.drawRBox(66, 48, 60, 16, BTN_RADIUS);  // X pixel range for font: 66->123
        
        u8g.setDefaultBackgroundColor();
        u8g.setFont(u8g_font_helvB08);
        u8g.setFontPosTop();
        u8g.drawStr(x, 51, btnNo);
        u8g.setFont(u8g_font_helvR08);
        u8g.setDefaultForegroundColor();
      } else {
        u8g.drawRFrame(66, 48, 60, 16, BTN_RADIUS);  // X pixel range for font: 66->123
        u8g.drawStr(x, 51, btnNo);
      }
      u8g.setDefaultForegroundColor();
    } while(u8g.nextPage());
    int btnID = keypad.getPressedButton();
    if(btnID != 0) {
      if(btnID == BTN_BACK && allowCancel) {
        delay(SW_MENU_DELAY);
        return 0;
      } else if(btnID == BTN_DOWN || btnID == BTN_UP) {
        delay(SW_MENU_DELAY/2);
        if(cursor == 0) cursor = 1;
        else cursor = 0;
      } else if(btnID == BTN_SELECT) {
        delay(SW_MENU_DELAY);
        cursor++;
        return cursor;
      }
    }
  }
  return 0;
}


void cuni_ui_alert(char title[], char text[], boolean showButton, char btnText[]) {
  isAlarm();
  while(true) {
    u8g.firstPage();
    do {
      //drawStatusBar();
      u8g.setFont(u8g_font_helvB08);
      u8g.setFontPosTop();
      u8g.drawStr(floor( ( 126 - u8g.getStrWidth(title) ) / 2 ),17,title);
      u8g.setFont(u8g_font_helvR08);
      u8g.setFontPosTop();
      u8g.drawStr(floor( ( 126 - u8g.getStrWidth(text) ) / 2 ),28,text);

      if(showButton) {
        u8g.drawRBox(18, 48, 90, 16, BTN_RADIUS);     
        u8g.setDefaultBackgroundColor();
        u8g.setFont(u8g_font_helvB08);
        u8g.setFontPosTop();
        u8g.drawStr(floor((126 - u8g.getStrWidth(btnText))/2 ), 51, btnText);
        u8g.setDefaultForegroundColor();
      }
    } while(u8g.nextPage());
    if(keypad.getPressedButton() == BTN_SELECT || (showButton == false && keypad.getPressedButton() != 0)) {
      delay(SW_MENU_DELAY);
      break;
    }
  }

}

/* CHESS */
void chess() {
  chess_Init(u8g.getU8g(), 0);
  boolean chLoop = true;
  while(chLoop) {
    chLoop = chessLoop();
  }
}
boolean chessLoop() {
  uint8_t keyCode = CHESS_KEY_NONE;
  
  u8g.firstPage();
  do {
    chess_Draw();
    uiStep();
    if ( uiKeyCode != CHESS_KEY_NONE )
      keyCode = uiKeyCode;
  } while( u8g.nextPage() );

  if(keyCode == CHESS_KEY_BACK) {
    delay(SW_MENU_DELAY);
    if(cuni_ui_confirm("Close without saving?","Quit game","Continue")) {
      delay(SW_MENU_DELAY);
      return false;
    }
  }

  //u8g_Delay(10);
  chess_Step(keyCode);
  uiStep();
  keyCode = uiKeyCode;
  return true;
}
void uiStep(void)
{
  uiKeyCodeSecond = uiKeyCodeFirst;
  int btn = keypad.getPressedButton();
  if ( btn == BTN_DOWN )
    uiKeyCodeFirst = CHESS_KEY_PREV;
  else if ( btn == BTN_UP )
    uiKeyCodeFirst = CHESS_KEY_NEXT;
  else if ( btn == BTN_SELECT )
    uiKeyCodeFirst = CHESS_KEY_SELECT;
  else if ( btn == BTN_BACK  )
    uiKeyCodeFirst = CHESS_KEY_BACK;
  else 
    uiKeyCodeFirst = CHESS_KEY_NONE;
  
  if ( uiKeyCodeSecond == uiKeyCodeFirst )
    uiKeyCode = uiKeyCodeFirst;
  else
    uiKeyCode = CHESS_KEY_NONE;
}


void sleepUntilButtonPressed(int buttonPin) {
  snooze.pinMode(buttonPin, INPUT_PULLUP, RISING);
  Snooze.deepSleep(snooze);
}

