#include "Config.h"
#if CUNI_OS_PLATFORM_ID == 2
#include "RTC_Teensy3.h"

CuniRTC::CuniRTC() {
  // initialize the RTC, if needed (for example, open i2c communication bus)
}

int CuniRTC::getYear() {
  return year();
}
int CuniRTC::getMonth() {
  return month();
}
int CuniRTC::getDay() {
  return day();
}
int CuniRTC::getWeekDay() {
  // some RTCs allow this by default, however, this workaround should work on every RTC
  static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
  int y = this->getYear();
  int m = this->getMonth();
  int d = this->getDay();
  y -= m < 3;
  return ( y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}
int CuniRTC::getHour() {
  return hour();
}
int CuniRTC::getMinute() {
  return minute();
}
int CuniRTC::getSecond() {
  return second();
}
char* CuniRTC::getFormattedHour() {
  updateFormattedHour();
  return formattedHour;
}
char* CuniRTC::getFullFormattedHour() {
  updateFormattedHour();
  return fullFormattedHour;
}
char *CuniRTC::getFormattedDate() {
  updateFormattedHour();
  return formattedDate;
}
char *CuniRTC::getFullFormattedDate() {
  updateFormattedHour();
  return fullFormattedDate;
}

void CuniRTC::updateFormattedHour() {
  sprintf(formattedHour,"%02d:%02d", getHour(), getMinute());
  sprintf(fullFormattedHour,"%02d:%02d:%02d", getHour(), getMinute(), getSecond());
  sprintf(formattedDate,"%02d/%02d", getDay(), getMonth());
  sprintf(fullFormattedDate,"%02d/%02d/%02d", getDay(), getMonth(), getYear() % 100);
}
#endif
