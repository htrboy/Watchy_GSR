#include "Watchy_GSR.h"
#include "secrets.h"
#include "analogTz.h"
#include "bearTime.h"
#include "tomPeterson.h"
#include "synth999.h"

// Place all of your data and variables here.
// Remember RTC_DATA_ATTR for your variables so they don't get wiped on deep sleep.


/*
 * WEATHER AND GEOLOCATE VARS
 * WEATHER FETCH CODE BY ZeroKelvinKeyboard
*/
#include <GeoLocate.h>

enum wifiStatusOptions {
  WIFI_NOT_RUNNING,
  WIFI_REQUESTED,
  WIFI_READY,
  WIFI_DONE
};

RTC_DATA_ATTR uint8_t wifiStatus = WIFI_NOT_RUNNING;
RTC_DATA_ATTR uint8_t wifiRequest = 0;
RTC_DATA_ATTR uint8_t wifiInserts = 0;

#define WIFI_REQUEST_WEATHER      0x01

GeoLocate Geo;

RTC_DATA_ATTR unsigned long geolocStartTime;
RTC_DATA_ATTR time_t lastWeatherReport = 0;
RTC_DATA_ATTR double minTemperature = 0;
RTC_DATA_ATTR double maxTemperature = 0;
RTC_DATA_ATTR double currentTemperature = 0;
RTC_DATA_ATTR char weatherDescription[20] = "N/A";
RTC_DATA_ATTR double windSpeed  = 0;
RTC_DATA_ATTR double wxCode = 999;
RTC_DATA_ATTR bool checkWx = false;

// Weather checking states
enum weatherUpdateStatus {
  WEATHER_UPDATE_INACTIVE,
  WEATHER_UPDATE_GEOLOC,
  WEATHER_UPDATE_OWN_FETCH,
  WEATHER_UPDATE_FAILED
};

enum weatherUpdateStatus weatherStatus;

// The base URL for OpenWeatherMap. This is the v2.5 API.
#define OWN_BASE_URL "http://api.openweathermap.org/data/2.5/weather"
// The time in milliseconds to wait to a GeoLocate request.
#define GEOLOC_TIMEOUT 3000
// The minimum time in seconds to wait between weather updates.
#define WEATHER_UPDATE_INTERVAL 1800 // every 30 minutes


/*
 * WATCHFACE VARS
*/
RTC_DATA_ATTR int8_t watchFace;
RTC_DATA_ATTR uint8_t MyStyle_3;  // 2 BearTime
RTC_DATA_ATTR uint8_t MyStyle_4;  //AnalogTz
RTC_DATA_ATTR uint8_t MyStyle_5;  //TomPeterson
RTC_DATA_ATTR uint8_t MyStyle_6;  //Synth999
RTC_DATA_ATTR uint8_t MyStyle_7;  //LSG

RTC_DATA_ATTR uint8_t backColor = GxEPD_WHITE;
RTC_DATA_ATTR bool lightMode = (backColor == GxEPD_WHITE) ? true : false;

RTC_DATA_ATTR bool synthWatchAction999 = false;
RTC_DATA_ATTR uint8_t anaTzWatchAction = 16;
RTC_DATA_ATTR uint8_t actionID;
RTC_DATA_ATTR const char* wxCondition;


class OverrideGSR : public WatchyGSR {
/*
 * Keep your functions inside the class, but at the bottom to avoid confusion.
 * Be sure to visit https://github.com/GuruSR/Watchy_GSR/blob/main/Override%20Information.md for full information on how to override
 * including functions that are available to your override to enhance functionality.
*/



  public:
    OverrideGSR() : WatchyGSR() {}

 

    void InsertPost(){
      if (USEDEBUG) {
        Serial.begin(115200);
      }
    };


/*
    String InsertNTPServer() { return "<your favorite ntp server address>"; }
*/

/*
    void InsertDefaults(){
    };
*/

/*
    bool OverrideBitmap(){
      return false;
    };
*/


    // Check if it's time for a weather update.
    // and only update if a screen displays weather
    void InsertOnMinute() {
      if (watchFace == 4) {
        checkNetItems();
      }
    };



