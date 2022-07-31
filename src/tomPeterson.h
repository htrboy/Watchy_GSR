// Simple Tom Peterson Watchface

void WatchyGSR::drawTomPetersonWatchFace() {

  bool darkMode = true;

  display.fillScreen(GxEPD_WHITE);
  display.drawBitmap(0, 0, tomPeterson_inv, DISPLAY_WIDTH, DISPLAY_HEIGHT, GxEPD_BLACK);

  //int16_t currentMinute = latestTime.minStr.toInt();
  //int16_t currentHour = latestTime.hourStr.toInt();

// drawHand(offset, width, length, angle, color)  
//Minute
drawAnalogHand(60, 4, 50, (WatchTime.Local.Minute / 60.0 * -3.1415926 * 2), (darkMode? true : true));
//Hour
drawAnalogHand(60, 4, 30, ((WatchTime.Local.Hour + WatchTime.Local.Minute / 60.0) / 12.0 * -3.1415926 * 2), (darkMode? true : true));
}
