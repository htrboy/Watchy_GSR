// WEATHER ICONS AND OTHER CODE FROM dezign999

void WatchyGSR::drawAnalogTzWatchFace() {

extern int8_t watchFace;
extern int8_t anaTzWatchAction; // timezone

WatchyGSR aGSR;

#define ANATZ_TIME_FONT aAntiCorona12pt7b  //FreeSansBold12pt7b
#define ANATZ_SMALL_TEXT smTextMono8pt7b
#define ANATZ_DATE_FONT aAntiCorona10pt7b  //FreeSerifBoldItalic10pt7b
//#define ANATZ_TEMP_FONT smTextMono8pt7b
#define ambientOffset 6

bool isNight = (WatchTime.Local.Hour >= 19 || WatchTime.Local.Hour <= 5) ? true : false;
bool lightMode = (BackColor() == GxEPD_WHITE) ? true : false;

extern char weatherDescription[20];
extern double wxCode;
extern double currentTemperature;
//extern String city;
const unsigned char* weatherIcon;

extern bool checkWx;


const char* dayNames[7] = { "SUN" , "MON" , "TUE" , "WED" , "THU" , "FRI" , "SAT" };
const char* monthNames[12] = { "JAN" , "FEB" , "MAR" , "APR" , "MAY" , "JUN" , "JUL" , "AUG" , "SEP" , "OCT" , "NOV" , "DEC" };
// Timezones need tweaking
const char* pTimeZones[23] = {"UTC", "LON", "PAR", "CAI", "MOW", "DXB", "KHI", "BKK", "HKG", "TYO", "SYD", "HON", "AKL", "MDY", "HNL", "ANC", "PDX", "DEN", "CHI", "NYC", "RIO", "FEN", "PDL"};


//const unsigned char weatherIcon = "";
const int outsideTemp = 0;

String d_gmtTimeString;
String gmtTimeString;
String d_gmtHour;
String currentHour;
String currentMinute;
String gmtMinStr = "";
String d_timeString = "";
String timeZoneString;

int tzHour = 0;
int GMTHour = 0;
int GMTMinute = 0;


watchFace = 4; // set watchface for actions


// keep timezone within limits
if (anaTzWatchAction < 0) {
  anaTzWatchAction = 23;
} else if (anaTzWatchAction > 23) {
  anaTzWatchAction = 0;
}

// get timezone name
 timeZoneString = pTimeZones[anaTzWatchAction];

// wrap to correct tzhour
tzHour = WatchTime.UTC.Hour + (anaTzWatchAction); // +- adjustment?
 if (tzHour > 23) {
   tzHour -= 23;
 } 


// draw watchface
display.fillScreen(BackColor());
display.drawBitmap(0, 0, clockFace_ana_bl, DISPLAY_WIDTH, DISPLAY_HEIGHT, lightMode ? GxEPD_BLACK : GxEPD_WHITE);  


// draw timezone airport name
//timeZoneString = mTimeZones[anaTzWatchAction];
display.setFont(&ANATZ_DATE_FONT);
display.setTextColor(lightMode ? GxEPD_BLACK : GxEPD_WHITE);
display.setCursor(20, 62);
display.print(timeZoneString);


// draw Battery    
display.fillRoundRect(138, 28, 34, 12, 4, lightMode ? GxEPD_BLACK : GxEPD_WHITE); //x+5
display.fillRoundRect(171, 32, 3, 4, 2, lightMode ? GxEPD_BLACK : GxEPD_WHITE); //x+5
display.fillRoundRect(140, 30, 30, 8, 3, lightMode ? GxEPD_WHITE : GxEPD_BLACK); //x+5
float batt = (getBatteryVoltage() - 3.3) / 0.9;
if (batt > 0) {
  display.fillRoundRect(142, 32, 26 * batt, 4, 2, (lightMode ? GxEPD_BLACK : GxEPD_WHITE)); //x+5
}


if (isNight)
   wxCode = 998;
 if (wxCode == 999 || 0) { //RTC
   weatherIcon = rtc;
 } else if (wxCode == 998) { //RTC SLEEEP
   weatherIcon = rtcsleep;
 } else if (wxCode > 801 && wxCode < 805) { //Cloudy
   weatherIcon = scatteredclouds;
 } else if (wxCode == 801) { //Few Clouds
   weatherIcon = (isNight) ? fewcloudsnight : fewclouds;
 } else if (wxCode == 800) { //Clear
   weatherIcon = (isNight) ? clearskynight : clearsky;
 } else if (wxCode >= 700) { //Atmosphere
   weatherIcon = mist;
 } else if (wxCode >= 600) { //Snow
   weatherIcon = snow;
 } else if (wxCode >= 500) { //Rain
   weatherIcon = rain;
 } else if (wxCode >= 300) { //Drizzle
   weatherIcon = drizzle;
 } else if (wxCode >= 200) { //Thunderstorm
   weatherIcon = thunderstorm;
 }
 display.fillRect(75, 51, 49, 44, lightMode ? GxEPD_WHITE : GxEPD_BLACK); //Redraw Helper Y=71
 display.drawBitmap(77, 53, weatherIcon, 45, 40, lightMode ? GxEPD_BLACK : GxEPD_WHITE); // Y=73

// WX TEMP
 display.setFont(&ANATZ_SMALL_TEXT);
 display.setTextColor(lightMode ? GxEPD_BLACK : GxEPD_WHITE);
 display.setCursor(30, 115);
 display.print(currentTemperature == 999 ? String("--") : int(round(currentTemperature*1.8+32)) + String("."));
// WX CONDITION
display.setFont(&ANATZ_DATE_FONT);
display.setTextColor(lightMode ? GxEPD_BLACK : GxEPD_WHITE);
//centerJustify(city, 100, 115);
 centerJustify(String(weatherDescription), 100, 115);


 // Watchy internal temp
display.setFont(&ANATZ_SMALL_TEXT);
display.setTextColor(lightMode ? GxEPD_BLACK : GxEPD_WHITE);
String temp = String(sensorTemp());
String tempString = temp + ".";
centerJustify(tempString , 155, 115);


// Steps
// Reset at midnight - this also resets GSR's 'yesterday steps' display
// if (WatchTime.Local.Hour == 23 && WatchTime.Local.Minute == 59) {
//      SBMA.resetStepCounter();
// }

uint32_t stepCount = SBMA.getCounter();
    
// Steps
String stepString = ("Steps: " + String(stepCount));
display.setFont(&ANATZ_DATE_FONT);
display.setTextColor(lightMode ? GxEPD_BLACK : GxEPD_WHITE);
//display.setTextColor(ForeColor());
//display.setCursor(50, 150);
//display.print(stepString);
centerJustify(stepString, 100, 150);
    

// 24h Local Time
//currentHour = String(((WatchTime.Local.Hour + tzHour) < 10) ? "0" + String(WatchTime.Local.Hour+tzHour) : String(WatchTime.Local.Hour+tzHour));
currentHour = String((tzHour < 10) ? "0" + String(tzHour) : String(tzHour));
currentMinute = String((WatchTime.Local.Minute < 10) ? "0" + String(WatchTime.Local.Minute) : String(WatchTime.Local.Minute));  
d_timeString = String(currentHour + ":" + currentMinute);
display.setFont(&ANATZ_TIME_FONT);
display.setTextColor(lightMode ? GxEPD_BLACK : GxEPD_WHITE);
display.setCursor(20, 40);
display.print(d_timeString); 

    
// GMT Time
GMTHour = WatchTime.UTC.Hour;
GMTMinute = WatchTime.UTC.Minute;  
gmtTimeString = GMTHour < 10 ? "0" + String(GMTHour) : String(GMTHour);
d_gmtTimeString = gmtTimeString + "Z";
display.setFont(&ANATZ_TIME_FONT);
display.setTextColor(lightMode ? GxEPD_BLACK : GxEPD_WHITE);
display.setCursor(20, 85);
display.print(d_gmtTimeString);

  
// Date
String wDay = dayNames[WatchTime.Local.Wday];
String _month = monthNames[WatchTime.Local.Month];
String _day = String(WatchTime.Local.Day);
String dateString = wDay + " " + _month + " " + _day;
display.setFont(&ANATZ_DATE_FONT);
display.setTextColor(lightMode ? GxEPD_BLACK : GxEPD_WHITE);
//display.setCursor(65, 173);
//display.print(dateString);
centerJustify(dateString, 100, 173);




// drawHand(offset, width, length, angle, color)  
//Minute
drawAnalogHand(60, 4, 50, (WatchTime.Local.Minute / 60.0 * -3.1415926 * 2), (lightMode ? false : true));
//Hour
drawAnalogHand(60, 4, 30, ((WatchTime.Local.Hour + WatchTime.Local.Minute / 60.0) / 12.0 * -3.1415926 * 2), (lightMode ? false : true));

} // end watchface 