    void InsertWiFi() {
      if (USEDEBUG) {
        char debug[] {"InsertWifi - WifiStatus"};
        log_d("DEBUG %s", debug);
        Serial.println(String(wifiStatus));
      }
      // Only respond to this function if WiFi was requested by this code.
      if (wifiStatus == WIFI_REQUESTED) {
        wifiStatus = WIFI_READY;
      } else if (wifiStatus != WIFI_READY) {
        return;
      }

      // Run network actions for whatever requested it.
      if (wifiRequest & WIFI_REQUEST_WEATHER) {
        updateWeather();
      // else if, etc only one at a time. highest priority first
      }

      // If there's no more requests, tell the OS we're done with the WiFi.
      if (wifiRequest == 0) {
        wifiStatus = WIFI_DONE;
        endWiFi();
      }
    };
    



void InsertWiFiEnding() {
  if (USEDEBUG) {
    char debug[] {"WifiEnding - WifiStatus"};
    log_d("DEBUG %s", debug);
    Serial.println(String(wifiStatus));
  }
      // Make sure WiFi code doesn't send another WiFi request until this block is done.
      if (wifiStatus == WIFI_DONE) {
        wifiStatus = WIFI_NOT_RUNNING;
      }
    };



// The next 3 functions allow you to add your own WatchFaces, there are examples that do work below.
    void InsertAddWatchStyles(){
      MyStyle_3 = AddWatchStyle("BearTime");
      MyStyle_4 = AddWatchStyle("AnalogTz");
      MyStyle_5 = AddWatchStyle("TomPeterson");
      MyStyle_6 = AddWatchStyle("Synth999");
      //MyStyle_7 = AddWatchStyle("LSG");
    };



    void InsertInitWatchStyle(uint8_t StyleID){
      
      //              Bear        Analog        Tom         Synth
      if (StyleID == MyStyle_3 || MyStyle_5 || MyStyle_6 ) {
          Design.Menu.Top = 72;
          Design.Menu.Header = 25;
          Design.Menu.Data = 66;
          Design.Menu.Gutter = 3;
          Design.Menu.Font = &aAntiCorona12pt7b;
          Design.Menu.FontSmall = &aAntiCorona11pt7b;
          Design.Menu.FontSmaller = &aAntiCorona10pt7b;
          Design.Face.Bitmap = nullptr;
          Design.Face.SleepBitmap = nullptr;
          Design.Face.Gutter = 4;
          Design.Face.Time = 56;
          Design.Face.TimeHeight = 45;
          Design.Face.TimeColor = lightMode ? GxEPD_BLACK : GxEPD_WHITE;
          Design.Face.TimeFont = &aAntiCorona36pt7b;
          Design.Face.TimeLeft = 0;
          Design.Face.TimeStyle = WatchyGSR::dCENTER;
          Design.Face.Day = 101;
          Design.Face.DayGutter = 4;
          Design.Face.DayColor = lightMode ? GxEPD_BLACK : GxEPD_WHITE;
          Design.Face.DayFont = &aAntiCorona16pt7b;
          Design.Face.DayFontSmall = &aAntiCorona15pt7b;
          Design.Face.DayFontSmaller = &aAntiCorona14pt7b;
          Design.Face.DayLeft = 0;
          Design.Face.DayStyle = WatchyGSR::dCENTER;
          Design.Face.Date = 143;
          Design.Face.DateGutter = 4;
          Design.Face.DateColor = lightMode ? GxEPD_BLACK : GxEPD_WHITE;
          Design.Face.DateFont = &aAntiCorona15pt7b;
          Design.Face.DateFontSmall = &aAntiCorona14pt7b;
          Design.Face.DateFontSmaller = &aAntiCorona13pt7b;
          Design.Face.DateLeft = 0;
          Design.Face.DateStyle = WatchyGSR::dCENTER;
          Design.Face.Year = 186;
          Design.Face.YearLeft = 99;
          Design.Face.YearColor = lightMode ? GxEPD_BLACK : GxEPD_WHITE;
          Design.Face.YearFont = &aAntiCorona16pt7b;
          Design.Face.YearLeft = 0;
          Design.Face.YearStyle = WatchyGSR::dCENTER;
          Design.Status.WIFIx = 5;
          Design.Status.WIFIy = 193;
          Design.Status.BATTx = 155;
          Design.Status.BATTy = 178;
      } else if (StyleID == MyStyle_4) {
          Design.Menu.Top = 72;
          Design.Menu.Header = 25;
          Design.Menu.Data = 66;
          Design.Menu.Gutter = 3;
          Design.Menu.Font = &aAntiCorona12pt7b;
          Design.Menu.FontSmall = &aAntiCorona11pt7b;
          Design.Menu.FontSmaller = &aAntiCorona10pt7b;
          Design.Face.Bitmap = nullptr;
          Design.Face.SleepBitmap = nullptr;
          Design.Face.Gutter = 4;
          Design.Face.Time = 56;
          Design.Face.TimeHeight = 45;
          Design.Face.TimeColor = lightMode ? GxEPD_BLACK : GxEPD_WHITE;
          Design.Face.TimeFont = &aAntiCorona36pt7b;
          Design.Face.TimeLeft = 0;
          Design.Face.TimeStyle = WatchyGSR::dCENTER;
          Design.Face.Day = 101;
          Design.Face.DayGutter = 4;
          Design.Face.DayColor = lightMode ? GxEPD_BLACK : GxEPD_WHITE;
          Design.Face.DayFont = &aAntiCorona16pt7b;
          Design.Face.DayFontSmall = &aAntiCorona15pt7b;
          Design.Face.DayFontSmaller = &aAntiCorona14pt7b;
          Design.Face.DayLeft = 0;
          Design.Face.DayStyle = WatchyGSR::dCENTER;
          Design.Face.Date = 143;
          Design.Face.DateGutter = 4;
          Design.Face.DateColor = lightMode ? GxEPD_BLACK : GxEPD_WHITE;
          Design.Face.DateFont = &aAntiCorona15pt7b;
          Design.Face.DateFontSmall = &aAntiCorona14pt7b;
          Design.Face.DateFontSmaller = &aAntiCorona13pt7b;
          Design.Face.DateLeft = 0;
          Design.Face.DateStyle = WatchyGSR::dCENTER;
          Design.Face.Year = 186;
          Design.Face.YearLeft = 99;
          Design.Face.YearColor = lightMode ? GxEPD_BLACK : GxEPD_WHITE;
          Design.Face.YearFont = &aAntiCorona16pt7b;
          Design.Face.YearLeft = 0;
          Design.Face.YearStyle = WatchyGSR::dCENTER;
          Design.Status.WIFIx = 5;
          Design.Status.WIFIy = 130;
          Design.Status.BATTx = 155;
          Design.Status.BATTy = 130;
      }
    };



