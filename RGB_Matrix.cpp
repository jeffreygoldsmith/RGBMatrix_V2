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

bool bitBoolean;
bool setBoolean;
static const byte ROW_NUM = 7;
static const byte LED_NUM = 64;
static const byte DIN_PIN = 13;
static const byte ROW_OFFSET = 8;

int timeArray[] = {tm.y, tm.mon, tm.wd, tm.d, tm.h, tm.m, tm.s};
byte timeRow[] = {7, 6, 5, 4, 3, 2, 1};
byte bitLength[] = {7, 4, 5, 3, 4, 6, 6};
unsigned long timeChange[] = {31557600, 2592000, 604800, 86400, 3600, 60, 1};

Adafruit_NeoPixel matrix = Adafruit_NeoPixel(LED_NUM, DIN_PIN, NEO_GRB);

void bitTime(int t, byte tLength, byte row)
{
  for (int i = 0; i < tLength; i++)
  {
    bitBoolean = bitRead(t, i); // Check each bit in t to be high or low

    if (bitBoolean == 1)  // If bit is high set LED to be high, else set low
      matrix.setPixelColor(i + (ROW_OFFSET * row), 255, 0, 0);
    else
      matrix.setPixelColor(i + (ROW_OFFSET * row), 0, 0, 0);

    matrix.show();
  }
}

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

bool

void Time::ChangeTime (byte setButton, byte rowButton, byte upButton, byte downButton)
{
  _rowNumber = 0;   // Set row to 0

  //
  // Check if setButton is pressed, if true debounce and go into set mode.
  //

  if (digitalRead(setButton))
  {
    delay(10);
    if (digitalRead(setButton))
    {
      delay(1000);
      setBoolean = 1;
    }

    if (setBoolean)
    {
      for (byte i = 0; i < ROW_NUM; i++)    // Flash row that is being set on/off
      {
        matrix.setPixelColor(i + (ROW_NUM * _rowNumber), 0, 0, 0);
        delay(1000);
        bitTime(timeArray[_rowNumber], bitLength[_rowNumber], timeRow[_rowNumber]);
        delay(1000);
      }

      //
      // Check if rowChange button is pressed, if true debounce and add one to _rowNumber.
      //

      if (digitalRead(rowButton))
      {
        delay(10);
        if (digitalRead(rowButton))
          _rowNumber++;
      }

      //
      // Check if up button is pressed, if true debounce and add time.
      //

      if (digitalRead(upButton))
      {
        delay(10);
        if (digitalRead(upButton))
          adjustTime(timeChange[_rowNumber]);
      }

      //
      // Check if down button is pressed, if true debounce and subtract time.
      //

      if (digitalRead(downButton))
      {
        delay(10);
        if (digitalRead(downButton))
          adjustTime(0 - timeChange[_rowNumber]);
      }

      if (digitalRead(setButton))
      {
        delay(10);
        if (digitalRead(setButton))
          setBoolean = 0;
      }
    }
  }
}