void WatchyGSR::centerJustify(const String txt, uint16_t xPos, uint16_t yPos) {
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(txt, 0, 0, &x1, &y1, &w, &h);
  display.setCursor(xPos - w / 2, yPos);
  display.print(txt);
}

void WatchyGSR::drawAnalogHand(int16_t rOffset, int16_t width, int16_t len, float angle, bool color) {
  // true = draw white
  // false = draw black
  bool darkMode = color;

  //xo added
  const int16_t xo = (rOffset * sin(angle));
  const int16_t yo = (rOffset * cos(angle));

  const int16_t xc = 100; // X center
  const int16_t yc = 100; // Y center

  int16_t xe = floor(-len * sin(angle));
  int16_t ye = floor(-len * cos(angle));

  int16_t xb = round(-ye / (len / width));
  int16_t yb = round(xe / (len / width));

  xe += (xc - xo);
  ye += (yc - yo);

  //display.drawLine(xc, yc, xe, ye, (darkMode ? GxEPD_BLACK : GxEPD_WHITE));
  display.fillTriangle(xc - xb - xo, yc - yb - yo, xc + xb - xo, yc + yb - yo, xe + xb, ye + yb, (darkMode ? GxEPD_WHITE : GxEPD_BLACK));
  display.fillTriangle(xc - xb - xo, yc - yb - yo, xe + xb, ye + yb, xe - xb, ye - yb, (darkMode ? GxEPD_WHITE : GxEPD_BLACK));
}

uint8_t WatchyGSR::sensorTemp() {
  uint8_t temperature;
  temperature = (SBMA.readTemperature() - ambientOffset) * 9 / 5 + 32;
  return temperature; // in Fahrenheit
}



