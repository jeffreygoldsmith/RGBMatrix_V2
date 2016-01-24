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
    bitTime(tm.y % CENTURY, YEAR, YEAR_ROW);
    bitTime(tm.mon, MONTH, MONTH_ROW);
    bitTime(tm.d, DAY, DAY_ROW);
    bitTime(tm.wd, WEEKDAY, WEEKDAY_ROW);
    bitTime(tm.h, HOUR, HOUR_ROW);
    bitTime(tm.m, MINUTE, MINUTE_ROW);
    bitTime(tm.s + 1, SECOND, SECOND_ROW);
  }
  Decode(now());
  Serial.print(tm.s);
  Serial.println();
  delay(1000);
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

int Encode(int years, int months, int d, int hours, int minutes, int seconds)
{
  int dy;

  static int days[] = // Days prior to start of month (origin-1)
  {
    0, -1, 30, 58, 89, 119, 150, 180, 211, 242, 272, 303, 333
  };

  dy = years - 1968; // Normalize year to leap year preceding epoch start
  dy = (dy - 2) * 365 + (dy >> 2); // Multiply by 365.25, giving days in years
  d -= !(years & 0x03) && months <= 2; // Reduce day if prior to leap point in leap year

  return seconds + 60 * (minutes + 60 * (hours + 24 * (dy + days[months] + d)));
}

struct tm Decode(time_t ts)
{
  int d, m, y;
//!  int ts;

//!  static time_t     lastts;
//!  static tm         tm;

//!  if (tm == lastts) // Check if value to convert is same as last
//!    return tm; // Return last result if so

//!  ts = lastts = tm; // Time value to decode

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
