#ifndef Watchface_h
#define Watchface_h

#include <Arduino.h>
#include "U8glib.h"
#include "Config.h"
#include CUNI_HW_RTC_PATH
#include CUNI_HW_EEPROM_PATH

class Watchface {
  public:
    Watchface(U8GLIB &u8g, CUNI_HW_RTC_NAME &rtc, CuniEEPROM &eeprom, int EEPROM_ADDR_LATEST_WATCHFACE);
    void setChangePosDelay(unsigned long changePosDelay);
    void drawWatchFace();
    void previousWatchFace();
    void nextWatchFace();
  private:
    U8GLIB &_u8g;
    CUNI_HW_RTC_NAME &_rtc;
    CuniEEPROM &_eeprom;
    int _EEPROM_latestWatchFace;
    int _watchFaceId;
    int _watchFaceCount = 0;
    unsigned long _timer = 0;
    unsigned long _changePosDelay = 10000; // can be changed
    int _posX = 42;
    int _posY = 26;
    
    void _updateEEPROM();
    
    void _watchfaceD1();
    void _watchfaceD2();
    void _watchfaceD3();
    void _watchfaceD4();
    void _watchfaceA1();

    // analog clock stuff
    int X = 0;
    int Y = 0;
    int I = 0;
    int X2 = 0;
    int Y2 = 0;
    int X3 = 0;
    int Y3 = 0;
    float angle = 0;
    int ScreenWidth = 128;
    int ScreenWidthC = 64;
    int ScreenHeight = 64;
    int ScreenHeightC = 32;

    void _dial();
    void _drawDial(float rotation, float ratio, int radius);
};

#endif
