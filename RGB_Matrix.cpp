#include "Arduino.h"
#include "RGB_Matrix.h"
#include <Time.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_Sensor.h>

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

static const byte ROW_NUM = 7;
static const byte ROW_OFFSET = 8;
static const byte LED_NUM = 64;
static const byte DIN_PIN = 6;
static const byte CENTURY = 2000;          // Century
static const byte NUM_ELEMENTS = 23;       // Number of barometer samples

static const byte row[] = {8, 7, 6, 5, 4, 3, 2};   // Row number of time measurements
static const byte bitLength[] = {7, 4, 5, 3, 4, 6, 8};   // Bit length of time measurements

int timeArray[] = {tm.y, tm.mon, tm.wd, tm.d, tm.h, tm.m, tm.s};
int timeChange[] = {31557600, 2592000, 604800, 86400, 3600, 60, 1};

bool buttonState [] = {0, 0, 0, 0, 0, 0};
bool changeRow [] = {0, 0, 0, 0, 0, 0, 0};
bool setBool = 0;
bool setCheck;
bool setCheckPrev;
bool rowCheck;
bool rowCheckPrev;
bool upCheck;
bool upCheckPrev;
bool downCheck;
bool downCheckPrev;

int sample[NUM_ELEMENTS];   // Array to hold barometer samples for past 24 hours

bool bitBool;
byte pSecond = 0;
byte pHour = 0;

Adafruit_NeoPixel matrix = Adafruit_NeoPixel(LED_NUM, DIN_PIN, NEO_GRB);
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

bool debounce (byte input, bool i)
{
  bool button = digitalRead(input);

  if (button != buttonState[i])
  {
    delay(5);
    button = digitalRead(input);
  }
  buttonState[i] = button;
}

void bitTime(int t, byte tLength, byte row, bool set)
{
  byte red = 10;
  byte green = 0;
  if (set)
  {
    red = 0;
    green = 10;
  }
  for (int i = 0; i < tLength; i++)
  {
    bitBool = bitRead(t, i); // Check each bit in t to be high or low

    if (bitBool)  // If bit is high set LED to be high, else set low
      matrix.setPixelColor(((ROW_OFFSET * row) - 1) - i, red, green, 0);
    else
      matrix.setPixelColor(((ROW_OFFSET * row) - 1) - i, 0, 0, 0);

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


Display::Display (byte dataIn, byte brightness)
{
  pinMode(dataIn, OUTPUT);
  matrix.begin();
  matrix.setBrightness(1);
  matrix.show();
}

void Display::DisplayTime ()
{
  if (setBool == LOW)
    Decode(now());

  bitTime(tm.y % 100, bitLength[6], row[6], changeRow[6]);
  bitTime(tm.mon, bitLength[5], row[5], changeRow[5]);
  bitTime(tm.d, bitLength[4], row[4], changeRow[4]);
  bitTime(tm.wd, bitLength[3], row[3], changeRow[3]);
  bitTime(tm.h, bitLength[2], row[2], changeRow[2]);
  bitTime(tm.m, bitLength[1], row[1], changeRow[1]);
  bitTime(tm.s + 1, bitLength[0], row[0], changeRow[0]);
}


Time::Time ()
{
  setTime(0, 0, 0, 2, 11, 1999);    // Initally set time to be 2 November 1999

  for (int i = 13; i > 7; i--)
    pinMode(i, INPUT);

  rowNumber = 6;   // Start at seconds row
}

void Time::ChangeTime (byte setButton, byte rowButton, byte upButton, byte downButton)
{
  //
  // Check if setButton is pressed, if true go into set mode.
  //
  Serial.println(setBool);
  if (setBool == 0)
  {
    setCheck = debounce(13, 0);

    if (setCheckPrev == LOW && setCheck == HIGH)
    {
      setBool = 1;
      //Serial.println(setBool);
    }

    setCheckPrev = setCheck;

    if (setBool)
    {
      for (byte i = 0; i < 7; i++)
      {
        if (i == rowNumber)
          changeRow[rowNumber] = 1;
        else
          changeRow[i] = 0;
      }

      rowCheck = debounce(rowButton, 1);     // If rowButton is pressed, advance 1 row

      if (rowCheckPrev == LOW && rowCheck == HIGH)
        rowNumber++;

      rowCheckPrev = rowCheck;


      upCheck = debounce(upButton, 2);      // Check if up button is pressed, add time

      if (upCheckPrev == LOW && upCheck == HIGH)
        adjustTime(timeChange[rowNumber]);

      upCheckPrev = upCheck;


      downCheck = debounce(downButton, 3);      // Check if down button is pressed, subtract time

      if (downCheckPrev == LOW && downCheck == HIGH)
        adjustTime(0 - timeChange[rowNumber]);

      downCheckPrev = downCheck;

      if (setBool)
      { // If setButton is pressed again, exit set mode
        setCheck = debounce(13, 0);

        if (setCheckPrev == LOW && setCheck == HIGH)
          setBool = 0;
          //Serial.println(setBool);

        setCheckPrev = setCheck;
      }
    }
  }
}



Barometer::Barometer ()
{
  bmp.begin();
  sensor_t sensor;
  bmp.getSensor(&sensor);
}

void Barometer::BarometerRead ()
{
  sensors_event_t barometer;    // Create new sensor event
  bmp.getEvent(&barometer);

  if (tm.h - pHour == 1)
  {
    for (byte i = 1; i < NUM_ELEMENTS; i++)
    {
      sample[i] = sample[i - 1];
    }
    sample[0] = barometer.pressure;

  }
}
