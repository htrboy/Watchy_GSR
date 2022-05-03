#ifndef WATCHY_GSR_H
#define WATCHY_GSR_H

#define Font12 FreeSerifBoldItalic10pt7b
#define RADEG       (180.0/M_PI)
#define SEC_IN_DAY   (60 * 60 * 24)
#define JULIAN_DAY_1970 (2451544.5 - 10957)

#include <Watchy.h>
#include "Defines_GSR.h"
#include "Web-HTML.h"
#include <Arduino.h>
#include <FunctionalInterrupt.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ArduinoOTA.h>
#include <Update.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <SmallRTC.h>
#include <SmallNTP.h>
#include <Olson2POSIX.h>
#include "GxEPD2_BW.h"
#include <mbedtls/base64.h>
#include <Wire.h>
#include <StableBMA.h>

#include "Icons_GSR.h"
#include "ArduinoNvs.h"
#include "resource.h"

extern RTC_DATA_ATTR struct WeatherUse final {
  uint8_t state;          // State = 0=Off
  uint8_t wait;           // waits up to 2 minutes 
  uint8_t pause;          // wait for wifi to connect
  uint8_t count;             // how many times have we tried
 bool updateWx;          
  bool check;             //last check failed or succeeded
  //bool done;
} getWeather;

extern RTC_DATA_ATTR struct TimeData final {
    time_t UTC_RAW;           // Copy of the UTC on init.
    tmElements_t UTC;         // Copy of UTC only split up for usage.
    tmElements_t Local;       // Copy of the Local time on init.
    String TimeZone;          // The location timezone, not the actual POSIX.
    unsigned long EPSMS;      // Milliseconds (rounded to the enxt minute) when the clock was updated via NTP.
    bool NewMinute;           // Set to True when New Minute happens.
    time_t TravelTest;        // For Travel Testing.
    int32_t Drifting;         // The amount to add to UTC_RAW after reading from the RTC.
    int64_t WatchyRTC;        // Counts Microseconds from boot.
    bool DeadRTC;             // Set when Drift fails to get a good count less than 30 seconds.
    uint8_t NextAlarm;        // Next index that will need to wake the Watchy from sleep to fire.
    bool BedTime;             // If the hour is within the Bed Time settings.
} WatchTime;

extern RTC_DATA_ATTR struct Optional final {
    bool TwentyFour;                  // If the face shows 24 hour or Am/Pm.
    bool LightMode;                    // Light/Dark mode.
    bool Feedback;                    // Haptic Feedback on buttons.
    bool Border;                      // True to set the border to black/white.
    bool Lefty;                       // Swaps the buttons to the other side.
    bool Swapped;                     // Menu and Back buttons swap ends (vertically).
    bool Orientated;                  // Set to false to not bother which way the buttons are.
    uint8_t Turbo;                    // 0-10 seconds.
    uint8_t MasterRepeats;            // Done for ease, will be in the Alarms menu.
    int Drift;                        // Seconds drift in RTC.
    bool UsingDrift;                  // Use the above number to add to the RTC by dividing it by 1000.
    uint8_t SleepStyle;               // 0==Disabled, 1==Always, 2==Sleeping
    uint8_t SleepMode;                // Turns screen off (black, won't show any screen unless a button is pressed)
    uint8_t SleepStart;               // Hour when you go to bed.
    uint8_t SleepEnd;                 // Hour when you wake up.
    uint8_t Performance;              // Performance style, "Turbo", "Normal", "Battery Saving" 
    bool NeedsSaving;                 // NVS code to tell it things have been updated, so save to NVS.
    bool BedTimeOrientation;          // Make Buttons only work while Watch is in normal orientation.
    uint8_t WatchFaceStyle;           // Using the Style values from Defines_GSR.
} Options;

