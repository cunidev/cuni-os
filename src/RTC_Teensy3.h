#ifndef RTC_h
#define RTC_h
#define CUNI_HW_RTC_NAME CuniRTC

#include <Arduino.h>
#include <Time.h>

class CuniRTC {
  public:
    CuniRTC();
    int getYear();
    int getMonth();
    int getDay();
    int getWeekDay();
    int getHour();
    int getMinute();
    int getSecond();
    char *getFormattedHour();
    char *getFullFormattedHour();
    char *getFormattedDate();
    char *getFullFormattedDate();
  private:
    char formattedHour[6];
    char fullFormattedHour[9];
    char formattedDate[6];
    char fullFormattedDate[9];
    void updateFormattedHour();
};


#endif