    void InsertDrawWatchStyle(uint8_t StyleID) {
      actionID = StyleID; // assign for use by other functions
      if (StyleID == MyStyle_3){
            if (SafeToDraw()) {
                drawBearWatchFace();
//                drawDay();
//                drawYear();
            }
            //if (NoMenu()) drawDate();
      } else if ( StyleID == MyStyle_4) {
        if (SafeToDraw()){
                drawAnalogTzWatchFace();
//                drawDay();
//                drawYear();
            }
      } else if ( StyleID == MyStyle_5) {
        if (SafeToDraw()){
                drawTomPetersonWatchFace();
//                drawDay();
//                drawYear();
            }
      }  else if ( StyleID == MyStyle_6) {
        if (SafeToDraw()){
                drawSynthWatchFace();
//                drawDay();
//                drawYear();
            }
      } 
    };



    bool InsertHandlePressed(uint8_t SwitchNumber, bool &Haptic, bool &Refresh) {
      
      switch (SwitchNumber) {

        case 2: //Back
          Haptic = true;  // Cause Hptic feedback if set to true.
          Refresh = true; // Cause the screen to be refreshed (redrwawn).
          return true;  // Respond with "I used a button", so the WatchyGSR knows you actually did something with a button.
          break;

        case 3: //Up   
        if (watchFace == 4) { // anaTz
          anaTzWatchAction++;        // increase timezone
          Haptic = true;
          Refresh = true;          
          return true; 
          break;  
        } else if (watchFace == 6) {
          synthWatchAction999 = !synthWatchAction999; // toggle the screen
          Haptic = true;
          Refresh = true;          
          return true;
          break;        
        }          
          // anaTzWatchAction++;        // increase timezone            
          // synthWatchAction999 = !synthWatchAction999; // toggle the screen
          // Haptic = true;
          // Refresh = true;          
        return false;        
        break;

        case 4: //Down  
          if (watchFace == 4) { // anaTz
          anaTzWatchAction--;        // decrease timezone 
          Haptic = true;
          Refresh = true;          
          return true;
          break;            
        }        
          
      }
      return false;
    };


/*
    bool OverrideSleepBitmap(){
      return false;
    };
*/

void checkNetItems() {
  if (USEDEBUG) {
    //Serial.println("CheckNetItems");
    char debug[] {"CheckNetItems"};
    log_d("DEBUG %s", debug);
    log_d("DEBUG: WX diff %d", WatchTime.UTC_RAW - lastWeatherReport);
    log_d("DEBUG: watchFace %d", watchFace);
  }
    
    if ((difftime(WatchTime.UTC_RAW, lastWeatherReport ) > WEATHER_UPDATE_INTERVAL))  {         
      // Set the weather bit for the WiFi code later.
      wifiRequest |= WIFI_REQUEST_WEATHER;
    }

    // If WiFi is needed for something and it is not busy, request WiFi from the GSR code.
    if (wifiRequest != 0 && wifiStatus == WIFI_NOT_RUNNING) {
      wifiStatus = WIFI_REQUESTED;
      AskForWiFi();
    }
  
};

void updateWeather() {
    // Weather state machine. Does only one WiFi request per call.
    String desc;

    if (weatherStatus == WEATHER_UPDATE_INACTIVE) {
      // Begin by starting GeoLocate.
      if (!Geo.beginGeoFromWeb()) {
        weatherStatus = WEATHER_UPDATE_FAILED;
      } else {
        // If it successfully starts, record the time the request started.
        weatherStatus = WEATHER_UPDATE_GEOLOC;
        geolocStartTime = millis();
      }
    } else if (weatherStatus == WEATHER_UPDATE_GEOLOC) {
      // Wait for GeoLocate.
      if (Geo.gotGeoFromWeb()) {
        // If successful we've got our geo position, move on to getting the weather.
        weatherStatus = WEATHER_UPDATE_OWN_FETCH;
      } else if (millis() - geolocStartTime > GEOLOC_TIMEOUT) {
        // Time out and abort the request if it takes too long.
        weatherStatus = WEATHER_UPDATE_FAILED;
        wxCode = 999;        
        currentTemperature = 999;
        desc = "NA";
        strncpy(weatherDescription, desc.c_str(), sizeof(weatherDescription));
      }
    } else if (weatherStatus == WEATHER_UPDATE_OWN_FETCH) {
      // End the active web connections from GeoLocate
      Geo.endGeoFromWeb();
      // Read the weather data and finish.
      fetchOWNData();
      weatherStatus = WEATHER_UPDATE_INACTIVE;
    // Remember the last timestamp when a fetch was attempted so we know how long it's been.
    // This happens *before* checking if the request actually succeeded.
    // We don't want the API sending requests every minute if there's no WiFi, bad for battery.
      lastWeatherReport = WatchTime.UTC_RAW;
      // Remove the request bit so we'll stop getting calls to this function until needed again.
      wifiRequest &= ~WIFI_REQUEST_WEATHER;
    } else if (weatherStatus == WEATHER_UPDATE_FAILED) {
      wifiRequest &= ~WIFI_REQUEST_WEATHER;
      weatherStatus = WEATHER_UPDATE_INACTIVE;
      lastWeatherReport = WatchTime.UTC_RAW;
      wxCode = 999;
      currentTemperature = 999;
    }
  };

void fetchOWNData() {
    if (USEDEBUG) {
      char debug[] {"fetchOWNData"};
      log_d("DEBUG %s", debug);
    }
      int response;
      String desc;
      HTTPClient http;
      char ownUrl[128];

      // Grab the co-ordinates from GeoLocate
      char *lat = Geo.Latitude;
      char *lon = Geo.Longitude;
      //char *cty = Geo.City;

      // Generate a URL for the API and send the request over the internet.
      snprintf(ownUrl, sizeof(ownUrl), "%s?lat=%s&lon=%s&units=metric&appid=%s", OWN_BASE_URL, lat, lon, OWN_KEY);
      http.begin(ownUrl);
      response = http.GET();

      // Only update if request succeeded.
      if (response != 200) {
        // If not, clear the data
        wxCode = 999;
        desc = "NA";
        currentTemperature = 999;
        return;
      }

      // Now parse the data from the v2.5 API into variable to be printed later.
      JSONVar weather = JSON.parse(http.getString());
      maxTemperature = double(weather["main"]["temp_max"]);
      minTemperature = double(weather["main"]["temp_min"]);
      currentTemperature = double(weather["main"]["temp"]);
      windSpeed = double(weather["wind"]["speed"]);
      desc = weather["weather"][0]["main"];
      wxCode = weather["weather"][0]["id"];
      //city = weather["name"];
      strncpy(weatherDescription, desc.c_str(), sizeof(weatherDescription));
      http.end();
    };



};

// Do not edit anything below this, leave all of your code above.
OverrideGSR watchy;

void setup(){
  watchy.init();
}

void loop(){}