extern RTC_DATA_ATTR weatherData latestWeather;
extern RTC_DATA_ATTR int8_t temperature;
extern RTC_DATA_ATTR String tempCondition;
extern RTC_DATA_ATTR char city;
extern RTC_DATA_ATTR String cityName;
extern RTC_DATA_ATTR int cityNameID;
extern RTC_DATA_ATTR uint8_t weatherMode;
extern RTC_DATA_ATTR int16_t weatherConditionCode;
extern RTC_DATA_ATTR bool updateWeather;
extern RTC_DATA_ATTR bool isNight;
extern RTC_DATA_ATTR bool showCached; // last weather data
extern RTC_DATA_ATTR bool weatherSyncFailed;

class SolarThings {
private:
    double longitude;
    double latitude;

    void from_sun(double M_moon, double e_moon, double a_moon, double N_moon, double w_moon, double i_moon, double (&moon_xy)[2]) {
        double E0 = M_moon + (180 / M_PI) * e_moon * sin(M_moon / RADEG) * (1 + e_moon * cos(M_moon / RADEG));
        E0 = normalize(E0);
        double E1 = E0 - (E0 - (180 / M_PI) * e_moon * sin(E0 / RADEG) - M_moon) / (1 - e_moon * cos(E0 / RADEG));
        E1 = normalize(E1) / RADEG;
        double x = a_moon * (cos(E1) - e_moon);
        double y = a_moon * (sqrt(1 - e_moon * e_moon)) * sin(E1);

        double r = sqrt(x * x + y * y);
        double v = atan2(y, x);
        v = normalize(v * RADEG);

        moon_xy[0] = r * (cos(N_moon / RADEG) * cos((v + w_moon) / RADEG) - sin(N_moon / RADEG) * sin(
                (v + w_moon) / RADEG) * cos((i_moon) / RADEG));
        moon_xy[1] = r * (sin(N_moon / RADEG) * cos((v + w_moon) / RADEG) + cos(N_moon / RADEG) * sin(
                (v + w_moon) / RADEG) * cos(i_moon / RADEG));
    }

    static double normalize(double degrees) {
        return fmod(degrees, 360);
    }

    double refraction_correction(double zenith) {
        double exoatm_elevation = 90 - zenith;
        if (exoatm_elevation > 85) {
            return 0;
        }
        double refractionCorrection;
        double te = tan(exoatm_elevation / RADEG);
        if (exoatm_elevation > 5.0) {
            refractionCorrection =
                    58.1 / te - 0.07 / (te * te * te) + 0.000086 / (te * te * te * te * te);
        } else {
            if (exoatm_elevation > -0.575) {
                refractionCorrection = get_refraction_corrected_elevation(exoatm_elevation);
            } else {
                refractionCorrection = -20.774 / te;
            }
        }
        return refractionCorrection / 3600;
    }

    double get_refraction_corrected_elevation(double e) {
        return 1735.0 + e * (-518.2 + e * (103.4 + e * (-12.79 + e * 0.711)));
    }

    double solar_noon_time(double lon, double eqTime) {
        return 720.0 + (lon * 4.0) - eqTime;
    }

    double sunrise_time(double lon, double eqTime, double ha, short timezone) {
        return 720.0 + ((lon - ha) * 4.0) - eqTime + 60 * timezone;
    }

    double sunset_time(double lon, double eqTime, double ha, short timezone) {
        return 720.0 + ((lon + ha) * 4.0) - eqTime + 60 * timezone;
    }

    double julian(double time) {
        return (time / SEC_IN_DAY) + JULIAN_DAY_1970;
    }

    double sun_declination(double t) {
        double e = obliquity_corrected(t) / RADEG;
        double b = sun_apparent_longitude(t) / RADEG;
        double sint = sin(e) * sin(b);
        double theta = asin(sint);
        return theta * RADEG;
    }

    double eccentricity_earth_orbit(double t) {
        return 0.016708634 - t * (0.000042037 + 0.0000001267 * t);
    }

    double equation_of_time(double t) {
        double eps = obliquity_corrected(t) / RADEG;
        double l0 = sun_geometric_mean_longitude(t) / RADEG;
        double m = sun_geometric_mean_anomaly(t) / RADEG;
        double e = eccentricity_earth_orbit(t);
        double y = tan(eps / 2);
        y *= y;

        double sin2l0 = sin(2 * l0);
        double cos2l0 = cos(2 * l0);
        double sin4l0 = sin(4 * l0);
        double sin1m = sin(m);
        double sin2m = sin(2 * m);

        double etime =
                y * sin2l0
                - 2 * e * sin1m
                + 4 * e * y * sin1m * cos2l0
                - 0.5 * y * y * sin4l0
                - 1.25 * e * e * sin2m;

        return (etime * RADEG) * 4.0;
    }

