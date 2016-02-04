
class Matrix
{
  public:
  Matrix(byte dataIn, byte ledNum, byte sideNum);
  void start();
  void show();
}

#include <Arduino.h>
#include <Time.h>
#include <Adafruit_NeoPixel.h>

static const byte ROW_OFFSET = 8;          // Amount of LEDs per row
static const byte LED_NUM = 64;            // Number of LEDs in matrix
static const byte DIN_PIN = 11;            // Pin used for data in

int pSecond = 0;
boolean bitBoolean;

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

//
// Using time as input set corresponding LEDs high.
//

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
