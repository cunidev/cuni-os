#ifndef RTC_h
#define RTC_h
#define CUNI_HW_RTC_NAME DS1307RTC

#include <Arduino.h>

class DS1307RTC {
  public:
    DS1307RTC();
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
