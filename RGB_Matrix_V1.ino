#include <Time.h>
#include <Adafruit_NeoPixel.h>

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

static const int YEAR = 7;
static const int MONTH = 4;
static const int DAY = 5;
static const int WEEKDAY = 3;
static const int HOUR = 4;
static const int MINUTE = 6;
static const int SECOND = 6;

//
// Row number of time measurements.
//

static const int YEAR_ROW = 1;
static const int MONTH_ROW = 2;
static const int DAY_ROW = 3;
static const int WEEKDAY_ROW = 4;
static const int HOUR_ROW = 5;
static const int MINUTE_ROW = 6;
static const int SECOND_ROW = 7;

static const int CENTURY = 2000;          // Century
static const int LED_NUM = 64;            // Number of LEDs in matrix
static const int DIN_PIN = 11;            // Pin used for data in
static const int ROW_OFFSET = 8;          // Amount of LEDs per row
static const int LED_BRIGHTNESS = 127;    // Brightness of LEDs

boolean bitBoolean;
int pSecond = 0;

Adafruit_NeoPixel matrix = Adafruit_NeoPixel(LED_NUM, DIN_PIN, NEO_GRB);

void setup()
{
  setTime(0, 0, 0, 13, 1, 2016);   //! Manually set time

  Serial.begin(9600);
  matrix.begin();
  matrix.setBrightness(50);  // Set brightness of LEDs
  matrix.show();  // Display data on matrix
}

void loop()
{
  //
  // Display time for second, minute, hour, etc.
  //

  if (second() - pSecond == 1)    // Check if 1 second has passed
  {
    bitTime(year() % CENTURY, YEAR, YEAR_ROW);
    bitTime(month(), MONTH, MONTH_ROW);
    bitTime(day(), DAY, DAY_ROW);
    bitTime(weekday(), WEEKDAY, WEEKDAY_ROW);
    bitTime(hour(), HOUR, HOUR_ROW);
    bitTime(minute(), MINUTE, MINUTE_ROW);
    bitTime(second() - 1, SECOND, SECOND_ROW);

    pSecond = second();   // Set lagging value for seconds

    if (pSecond == 59)
      pSecond = 0;

    Serial.println(second());
  }
}



//
// Using time as input set corresponding LEDs high.
//

void bitTime(int t, int tLength, int row)
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