    double sun_geometric_mean_anomaly(double t) {
        return 357.52911 + t * (35999.05029 - 0.0001537 * t);
    }

    double sun_equation_of_center(double t) {
        double m = sun_geometric_mean_anomaly(t) / RADEG;
        return sin(1 * m) * (1.914602 - t * (0.004817 + 0.000014 * t))
               + sin(2 * m) * (0.019993 - t * (0.000101))
               + sin(3 * m) * (0.000289);
    }

    double sun_geometric_mean_longitude(double t) {
        double l0 = 280.46646 + t * (36000.76983 + 0.0003032 * t);
        l0 = l0 - 360 * floor(l0 / 360);
        return l0;
    }

    double sun_true_longitude(double t) {
        return sun_geometric_mean_longitude(t) + sun_equation_of_center(t);
    }

    double sun_apparent_longitude(double t) {
        double omega = (125.04 - 1934.136 * t) / RADEG;
        return sun_true_longitude(t) - 0.00569 - 0.00478 * sin(omega);
    }

    double mean_obliquity_of_ecliptic(double t) {
        double seconds = 21.448 - t * (46.8150 + t * (0.00059 - t * (0.001813)));
        return 23.0 + (26.0 + (seconds / 60.0)) / 60.0;
    }

    double obliquity_corrected(double t) {
        double e0 = mean_obliquity_of_ecliptic(t);
        double omega = (125.04 - 1934.136 * t) / RADEG;
        return e0 + 0.00256 * cos(omega);
    }

public:
    SolarThings(double longitude, double latitude) {      
        this->longitude = -longitude;
        this->latitude = (latitude > 89.8) ? 89.8 : latitude;
        this->latitude = (latitude < -89.8) ? -89.8 : this->latitude;
    }

    double elevation(long time) {
        double julian_day = julian(time);
        double mtime = (julian_day - 2451545) / 36525;

        double solar_declin = sun_declination(mtime);
        double eqTime = equation_of_time(mtime);

        double trueSolarTime = ((julian_day + 0.5) - floor(julian_day + 0.5)) * 1440;
        trueSolarTime += (eqTime - 4.0 * longitude);
        trueSolarTime -= 1440 * floor(trueSolarTime / 1440);


        double latitude_rad = this->latitude / RADEG;

        double csz =
                sin(latitude_rad) * sin(solar_declin / RADEG)
                + cos(latitude_rad)
                  * cos(solar_declin / RADEG)
                  * cos((trueSolarTime / 4 - 180) / RADEG);
        if (csz > +1) csz = +1;
        if (csz < -1) csz = -1;

        double zenith = acos(csz);

        double refractionCorrection = this->refraction_correction(zenith * RADEG);
        double solarZen = (zenith * RADEG) - refractionCorrection;

        double elevation = 90 - solarZen;

        return elevation;
    }

    long sunrise(long time, short timezone) {
        double julian_day = julian(time);
        double mtime = (julian_day - 2451545) / 36525;
        double solar_declin = sun_declination(mtime);
        double eq_time = equation_of_time(mtime);

        double latitude_rad = this->latitude / RADEG;

        double ha = acos((cos(90.883 / RADEG)/(cos(latitude_rad) * cos(solar_declin / RADEG)))
                - tan(latitude_rad) * tan(solar_declin / RADEG)) * RADEG;
        long sunrise_timev = round(sunrise_time(longitude, eq_time, ha, timezone) * (60))
                             + (time / SEC_IN_DAY) * SEC_IN_DAY;

        return sunrise_timev;
    }

