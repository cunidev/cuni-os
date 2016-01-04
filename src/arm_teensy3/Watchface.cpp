#include "Watchface.h"

Watchface::Watchface(U8GLIB &u8g, CUNI_HW_RTC_NAME &rtc, CuniEEPROM &eeprom, int EEPROM_ADDR_LATEST_WATCHFACE) : _u8g(u8g), _rtc(rtc), _eeprom(eeprom) {
  _EEPROM_latestWatchFace = EEPROM_ADDR_LATEST_WATCHFACE;
  _watchFaceId = _eeprom.read(_EEPROM_latestWatchFace);

  _watchFaceCount = 5; // remember to edit this if necessary!
}
void Watchface::setChangePosDelay(unsigned long changePosDelay) {
  _changePosDelay = changePosDelay;
}

void Watchface::_updateEEPROM() {
  _eeprom.update(_EEPROM_latestWatchFace, _watchFaceId);
}

void Watchface::drawWatchFace() {
  switch(_watchFaceId) {
    case 0:
    _watchfaceD1();
    break;
    case 1:
    _watchfaceD2();
    break;
    case 2:
    _watchfaceD3();
    break;
    case 3:
    _watchfaceD4();
    break;
    case 4:
    _watchfaceA1();
    break;
  }
}
void Watchface::previousWatchFace() {
  if(_watchFaceId > 0)
    _watchFaceId--;
  else
    _watchFaceId = _watchFaceCount - 1;
    
  _updateEEPROM();
}
void Watchface::nextWatchFace() {
  if(_watchFaceId < _watchFaceCount - 1)
    _watchFaceId++;
  else
    _watchFaceId = 0;

  _updateEEPROM();
}

void Watchface::_watchfaceD1() {
  if(_timer == 0) {
    _timer = millis();
  } else if(millis() - _timer >= _changePosDelay) {
    _posX = random(0,75);
    _posY = random(15,40);
    _timer = 0;
  }
  _u8g.setFont(u8g_font_helvB08);
  _u8g.drawStr(_posX,_posY,_rtc.getFullFormattedHour());
  _u8g.setFontPosTop();
  _u8g.setFont(u8g_font_helvR08);
  _u8g.drawStr(_posX,_posY+10,_rtc.getFullFormattedDate());

}
void Watchface::_watchfaceD2() {
  _u8g.setFont(u8g_font_freedoomr25n);
  _u8g.setFontPosTop();
  _u8g.drawStr(20,18,_rtc.getFormattedHour());
}
void Watchface::_watchfaceD3() {
  _u8g.setFont(u8g_font_osb29n);
  _u8g.setFontPosTop();
  int x = floor((126- _u8g.getStrWidth(_rtc.getFormattedHour()))/2);
  _u8g.drawStr(x,18,_rtc.getFormattedHour());
}
void Watchface::_watchfaceD4() {
  int x;
  _u8g.setFont(u8g_font_freedoomr10r);
  _u8g.setFontPosTop();
  x = floor((126-_u8g.getStrWidth( _rtc.getFullFormattedHour() ))/2);
  _u8g.drawStr(x, 20, _rtc.getFullFormattedHour());
  _u8g.setFont(u8g_font_helvB08);
  _u8g.setFontPosTop();
  x = floor(( 126- _u8g.getStrWidth ( _rtc.getFullFormattedDate() ))/2);
  _u8g.drawStr(x,  33, _rtc.getFullFormattedDate());
}
void Watchface::_watchfaceA1() {
  _u8g.setFont(u8g_font_04b_03);

  _dial();

  _drawDial(_rtc.getHour(), 12.0, 15);     // rotation, rotationsteps/rotation, radius
  _drawDial(_rtc.getMinute(), 60.0, 24);   // rotation, rotationsteps/rotation, radius
  _drawDial(_rtc.getSecond(), 60, 27);     // rotation, rotationsteps/rotation, radius
}




void Watchface::_drawDial(float rotation, float ratio, int radius) {
  angle =  (rotation-15) * 2.0 * 3.1415 / ratio; // 
  X2 = ScreenWidthC + radius * cos(angle);
  Y2 = ScreenHeightC + radius * sin(angle);
  _u8g.drawLine(ScreenWidthC, ScreenHeightC, X2, Y2);
}

void Watchface::_dial() { // draw the dial
  // u8g.drawCircle(ScreenWidthC, ScreenHeightC, 31);  // drwas circle around the dial
  _u8g.drawCircle(ScreenWidthC, ScreenHeightC, 1);

  _u8g.setFont(u8g_font_04b_03b);  // small size figures on dial
  _u8g.setFontPosTop();
  _u8g.drawStr(60, 5, "12");
  _u8g.drawStr(86, 29, "3");
  _u8g.drawStr(63, 52, "6");
  _u8g.drawStr(39, 29, "9");

  /*
   _u8g.setFont(u8g_font_unifont);  // medium size figures on dial
   _u8g.setFontPosTop();
   _u8g.drawStr(55, 4, "12");
   _u8g.drawStr(83, 25, "3");
   _u8g.drawStr(60, 45, "6");
   _u8g.drawStr(39, 25, "9");
   */

  for(int dash = 0; dash<12; dash++) { // draw the "5 minutes" dashes
    angle = dash / 12.0 * 2 * 3.1415;
    X2 = ScreenWidthC + 30 * cos(angle);
    Y2 = ScreenHeightC + 30 * sin(angle);
    X3 = ScreenWidthC + 28 * cos(angle);
    Y3 = ScreenHeightC + 28 * sin(angle);

    _u8g.drawLine(X2, Y2, X3, Y3);
  }

}
