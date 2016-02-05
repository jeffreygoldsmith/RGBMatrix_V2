#include "Arduino.h"
#include <Time.h>
#include "RGB_Matrix.h"
#include <Adafruit_NeoPixel.h>

struct tm
{
  int y;    // Year
  int mon;  // Month
  int d;    // Day
  int wd;   // Weekday
  int h;    // Hour
  int m;    // Minute
  int s;    // Second
} tm;

struct tm Decode(time_t ts)
{
  int d, m, y;

  tm.s = ts % 60; // Seconds
  ts /= 60;

  tm.m = ts % 60; // Minutes
  ts /= 60;

  tm.h = ts % 24; // Hours
  ts /= 24;

  ts = ts * 100 + 36525 * 2 - 5950; // Move start of epoch to 1 March 1968 and scale
  y = (ts + 75) / 36525; // Years since start of epoch
  d = (175 + ts - y * 36525) / 100 * 10; // Days since start of year (times 10)
  m = (d - 6) / 306; // Months since start of year

  tm.y = 1968 + y + (m >= 10); // Year
  tm.mon = 1 + (m + 2) % 12; // Month
  tm.d = (4 + d - m * 306) / 10; // Day
  tm.wd = 1 + (5 + ts / 100) % 7; // Day of week (Sunday = 1)

  return tm;
}

Matrix::Matrix (byte dataIn, byte ledNum, byte brightness)
{
  pinMode(dataIn, OUTPUT);
  Adafruit_NeoPixel matrix = Adafruit_NeoPixel(ledNum, dataIn, NEO_GRB);
  matrix.begin();
  matrix.setBrightness(brightness);
  matrix.show();
}

Time::Time ()
{
  _pSecond = 0;
}

void Time::ChangeTime (byte setButton, byte rowButton, byte upButton, byte downButton)
{
  _rowNumber = 0;
   
  void flash()
  {
    
  }
    
  if (digitalRead(setButton)
  {
    delay(10);
    if (digitalRead(setButton)
  }
}

