#include "Watchface.h"

Watchface::Watchface(U8GLIB &u8g, CuniRTC &rtc, CuniEEPROM &eeprom, int EEPROM_ADDR_LATEST_WATCHFACE) : _u8g(u8g), _rtc(rtc), _eeprom(eeprom) {
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
  // ANALOG CLOCK HERE!
}
