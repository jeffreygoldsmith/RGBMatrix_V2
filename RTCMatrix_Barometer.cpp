//
// Include libraries.
//

#include "Arduino.h"
#include "RTCMatrix_Barometer.h"
#include <Time.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

struct tm tm;

static const byte DIN_PIN = 6;             // Data in pin
static const byte LED_NUM = 64;            // Number of LEDs on RGB matrix
static const byte ROW_OFFSET = 8;          // Amount of LEDs per row on matrix
static const byte ROW_NUM = 7;             // Number of rows used on matrix
static const byte HOUR_AVERAGE = 12.5;     // Average of numbers 1 to 24
static const byte NUM_ELEMENTS = 24;       // Amount of elements in array of barometer samples

static const byte row[] = { 8, 7, 6, 4, 3, 2, 1 };         // Row number of time measurements
static const byte bitLength[] = { 6, 6, 5, 3, 5, 4, 8 };   // Bit length of time measurements

static const float xBar[] = { -11.5, -10.5, -9.5, -8.5, -7.5, -6.5, -5.5, -4.5,
                              -3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5, 4.5,
                              5.5, 6.5, 7.5, 8.5, 9.5, 10.5, 11.5
                            };
float yBar[24];

float sample[NUM_ELEMENTS]; // Barometer samples
float yAverage;               // Sum of barometer samples
float xyProduct;
float xSq;
float slope;

int hourPrev;               // Lagging value of hours
unsigned long unixPrev;     // Lagging value of unix time

Adafruit_NeoPixel matrix = Adafruit_NeoPixel(LED_NUM, DIN_PIN, NEO_GRB); // Create new NeoPixel object
RTC_DS1307 rtc; // Create new RTC object
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085); // Create new barometer sensor object

//
// Function to take time and display value on matrix.
//
void bitTime(int t, byte tLength, byte row)
{
  for (int i = 0; i < tLength; i++)
  {
    bool bitBool = bitRead(t, i); // Check each bit in t to be high or low
    matrix.setPixelColor(ROW_OFFSET * row - 1 - i, 30 * bitBool, 0, 0); // If bit is high set LED to be high, else set low
  }
  matrix.show();
}


//
// Function to take time_t variable and decode into individual measurements of time.
//
struct tm Decode(time_t ts)
{
  int d, m, y;
  struct tm tm;

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


//
// Class to control time and RTC.
//

//
// Time::Time() -- Class constructor
//
Time::Time()
{
  rtc.begin(); // Initialize RTC
}


//
// Function to initialize RTC.
//
void Time::Sync()
{
  rtc.adjust(DateTime(2016, 2, 15, 2, 25, 30)); // Set time to system time
  DateTime now = rtc.now(); // Take reading from RTC and update current time
  unixPrev = now.unixtime(); // Set lagging value of unix time to be initial unix time
}


//
// Time::UpdateTime() -- Update time based on readings from RTC.
//
void Time::UpdateTime()
{
  Serial.begin(9600);
  DateTime now = rtc.now(); // Take reading from RTC and update current time

  if (now.unixtime() - unixPrev == 1) // Check for second transition
  {
    Serial.println(now.unixtime());
    tm = Decode(now.unixtime()); // Compute and decode current time
    unixPrev = now.unixtime(); // Set lagging value of unix time
    Serial.println(tm.h);
  }
}


//
// Class to control RGB matrix.
//


//
// Display::Display() -- Class constructor
//
Display::Display(byte dataIn, byte brightness)
{
  pinMode(dataIn, OUTPUT); // Set data in pin to output
  matrix.begin(); // Initialize matrix
  matrix.setBrightness(brightness); // Set brightness
  matrix.show(); // Set all LEDs to off initially
}


//
// Display::DisplayTime() -- Display time on matrix.
//
void Display::DisplayTime()
{
  if (tm.h <= 12)
  {
    for (byte i = 0; i < 8; i++)
      matrix.setPixelColor(ROW_OFFSET * 5 - i - 1, 0, 30, 0);
  }else{
    for (byte i = 0; i < 8; i++)
      matrix.setPixelColor(ROW_OFFSET * 5 - i - 1, 0, 0, 30);
  }
  bitTime(tm.y % 100, bitLength[6], row[6]); // Display time on matrix
  bitTime(tm.mon, bitLength[5], row[5]);
  bitTime(tm.d, bitLength[4], row[4]);
  bitTime(tm.wd, bitLength[3], row[3]);
  bitTime(tm.h % 12, bitLength[2], row[2]);
  bitTime(tm.m, bitLength[1], row[1]);
  bitTime(tm.s, bitLength[0], row[0]);
}


//
// Class to store samples from barometer and process samples to display barometer row.
//


//
// Barometer::Barometer() -- Class constructor
//
Barometer::Barometer()
{
  bmp.begin(); // Initialize barometer
  sensor_t sensor;
  bmp.getSensor(&sensor);
}


//
// Barometer::BarometerRead() -- Store input from barometer
//
void Barometer::BarometerRead()
{
  sensors_event_t barometer; // Get barometer information
  bmp.getEvent(&barometer);

  if (tm.h - hourPrev == 1) // Check for hour transition
  {
    int sumSample = 0;

    for (byte i = 0; i < NUM_ELEMENTS - 1; i++) // Shift all elements down and take sample
      sample[i] = sample[i + 1];

    sample[NUM_ELEMENTS - 1] = barometer.pressure; // Add new sample to array

    for (byte i = 0; i < NUM_ELEMENTS; i++)
      yAverage += sample[i]; // Add values of sample to create sum of sample

    yAverage /= NUM_ELEMENTS; // divide sum of sample by number of elements

    for (byte i = 0; i < NUM_ELEMENTS; i++)
      yBar[i] = sample[i] - yAverage; // Subtract y by y average

    for (byte i = 0; i < NUM_ELEMENTS; i++)
    {
      xyProduct += yBar[i] * xBar[i];
      xSq += xBar[i] * xBar[i];
    }

    slope = xyProduct / xSq; // Calculate slope

    hourPrev = tm.h; // Increment lagging value of hour
  }
}


//
// Barometer::BarometerDisplay() -- Display barometer reading on matrix.
//
void Barometer::BarometerDisplay()
{
  for (byte i = 0; i < ROW_OFFSET; i++)
  {

  }
}


