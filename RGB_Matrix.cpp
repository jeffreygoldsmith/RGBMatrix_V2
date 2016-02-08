#include "Arduino.h"
#include "RGB_Matrix.h"
#include <Time.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_BMP085_U.h>

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
static const byte DIN_PIN = 13;
static const byte CENTURY = 2000;          // Century
static const byte NUM_ELEMENTS = 24;       // Number of barometer samples

static const byte row[] = {7, 6, 5, 4, 3, 2, 1};   // Row number of time measurements
static const byte bitLength[] = {7, 4, 5, 3, 4, 6, 6};   // Bit length of time measurements

int timeArray[] = {tm.y, tm.mon, tm.wd, tm.d, tm.h, tm.m, tm.s};
unsigned int timeChange[] = {31557600, 2592000, 604800, 86400, 3600, 60, 1};
int sample[NUM_ELEMENTS];   // Array to hold barometer samples for past 24 hours

bool bitBool;
bool setBoolean;
bool setPrev;
bool rowPrev;
bool upPrev;
bool downPrev;
byte pSecond = 0;

Adafruit_NeoPixel matrix = Adafruit_NeoPixel(DIN_PIN, LED_NUM, NEO_GRB);
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

bool debounce (bool input, bool pInput)
{
  if (input != pInput)
  {
    if (input)
    {
      return true;
    }
  }
  delay(50);
}

void bitTime(int t, byte tLength, byte row)
{
  for (int i = 0; i < tLength; i++)
  {
    bitBool = bitRead(t, i); // Check each bit in t to be high or low

    if (bitBool)  // If bit is high set LED to be high, else set low
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





Display::Display (byte dataIn, byte brightness)
{
  pinMode(dataIn, OUTPUT);
  matrix.begin();
  matrix.setBrightness(brightness);
  matrix.show();
}

void Display::DisplayTime ()
{
  if (second() - pSecond == 1)    // Check if 1 second has passed
  {
    Decode(now());

    bitTime(tm.y % CENTURY, bitLength[6], row[6]);
    bitTime(tm.mon, bitLength[5], row[5]);
    bitTime(tm.d, bitLength[4], row[4]);
    bitTime(tm.wd, bitLength[3], row[3]);
    bitTime(tm.h, bitLength[2], row[2]);
    bitTime(tm.m, bitLength[1], row[1]);
    bitTime(tm.s + 1, bitLength[0], row[0]);
  }
}





Time::Time ()
{
  setTime(0, 0, 0, 2, 11, 1999);    // Initally set time to be 2 November 1999
}

void Time::ChangeTime (byte setButton, byte rowButton, byte upButton, byte downButton)
{
  rowNumber = 0;   // Set row to 0

  //
  // Check if setButton is pressed, go into set mode.
  //

  if (debounce(digitalRead(setButton), setPrev))
  {
    setBoolean = 1;
    setPrev = digitalRead(setButton);
  }

  if (setBoolean)
  {
    for (byte i = 0; i < ROW_NUM; i++)    // Flash row that is being set on/off
    {
      matrix.setPixelColor(i + (ROW_NUM * rowNumber), 0, 0, 0);
      delay(1000);
      bitTime(timeArray[rowNumber], bitLength[rowNumber], row[rowNumber]);
      delay(1000);
    }

    //
    // If rowButton is pressed, advance 1 row.
    //

    if (debounce(digitalRead(rowButton), rowPrev))
    {
      rowNumber++;
      rowPrev = digitalRead(rowButton);
    }

    //
    // Check if up button is pressed, add time.
    //

    if (debounce(digitalRead(upButton), upPrev))
    {
      adjustTime(timeChange[rowNumber]);
      upPrev = digitalRead(upButton);
    }

    //
    // Check if down button is pressed, subtract time.
    //

    if (debounce(digitalRead(downButton), downPrev))
    {
      adjustTime(0 - timeChange[rowNumber]);
      downPrev = digitalRead(downButton);
    }

    if (debounce(digitalRead(setButton), setPrev))
    {
      setBoolean = 0;
      setPrev = digitalRead(setButton);
    }
  }
}




Barometer::Barometer ()
{
  bmp.begin();
  sensor_t sensor;
  bmp.getSensor(&sensor);
  sensors_event_t barometer;    // Create new sensor event
  bmp.getEvent(&barometer);
}

void Barometer::BarometerRead ()
{
  
}




