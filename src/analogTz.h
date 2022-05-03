void WatchyGSR::drawAnalogTz() {

#define ANATZ_TIME_FONT FreeSansBold12pt7b
#define ANATZ_SMALL_TEXT smTextMono8pt7b
#define ANATZ_DATE_FONT FreeSerifBoldItalic10pt7b
#define DISPLAY_WIDTH 200
#define DISPLAY_HEIGHT 200

// originally for showing that weather is not updating when sleeping
bool isNight = (WatchTime.Local.Hour >= 18 || WatchTime.Local.Hour <= 5) ? true : false;
bool darkMode = Options.LightMode;

//draw watchface
display.fillScreen(Options.LightMode ? GxEPD_BLACK : GxEPD_WHITE); //redraw helper
display.drawBitmap(0, 0, clockFace_ana_bl, DISPLAY_WIDTH, DISPLAY_HEIGHT, Options.LightMode ? GxEPD_WHITE : GxEPD_BLACK);              
    
String d_gmtTimeString;
String gmtTimeString;
String d_gmtHour;
//String d_timeZoneStr = timeZoneNameStr;
String currentHour;
String currentMinute;
int localHour = 0;
int GMTHour = 0;
int GMTMinute = 0;
String gmtMinStr = "";
String d_timeString = "";
//bool darkMode = true;

// Draw Battery    
display.fillRoundRect(138, 28, 34, 12, 4, Options.LightMode ? GxEPD_WHITE : GxEPD_BLACK); //x+5
display.fillRoundRect(171, 32, 3, 4, 2, Options.LightMode ? GxEPD_WHITE : GxEPD_BLACK); //x+5
display.fillRoundRect(140, 30, 30, 8, 3, !Options.LightMode ? GxEPD_WHITE : GxEPD_BLACK); //x+5
float batt = (getBatteryVoltage() - 3.3) / 0.9;
if (batt > 0) {
  display.fillRoundRect(142, 32, 26 * batt, 4, 2, (!Options.LightMode ? GxEPD_BLACK : GxEPD_WHITE)); //x+5
}

// Weather & Temp
// renew every 30 minutes - show last in meantime
if (WatchTime.Local.Minute % 30 == 0) {
  if (USEDEBUG) {
      Serial.println(String(WatchTime.Local.Hour)+String(WatchTime.Local.Minute));
      Serial.println("---Time for weather---");  
    }
    
    showCached = false;
    getWeather.updateWx = true;
//    getWeather.wait = 0;
    getWeather.state = 1;
    AskForWiFi();    
  } //else {
    //getWeather.state = 0;
    //showCached = true;  
//}

//Weather Icon
  const unsigned char* weatherIcon;
  
if (isNight)
    weatherConditionCode = 998;

  if (weatherConditionCode == 999 || 0) { //RTC
    weatherIcon = rtc;
  } else if (weatherConditionCode == 998) { //RTC SLEEEP
    weatherIcon = rtcsleep;
  } else if (weatherConditionCode > 801 && weatherConditionCode < 805) { //Cloudy
    weatherIcon = scatteredclouds;
  } else if (weatherConditionCode == 801) { //Few Clouds
    weatherIcon = (isNight) ? fewcloudsnight : fewclouds;
  } else if (weatherConditionCode == 800) { //Clear
    weatherIcon = (isNight) ? clearskynight : clearsky;
  } else if (weatherConditionCode >= 700) { //Atmosphere
    weatherIcon = mist;
  } else if (weatherConditionCode >= 600) { //Snow
    weatherIcon = snow;
  } else if (weatherConditionCode >= 500) { //Rain
    weatherIcon = rain;
  } else if (weatherConditionCode >= 300) { //Drizzle
    weatherIcon = drizzle;
  } else if (weatherConditionCode >= 200) { //Thunderstorm
    weatherIcon = thunderstorm;
  }
  display.fillRect(75, 71, 49, 44, darkMode? GxEPD_BLACK : GxEPD_WHITE); //Redraw Helper
  display.drawBitmap(77, 73, weatherIcon, 45, 40, darkMode ? GxEPD_WHITE : GxEPD_BLACK);

  int16_t  x1, y1;
  uint16_t w, h;
  display.setFont(&ANATZ_SMALL_TEXT);
  display.setTextColor(darkMode ? GxEPD_WHITE : GxEPD_BLACK);
  display.getTextBounds(String(temperature) + ".", 45, 13, &x1, &y1, &w, &h);
  display.setCursor(100 - w / 2, 128); // MINUS 66 XY
  display.println(String(temperature) + ".");

  //cityName = getCityName();  // This is my wifi name
  display.getTextBounds(cityName, 45, 13, &x1, &y1, &w, &h);
  display.setCursor(99 - w / 2, 67); // MINUS 66 XY
  display.println("PDX"); // hard coding this in until I can work on the code
  
// Steps
if (WatchTime.Local.Hour == 23 && WatchTime.Local.Minute == 59) {
      SBMA.resetStepCounter();
}
  uint32_t stepCount = SBMA.getCounter();
    
if (stepCount > 500) { // if you've actually been walking
    String stepString = ("Steps: " + String(stepCount));
    display.setFont(&ANATZ_DATE_FONT);
    display.setTextColor(Options.LightMode ? GxEPD_WHITE : GxEPD_BLACK);
    display.setCursor(50, 150);
    display.print(stepString);
    }
    
// GMT Time
GMTHour = WatchTime.UTC.Hour;
GMTMinute = WatchTime.UTC.Minute;  
gmtTimeString = GMTHour < 10 ? "0" + String(GMTHour) : String(GMTHour);
d_gmtTimeString = gmtTimeString + "Z";
display.setFont(&ANATZ_TIME_FONT);
display.setTextColor(Options.LightMode ? GxEPD_WHITE : GxEPD_BLACK);
display.setCursor(20, 65);
display.print(d_gmtTimeString);
  
// 24h Local Time
currentHour = String((WatchTime.Local.Hour < 10) ? "0" + String(WatchTime.Local.Hour) : String(WatchTime.Local.Hour));
currentMinute = String((WatchTime.Local.Minute < 10) ? "0" + String(WatchTime.Local.Minute) : String(WatchTime.Local.Minute));
  
d_timeString = String(currentHour + ":" + currentMinute);
display.setFont(&ANATZ_TIME_FONT);
display.setTextColor(Options.LightMode ? GxEPD_WHITE : GxEPD_BLACK);
display.setCursor(20, 40);
display.print(d_timeString); 

// Date
String wDay = dayNames[WatchTime.Local.Wday];
String _month = monthNames[WatchTime.Local.Month-1];
String _day = String(WatchTime.Local.Day);
String dateString = wDay + " " + _month + " " + _day;
display.setFont(&ANATZ_DATE_FONT);
display.setTextColor(Options.LightMode ? GxEPD_WHITE : GxEPD_BLACK);
display.setCursor(40, 173);
display.print(dateString);

// drawHand(offset, width, length, angle, color)  
//Minute
drawAnalogHand(60, 4, 50, (WatchTime.Local.Minute / 60.0 * -3.1415926 * 2), (darkMode? true : false));
//Hour
drawAnalogHand(60, 4, 30, ((WatchTime.Local.Hour + WatchTime.Local.Minute / 60.0) / 12.0 * -3.1415926 * 2), (darkMode? true : false));

}
