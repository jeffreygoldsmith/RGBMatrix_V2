#include <Adafruit_BMP085_U.h>
#include <Time.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Functions.cpp>

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
/*
 * Name   : Jeffrey Goldsmith
 * Teacher: Mr. D'Arcy
 * Date   : 20 January 2016
 * Course : TEI3M
 *
 * Description: LED RGB 1 line serial in binary clock matrix
 */

//
// Bit length of time measurements.
//

static const byte YEAR = 7;
static const byte MONTH = 4;
static const byte DAY = 5;
static const byte WEEKDAY = 3;
static const byte HOUR = 4;
static const byte MINUTE = 6;
static const byte SECOND = 6;

//
// Row number of time measurements.
//

static const byte YEAR_ROW = 1;
static const byte MONTH_ROW = 2;
static const byte DAY_ROW = 3;
static const byte WEEKDAY_ROW = 4;
static const byte HOUR_ROW = 5;
static const byte MINUTE_ROW = 6;
static const byte SECOND_ROW = 7;

static const byte CENTURY = 2000;          // Century
static const byte LED_NUM = 64;            // Number of LEDs in matrix
static const byte DIN_PIN = 11;            // Pin used for data in
static const byte LED_BRIGHTNESS = 127;    // Brightness of LEDs
static const byte NUM_BUTTONS = 6;         // Number of buttons
static const byte NUM_ELEMENTS = 24;       // Number of barometer samples

//
// Array to hold barometer samples for past 24 hours
//

int sample[NUM_ELEMENTS];

boolean bitBoolean;
int pSecond = 0;

Adafruit_NeoPixel matrix = Adafruit_NeoPixel(LED_NUM, DIN_PIN, NEO_GRB);

void setup()
{
  for (byte i = 0; i < NUM_BUTTONS; i++)
    pinMode(13 - i, INPUT); 
  
  setTime(0, 0, 0, 13, 1, 2016);   //! Manually set time

  bmp.begin();
  Serial.begin(9600);
  matrix.begin();
  matrix.setBrightness(50);  // Set brightness of LEDs
  matrix.show();  // Display data on matrix

  sensor_t sensor;
  bmp.getSensor(&sensor);
}

void loop()
{  
  sensors_event_t event;    // Create new sensor event
  bmp.getEvent(&event);
  Serial.print(event.pressure); //!
  delay(500); //!
  
  //
  // Display time for second, minute, hour, etc.
  //

  Decode(now());    // Get current time
  
  if (second() - pSecond == 1)    // Check if 1 second has passed
  {
    bitTime(tm.y % CENTURY, YEAR, YEAR_ROW);
    bitTime(tm.mon, MONTH, MONTH_ROW);
    bitTime(tm.d, DAY, DAY_ROW);
    bitTime(tm.wd, WEEKDAY, WEEKDAY_ROW);
    bitTime(tm.h, HOUR, HOUR_ROW);
    bitTime(tm.m, MINUTE, MINUTE_ROW);
    bitTime(tm.s + 1, SECOND, SECOND_ROW);
  }
}
