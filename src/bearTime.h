void WatchyGSR::drawBearWatchFace() {

    bool lightMode = (BackColor() == GxEPD_WHITE) ? true : false;

    display.fillScreen(lightMode ? GxEPD_WHITE : GxEPD_BLACK);
    display.drawBitmap(0, 0, SoB_cf_inv, DISPLAY_WIDTH, DISPLAY_HEIGHT, lightMode ? GxEPD_BLACK : GxEPD_WHITE);

  //  int randomX1 = 0;
  //  int randomY1 = 0;
  //  int randomX2 = 0;
  //  int randomY2 = 0;

    int randomX1 = random(5, 40);
    int randomX2 = random(130, 165);
    int randomY1 = random(10, 50);
    int randomY2 = random(60, 125);

    display.drawBitmap(randomX1, randomY1, bee1, 27, 24, lightMode ? GxEPD_BLACK : GxEPD_WHITE);
    display.drawBitmap(randomX2, randomY1, bee2_inv, 23, 25, lightMode ? GxEPD_BLACK : GxEPD_WHITE);
    display.drawBitmap(randomX2, randomY2, bee3_inv, 23, 24, lightMode ? GxEPD_BLACK : GxEPD_WHITE);

    //int currentMinute = latestTime.minStr.toInt();
    //int currentHour = latestTime.hourStr.toInt(); // adjusted local time

  // drawHand(offset, width, length, angle, color (true=white))  
  //Minute
  drawAnalogHand(60, 4, 50, (WatchTime.Local.Minute / 60.0 * -3.1415926 * 2), (lightMode ? false : true));
  //Hour
  drawAnalogHand(60, 4, 30, ((WatchTime.Local.Hour + WatchTime.Local.Minute / 60.0) / 12.0 * -3.1415926 * 2), (lightMode ? false : true));

}
