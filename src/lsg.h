// Little Sun Gazer
// PORTED FROM CODE BY dr-mod, HERE:
// https://github.com/dr-mod/little-sun-gazer
// (dude is based in Kyiv)
// на хуй Путін

#include <math.h>
#include <stdio.h>
#include <cstdlib>

//#define Font12 &aAntiCorona14pt7b;
#define RADEG       (180.0/M_PI)
#define SEC_IN_DAY   (60 * 60 * 24)
#define JULIAN_DAY_1970 (2451544.5 - 10957)

//TODO: Remove all date related code
extern uint8_t buf[];
extern char *week[];

const int SecondsPerMinute = 60;
const int SecondsPerHour = 3600;
const int SecondsPerDay = 86400;
const int daysOfMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
time_t rawtime;
time_t utc_time;
uint16_t time_zone;
String currentHour;
String currentMinute;

bool IsLeapYear(short year) {
    if (year % 4 != 0) return false;
    if (year % 100 != 0) return true;
    return (year % 400) == 0;
}

time_t mkgmtime(short year, short month, short day, short hour, short minute, short second) {
    time_t secs = 0;
    for (short y = 1970; y < year; ++y)
        secs += (IsLeapYear(y) ? 366 : 365) * SecondsPerDay;
    for (short m = 1; m < month; ++m) {
        secs += daysOfMonth[m - 1] * SecondsPerDay;
        if (m == 2 && IsLeapYear(year)) secs += SecondsPerDay;
    }
    secs += (day - 1) * SecondsPerDay;
    secs += hour * SecondsPerHour;
    secs += minute * SecondsPerMinute;
    secs += second;
    return secs;
  }

static const char *DATETIME_MONTHS[12] = {
        "January",
        "February",
        "March",
        "April",
        "May",
        "June",
        "July",
        "August",
        "September",
        "October",
        "November",
        "December"
};

static const char *DATETIME_DAYS[7] = {
        "Sunday",
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday",
        "Saturday",
};

struct solarTime final {
  int sYear;
  int sMonth;
  int sDay;
  int sHour;
  int sMinute;
  int sSecond;
} sTime;

void WatchyGSR::drawLittleSunGazerWatchFace() {
  
  sTime.sYear = WatchTime.Local.Year;
  sTime.sMonth = WatchTime.Local.Month;
  sTime.sDay = WatchTime.Local.Day;
  sTime.sHour = WatchTime.Local.Hour;
  sTime.sMinute = WatchTime.Local.Minute;
  sTime.sSecond = WatchTime.Local.Second;
  
  rawtime = mkgmtime(sTime.sYear, sTime.sMonth, sTime.sDay, sTime.sHour, sTime.sMinute, sTime.sSecond);
  utc_time = WatchTime.UTC.Hour;
  time_zone = -7;
  
  
  // Kyiv coordinates
  //SolarThings solar(30.524937, 50.464631);
  
  // Portland OR (longitude, latitude)
  SolarThings solar(-122.679565, 45.512794); 
  
  
  display.fillScreen(Options.LightMode ? GxEPD_BLACK : GxEPD_WHITE);
  
  uint16_t dColor = Options.LightMode ? GxEPD_WHITE : GxEPD_BLACK;
  
  sun_paths(solar, rawtime, time_zone);
  moon_earth(solar, rawtime, time_zone);

  currentHour = String((WatchTime.Local.Hour < 10) ? "0" + String(WatchTime.Local.Hour) : String(WatchTime.Local.Hour));
  currentMinute = String((WatchTime.Local.Minute < 10) ? "0" + String(WatchTime.Local.Minute) : String(WatchTime.Local.Minute));

  // Draw Time
  String time_str = String(currentHour + ":" + currentMinute);
  display.setFont(&aAntiCorona14pt7b);
  display.setTextColor(dColor);
  display.setCursor(75, 185);
  display.print(time_str);
}