    long sunset(long time, short timezone) {
        double julian_day = julian(time);
        double mtime = (julian_day - 2451545) / 36525;
        double solar_declin = sun_declination(mtime);
        double eq_time = equation_of_time(mtime);

        double latitude_rad = this->latitude / RADEG;

        double ha = acos((cos(90.883 / RADEG)/(cos(latitude_rad) * cos(solar_declin / RADEG)))
                         - tan(latitude_rad) * tan(solar_declin / RADEG)) * RADEG;
        long sunrise_timev = round(sunset_time(longitude, eq_time, ha, timezone) * (60))
                             + (time / SEC_IN_DAY) * SEC_IN_DAY;

        return sunrise_timev;
    }

    void celestial_bodies(long time, double (&result)[4]) {
        double julian_day = julian(time);
        double d = (julian_day - 2451545);

        double w = 282.9404 + 4.70935E-5 * d;
        double e = (0.016709 - (1.151E-9 * d));
        double M = normalize(356.047 + 0.9856002585 * d);

        double E = M + (180 / M_PI) * e * sin(M / RADEG) * (1 + e * cos(M / RADEG));

        double xv = cos(E) - e;
        double yv = sin(E) * sqrt(1 - e * e);

        double r = sqrt(xv * xv + yv * yv);

        double v = atan2(yv, xv) * RADEG;
        double lonsun = normalize(v + w) / RADEG;
        double xs = r * cos(lonsun); 
        double ys = r * sin(lonsun);

        double earthX = -1 * xs; 
        double earthY = 1 * ys; // changed to pos 1 for 90° CW rotation

        double N_moon = normalize(125.1228 - 0.0529538083 * d);
        double i_moon = 5.1454;
        double w_moon = 318.0634 + 0.1643573223 * d;
        double a_moon = 60.2666 * 4.26354E-5;
        double e_moon = 0.054900;
        double M_moon = normalize(115.3654 + 13.0649929509 * d);

        double moon_xy[2];
        from_sun(M_moon, e_moon, a_moon, N_moon, w_moon, i_moon, moon_xy);

        result[0] = earthX;
        result[1] = earthY;
        result[2] = moon_xy[0];
        result[3] = moon_xy[1];
    }
};

extern RTC_DATA_ATTR SolarThings solar(double longitude, double latitude);

class WatchyGSR{
    public:
        static SmallRTC SRTC;
        static SmallNTP SNTP;
        static GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display;
        static constexpr const char* Build = "1.4.3";
        enum DesOps {dSTATIC, dLEFT, dRIGHT, dCENTER};
    public:
        WatchyGSR();
        virtual void init(String datetime = "") final;
        void showWatchFace();
        void drawWatchFace(); //override this method for different watch faces
        void drawTime();
        void drawDay();
        void drawDate();
        void drawYear();
        virtual void handleButtonPress(uint8_t Pressed) final;
        virtual void deepSleep() final;
        virtual float getBatteryVoltage() final;
        virtual float BatteryRead() final;
        virtual bool IsDark() final;
        IRAM_ATTR virtual void handleInterrupt() final;
        void drawChargeMe();
        void drawStatus();
        virtual void VibeTo(bool Mode) final;
        virtual String MakeTime(int Hour, int Minutes, bool& Alarm) final;
        virtual String MakeHour(uint8_t Hour) final;
        virtual String MakeMinutes(uint8_t Minutes) final;
        virtual uint16_t ForeColor() final;
        virtual uint16_t BackColor() final;
        void InsertPost();
        void InsertBitmap();
        void InsertDefaults();
        void InsertOnMinute();
        void InsertWiFi();
        void InsertWiFiEnding();
        void InsertAddWatchStyles();
        void InsertDrawWatchStyle(uint8_t StyleID);
        void InsertInitWatchStyle(uint8_t StyleID);
        virtual uint8_t AddWatchStyle(String StyleName) final;
        String InsertNTPServer();
        virtual void AllowDefaultWatchStyles(bool Allow = true) final;
        virtual void AskForWiFi() final;
        virtual wl_status_t currentWiFi() final;
        virtual void endWiFi() final;
        virtual void getAngle(uint16_t Angle, uint8_t Away, uint8_t &X, uint8_t &Y) final;
        virtual bool SafeToDraw() final;
        void initWatchFaceStyle();
        void drawWatchFaceStyle();
        weatherData askForWeather();
        bool noAlpha(String str);
        int rtcTemp();
        const char* dayNames[7] = { "SUN" , "MON" , "TUE" , "WED" , "THU" , "FRI" , "SAT" };
        const char* monthNames[12] = { "JAN" , "FEB" , "MAR" , "APR" , "MAY" , "JUN" , "JUL" , "AUG" , "SEP" , "OCT" , "NOV" , "DEC" };
        uint16_t ambientOffset = 5; // This varies per RTC 
        void rightJustify(const String txt, uint16_t &yPos);

