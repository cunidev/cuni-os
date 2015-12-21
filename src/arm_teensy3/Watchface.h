#include <Arduino.h>
#include <Time.h>
#include "U8glib.h"
#include "AnalogClock.h"

int watchFaceId = 0;
int x = 42;
int y = 26;
int z = 0;

void switchWatchFace() {
  if(watchFaceId < 4)
    watchFaceId++;
  else
    watchFaceId = 0;
}
void switchWatchFacePrevious() {
  if(watchFaceId > 0)
    watchFaceId--;
  else
    watchFaceId = 4;
}

void drawWatchFace(char* formattedHour, char* fullFormattedHour, char* fullFormattedDate, U8GLIB_SSD1306_128X64 u8g) {
  switch(watchFaceId) {
    case 0:
    z++;
    u8g.setFont(u8g_font_helvB08);
    u8g.drawStr(x,y,fullFormattedHour);
    u8g.setFontPosTop();
    u8g.setFont(u8g_font_helvR08);
    u8g.drawStr(x,y+10,fullFormattedDate);
    if(z == 800) {
      x = random(0,75);
      y = random(15,40);
      z = 0;
    }
    break;
    case 1:
    u8g.setFont(u8g_font_freedoomr25n);
    u8g.setFontPosTop();
    u8g.drawStr(20,18,formattedHour);
    break;
    case 2:
    drawAnalogClock(u8g);
    break;
    case 3:
    u8g.setFont(u8g_font_osb29n);
    u8g.setFontPosTop();
    u8g.drawStr(floor((126-u8g.getStrWidth(formattedHour))/2),18,formattedHour);
    break;
    case 4:
    u8g.setFont(u8g_font_freedoomr10r);
    u8g.setFontPosTop();
    u8g.drawStr(floor((126-u8g.getStrWidth(fullFormattedHour))/2),20,fullFormattedHour);
    u8g.setFont(u8g_font_helvB08);
    u8g.setFontPosTop();
    u8g.drawStr(floor((126-u8g.getStrWidth(fullFormattedDate))/2),33,fullFormattedDate);
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    default:
    break;
  }
}
