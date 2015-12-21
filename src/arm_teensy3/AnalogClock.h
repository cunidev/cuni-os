#include <Arduino.h>
#include <Time.h>
#include "U8glib.h"

int X = 0;
int Y = 0;
int I = 0;
int X2 = 0;
int Y2 = 0;
int X3 = 0;
int Y3 = 0;
float angle = 0;

int ScreenWith = 128;
int ScreenWithC = 64;
int ScreenHeight = 64;
int ScreenHeightC = 32;

int seconds = 0;
int minutes = 0;
int hours = 0;

void getTime()        
{  
  seconds = second();           // get seconds,    
  minutes = minute();           // minutes   
  hours = hour();               // and hours. 
  hours = hours +1;                 // daylight savings  
} 

void drawDial(float rotation, float ratio, int radius, U8GLIB_SSD1306_128X64 u8g) {
  angle =  (rotation-15) * 2.0 * 3.1415 / ratio; // 
  X2 = ScreenWithC + radius * cos(angle);
  Y2 = ScreenHeightC + radius * sin(angle);
  u8g.drawLine(ScreenWithC, ScreenHeightC, X2, Y2);
}

void dial(U8GLIB_SSD1306_128X64 u8g) { // draw the dial
  // u8g.drawCircle(ScreenWithC, ScreenHeightC, 31);  // drwas circle around the dial
  u8g.drawCircle(ScreenWithC, ScreenHeightC, 1);

  u8g.setFont(u8g_font_04b_03b);  // small size figures on dial
  u8g.setFontPosTop();
  u8g.drawStr(60, 5, "12");
  u8g.drawStr(86, 29, "3");
  u8g.drawStr(63, 52, "6");
  u8g.drawStr(39, 29, "9");

  /*
  u8g.setFont(u8g_font_unifont);  // medium size figures on dial
   u8g.setFontPosTop();
   u8g.drawStr(55, 4, "12");
   u8g.drawStr(83, 25, "3");
   u8g.drawStr(60, 45, "6");
   u8g.drawStr(39, 25, "9");
   */

  for(int dash = 0; dash<12; dash++) { // draw the "5 minutes" dashes
    angle = dash / 12.0 * 2 * 3.1415;
    X2 = ScreenWithC + 30 * cos(angle);
    Y2 = ScreenHeightC + 30 * sin(angle);
    X3 = ScreenWithC + 28 * cos(angle);
    Y3 = ScreenHeightC + 28 * sin(angle);

    u8g.drawLine(X2, Y2, X3, Y3);
  }

}

void drawAnalogClock(U8GLIB_SSD1306_128X64 u8g) {
  // graphic commands to redraw the complete screen should be placed here 
  u8g.setFont(u8g_font_04b_03);

  u8g.setPrintPos(0,64);

  /*if bitRead(seconds,0) {
    u8g.print("Your text!");    // any short text will do :-)
  } 
  else {
    u8g.print("Time for");      // same here ;-p
  }*/

  dial(u8g);
  getTime();

  drawDial(hours, 12.0, 15, u8g);     // rotation, rotationsteps/rotation, radius
  drawDial(minutes, 60.0, 24, u8g);   // rotation, rotationsteps/rotation, radius
  drawDial(seconds, 60, 27, u8g);     // rotation, rotationsteps/rotation, radius
}
