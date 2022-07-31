
// By Dezign999, here: https://github.com/dezign999/watchy999
// check his watch faces, they're great

void WatchyGSR::drawSynthWatchFace() {

#define SYNTH_TIME_FONT absinth44pt7b
#define SYNTH_DATE_FONT synthDate14pt7b
#define SYNTH_SMALL_TEXT tinyPixel10pt7b

extern int8_t watchFace;

  bool lightMode = (BackColor() == GxEPD_WHITE) ? false : true;
  extern bool synthWatchAction999;
  const unsigned char *abSM [10] = {ab0, ab1, ab2, ab3, ab4, ab5, ab6, ab7, ab8, ab9};

  String synthCurrentHour;
  String synthCurrentMinute;
  
  char latestTimeHour1;
  char latestTimeHour2;
  char latestTimeMin1;
  char latestTimeMin2;
  
  display.fillScreen((lightMode) ? GxEPD_BLACK : GxEPD_WHITE);
  display.setFont(&SYNTH_TIME_FONT);
  display.setTextColor((lightMode) ? GxEPD_WHITE : GxEPD_BLACK);

  const char* dayNames[7] = { "SUN" , "MON" , "TUE" , "WED" , "THU" , "FRI" , "SAT" };
  const char* monthNames[12] = { "JAN" , "FEB" , "MAR" , "APR" , "MAY" , "JUN" , "JUL" , "AUG" , "SEP" , "OCT" , "NOV" , "DEC" };
  const char* dayAbbrev = dayNames[WatchTime.Local.Wday];
  const char* monthAbbrev = monthNames[WatchTime.Local.Month];

  synthCurrentHour = String((WatchTime.Local.Hour < 10) ? "0" + String(WatchTime.Local.Hour) : String(WatchTime.Local.Hour));
  synthCurrentMinute = String((WatchTime.Local.Minute < 10) ? "0" + String(WatchTime.Local.Minute) : String(WatchTime.Local.Minute));

  latestTimeHour1 = MakeHour(WatchTime.Local.Hour/10).charAt(0);
  latestTimeHour2 = MakeHour(WatchTime.Local.Hour%10).charAt(0);
  latestTimeMin1 = MakeMinutes(WatchTime.Local.Minute/10).charAt(0);
  latestTimeMin2 = MakeMinutes(WatchTime.Local.Minute%10).charAt(0);

  String wDay = dayNames[WatchTime.Local.Wday];
  String _month = monthNames[WatchTime.Local.Month-1];
  String _day = String(WatchTime.Local.Day);
  String dateString = wDay + " " + _month + " " + _day;

  watchFace = 6; // set watchface for actions

  if (synthWatchAction999) {
    display.fillRect(10, 10, 180, 180, (lightMode) ? GxEPD_BLACK : GxEPD_WHITE); //Redraw Helper
    //Hour
    display.setCursor(18, 102);
    display.print(synthCurrentHour);
    //Minute
    display.setCursor(18, 182);
    display.print(synthCurrentMinute);
  } else {
    display.fillRect(12, 13, 130, 130, (lightMode) ? GxEPD_BLACK : GxEPD_WHITE); //Redraw Helper
    display.drawBitmap(18, 19, abSM[latestTimeHour1 - '0'], 61, 61, (lightMode) ? GxEPD_WHITE : GxEPD_BLACK); //hour digit 1
    display.drawBitmap(75, 19, abSM[latestTimeHour2 - '0'], 61, 61, (lightMode) ? GxEPD_WHITE : GxEPD_BLACK); //hour digit 2
    display.drawBitmap(18, 76, abSM[latestTimeMin1 - '0'], 61, 61, (lightMode) ? GxEPD_WHITE : GxEPD_BLACK); //minute digit 1
    display.drawBitmap(75, 76, abSM[latestTimeMin2 - '0'], 61, 61, (lightMode) ? GxEPD_WHITE : GxEPD_BLACK); //minute digit 2
  }

  if (!synthWatchAction999) {
    //Date
    display.fillRect(142, 88, 45, 55, (lightMode) ? GxEPD_BLACK : GxEPD_WHITE); //Redraw Helper
    display.setTextColor((lightMode) ? GxEPD_WHITE : GxEPD_BLACK);
    display.setFont(&SYNTH_DATE_FONT);
    display.setCursor(144, 101 ); //120
    display.print(dateString);
    display.setFont(&SYNTH_SMALL_TEXT);
    display.setCursor(144, 119); //98
    display.print(monthAbbrev);
    display.setCursor(144, 137);
    display.print(dayAbbrev);

    //Battery
    int dotX = 21;
    for (uint8_t i = 0; i < 21; i++) {
      display.fillRect(dotX, 154, 2, 2, (lightMode) ? GxEPD_BLACK : GxEPD_WHITE);
      dotX += 8;
    }

//    float battery =  analogReadMilliVolts(ADC_PIN) / 1000.0f * 2.0f;
    float battery = getBatteryVoltage();
    float batt = (battery - (4.2 - 0.9)) / 0.9;
    if (batt > 0) {
      display.fillRect(18, 154, 165 * batt, 2, (lightMode) ? GxEPD_BLACK : GxEPD_WHITE);
    }

    //Temp
    display.fillRect(12, 167, 188, 21, (lightMode) ? GxEPD_WHITE : GxEPD_BLACK); //Redraw Helper
    display.setTextColor((lightMode) ? GxEPD_BLACK : GxEPD_WHITE);
    display.setFont(&SYNTH_SMALL_TEXT);
    display.setCursor(18, 182);
    display.println(String(sensorTemp())); // + "' " + tempCondition);
  } 

}