        // Add watchface draw function defs here        
        void drawAnalogTz();
        void drawBearWatchFace();
        void drawTomPetersonWatchFace();
        void drawTimeScreenFace();
        void drawLittleSunGazerWatchFace();
        
        // AnalogTz, BearTime, TomPeterson
        void drawAnalogHand(int16_t rOffset, int16_t width, int16_t len, float angle, bool color);
        void drawWeather();
        void waitForConnect();
        
        // little-sun-gazer
        float sun_paths(SolarThings &solar, time_t &utc_time, uint16_t &time_zone);
        void moon_earth(SolarThings &solar, time_t &utc_time, uint16_t &time_zone);
        
   private:
        void setStatus(String Status);
        void drawMenu();
        void drawData(String dData, byte Left, byte Bottom, WatchyGSR::DesOps Style, bool isTime = false, bool PM = false);
        void GoDark();
        void detectBattery();
        void processWxRequest();                     // GET WEATHER
        void ProcessNTP();
        void UpdateUTC();
        void UpdateClock();
        void ManageTime();
        void _rtcConfig();
        void _bmaConfig();
        void UpdateBMA();
        static uint16_t _readRegister(uint8_t address, uint8_t reg, uint8_t *data, uint16_t len);
        static uint16_t _writeRegister(uint8_t address, uint8_t reg, uint8_t *data, uint16_t len);
        void UpdateFonts();
        String MakeTOD(uint8_t Hour, bool AddZeros);
        String MakeSeconds(uint8_t Seconds);
        String MakeSteps(uint32_t uSteps);
        void CheckAlarm(int I);
        void CheckCD();
        void CalculateTones();
        void StopCD();
        uint8_t getToneTimes(uint8_t ToneIndex);
        String getReduce(uint8_t Amount);
        void monitorSteps();
        uint8_t getButtonPins();
        uint8_t getButtonMaskToID(uint64_t HW);
        uint8_t getSwapped(uint8_t pIn);        
        void processWiFiRequest();
        String WiFiIndicator(uint8_t Index);
        void UpdateWiFiPower(String SSID, String PSK);
        void UpdateWiFiPower(String SSID);
        void UpdateWiFiPower(uint8_t PWRIndex = 0);
        static void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
        String buildWiFiAPPage();
        void parseWiFiPageArg(String ARG, String DATA);
        void setupDefaults();
        String APIDtoString(uint8_t Index);
        String PASStoString(uint8_t Index);
        void initZeros();
        String GetSettings();
        void StoreSettings(String FromUser);
        void RetrieveSettings();
        void RecordSettings();
        bool OkNVS(String FaceName);
        void SetNVS(String FaceName, bool Enabled = true);
        void NVSEmpty();
        void SetTurbo();
        bool InTurbo();
        bool BedTime();
        bool UpRight();
        bool DarkWait();
        bool Showing();
        void RefreshCPU();
        void RefreshCPU(int Value);
        uint8_t getTXOffset(wifi_power_t Current);
        void DisplayInit(bool ForceDark = false);
        void DisplaySleep();
        //String getCityName();
};

//extern RTC_DATA_ATTR float sun_paths(SolarThings &solar, time_t utc_time, uint16_t time_zone);
//extern RTC_DATA_ATTR void moon_earth(SolarThings &solar, time_t utc_time, uint16_t time_zone);

extern RTC_DATA_ATTR StableBMA SBMA;

#endif
