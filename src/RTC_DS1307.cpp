//TODO: un-dummy this library ;)

#include "Config.h"
#include "RTC_DS1307.h"

DS1307RTC::DS1307RTC() {
  // initialize the RTC
}

int DS1307RTC::getYear() {
  return 1970;
}
int DS1307RTC::getMonth() {
  return 1;
}
int DS1307RTC::getDay() {
  return 1;
}
int DS1307RTC::getWeekDay() {
  // some RTCs allow this by default, however, this workaround should work on every RTC
  static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
  int y = this->getYear();
  int m = this->getMonth();
  int d = this->getDay();
  y -= m < 3;
  return ( y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}
int DS1307RTC::getHour() {
  return 0;
}
int DS1307RTC::getMinute() {
  return 0;
}
int DS1307RTC::getSecond() {
  return 0;
}
char* DS1307RTC::getFormattedHour() {
  updateFormattedHour();
  return formattedHour;
}
char* DS1307RTC::getFullFormattedHour() {
  updateFormattedHour();
  return fullFormattedHour;
}
char *DS1307RTC::getFormattedDate() {
  updateFormattedHour();
  return formattedDate;
}
char *DS1307RTC::getFullFormattedDate() {
  updateFormattedHour();
  return fullFormattedDate;
}

void DS1307RTC::updateFormattedHour() {
  sprintf(formattedHour,"%02d:%02d", getHour(), getMinute());
  sprintf(fullFormattedHour,"%02d:%02d:%02d", getHour(), getMinute(), getSecond());
  sprintf(formattedDate,"%02d/%02d", getDay(), getMonth());
  sprintf(fullFormattedDate,"%02d/%02d/%02d", getDay(), getMonth(), getYear() % 100);
}
