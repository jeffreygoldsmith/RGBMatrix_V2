//
// Include libraries.
//

#include "Arduino.h"
#include "RGB_Matrix.h"
#include <Time.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_Sensor.h>
#include <assert.h>

struct tm tm;

static const byte DIN_PIN = 6;             // Data in pin
static const byte LED_NUM = 64;            // Number of LEDs on RGB matrix
static const byte ROW_OFFSET = 8;          // Amount of LEDs per row on matrix
static const byte ROW_NUM = 7;             // Number of rows used on matrix
static const byte CENTURY = 2000;          // Century
static const byte NUM_ELEMENTS = 23;       // Number of barometer samples

static const byte row[] = { 8, 7, 6, 5, 4, 3, 2 };   // Row number of time measurements
static const byte bitLength[] = { 7, 4, 5, 3, 4, 6, 8 };   // Bit length of time measurements

int *timeArray[] = { &tm.y, &tm.mon, &tm.d, &tm.wd, &tm.h, &tm.m, &tm.s };
int timeMin[] = { 1970, 1, 1, -1, 0, 0, 0 };
int timeMax[] = { 2038, 12, 31, -1, 23, 59, 59 };
//!int timeChange[] = { 31557600, 2592000, 604800, 86400, 3600, 60, 1 };

bool buttonState[] = { false, false, false, false, false, false };  // Array to store button states
bool changeRow[] = { false, false, false, false, false, false, false }; // Array to store which row is being set

bool setBool;                               // Button state booleans
bool setCheckPrev;
bool rowCheckPrev;
bool upCheckPrev;
bool downCheckPrev;

int sample[NUM_ELEMENTS];   // Array to hold barometer samples for past 24 hours

//!bool bitBool;               // Boolean to detect whether byte(x) is high or low
time_t baseTime;            // Base time in seconds since 1 January 1970 (set by user)
unsigned long baseElapsedTime;  // Millisecond count when time set
//!byte secondPrev = 0;        // Lagging value of seconds
byte hourPrev = 0;          // Lagging value of hours

Adafruit_NeoPixel matrix = Adafruit_NeoPixel(LED_NUM, DIN_PIN, NEO_GRB); // Create new NeoPixel object
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085); // Create new barometer object


//
// Function to debounce button input.
//
bool debounce(byte input, bool i)
{
  bool button = digitalRead(input);

  if (button != buttonState[i])
  {
    delay(5);
    buttonState[i] = digitalRead(input);
  }
}


//
// Function to take time and display value on matrix.
//
void bitTime(int t, byte tLength, byte row, bool set)
{
  byte red = set ? 0 : 10;
  byte green = set ? 10 : 0;

  for (int i = 0; i < tLength; i++)
  {
    bool bitBool = bitRead(t, i); // Check each bit in t to be high or low
    matrix.setPixelColor(ROW_OFFSET * row - 1 - i, red * bitBool, green * bitBool, 0); // If bit is high set LED to be high, else set low
    matrix.show();
  }
}


//
// Encode() -- Convert timestamp to seconds since 1 January 1970
//
time_t Encode(int years, int months, int days, int hours, int mins, int secs)
{
      int         dy;
 
      assert(years >= timeMin[0] && years <= timeMax[0]);
      assert(months >= timeMin[1] && months <= timeMax[1]);
      assert(days >= timeMin[2] && days <= timeMax[2]);
      assert(hours >= timeMin[4] && hours <= timeMax[4]);
      assert(mins >= timeMin[5] && mins <= timeMax[5]);
      assert(secs >= timeMin[6] && secs <= timeMax[6]);
 
      static int DAYS[] = // Days prior to start of month (origin-1)
      {
            0, -1, 30, 58, 89, 119, 150, 180, 211, 242, 272, 303, 333
      };
 
      dy = years - 1968; // Normalize year to leap year preceding epoch start
      dy = (dy - 2) * 365 + (dy >> 2); // Multiply by 365.25, giving days in years
      days -= !(years & 0x03) && months <= 2; // Reduce day if prior to leap point in leap year
 
      return secs + 60 * (mins + 60 * (hours + 24 * (dy + DAYS[months] + days)));
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
// Class to control RGB matrix.
//


//
// Display::Display() -- Class constructor
//
Display::Display(byte dataIn, byte brightness)
{
  pinMode(dataIn, OUTPUT); // Set data in pin to output
  matrix.begin(); // Initialize matrix
  matrix.setBrightness(1); // Set brightness
  matrix.show(); // Set all LEDs to off initially
}


//
// Function to display time on matrix.
//
void Display::DisplayTime()
{
  if (!setBool) // Skip if in set mode
  {    
//    Serial.println(millis());
//    if (millis() % 1000 == 0) // Look for seconds transition
//    {
//      baseTime++; // Increment time
      tm = Decode(baseTime + (millis() - baseElapsedTime) / 1000); // Compute and decode current time
//    }
  }
  else
    Serial.println("In set mode"); //!

  bitTime(tm.y % 100, bitLength[6], row[6], changeRow[6]); // Display time on matrix
  bitTime(tm.mon, bitLength[5], row[5], changeRow[5]);
  bitTime(tm.d, bitLength[4], row[4], changeRow[4]);
  bitTime(tm.wd, bitLength[3], row[3], changeRow[3]);
  bitTime(tm.h, bitLength[2], row[2], changeRow[2]);
  bitTime(tm.m, bitLength[1], row[1], changeRow[1]);
  bitTime(tm.s, bitLength[0], row[0], changeRow[0]);
}


//
// Class to control time.
//


//
// Time::Time() -- Class constructor //!
//
//Time::Time()
void Time::init()
{
  tm.y = 1999; // Initial time value
  tm.mon = 11;
  tm.d = 2;


  Serial.println(Decode(Encode(1999, 11, 2, 0, 0, 0)).y); //!

  
  set_time(tm);

  for (int i = 13; i > 7; i--)
    pinMode(i, INPUT);

  rowNumber = 6; // Start at seconds row
}


//
// Time::set_time() -- //!
//
void Time::set_time(struct tm tm)
{
  baseTime = Encode(tm.y, tm.mon, tm.d, tm.h, tm.m, tm.s); // Initalize time
//!  Serial.println(Decode(baseTime).y); //!
  baseElapsedTime = millis(); // Base for elapsed time calculation
}


//
// Function to control adjustments to time.
//
void Time::ChangeTime(byte setButton, byte rowButton, byte upButton, byte downButton)
{
  bool setCheck = debounce(setButton, 0); // Get button state

  if (!(setBool || setCheck)) // If not in or entering set mode,
    return; // we're done

  if (setCheckPrev == false && setCheck == true) // Check if toggling set mode
  {
    setBool = !setBool; // Toggle set mode
    Serial.println(setBool); //!
  }

  setCheckPrev = setCheck; // Update state

  if (setBool) // Now, check if in set mode
  {
    Serial.println("In setBool while"); //!
    
    bool rowCheck = debounce(rowButton, 1); // If rowButton is pressed, advance 1 row

    if (rowCheckPrev == false && rowCheck == true)
    {
      rowNumber = ++rowNumber % 7;

      if (rowNumber == 3) //!
        rowNumber++; // Skip over day of week
        
      Serial.println(rowNumber); //!
    }

    rowCheckPrev = rowCheck;

    for (byte i = 0; i < 7; i++) // Change colour of row being set to green
      changeRow[i] = i == rowNumber;

    bool upCheck = debounce(upButton, 2); // Check if up button is pressed, add time

    if (upCheckPrev == false && upCheck == true)
      adjust_time(rowNumber, 1); // Increment value
 //!     adjustTime(timeChange[rowNumber]);

    upCheckPrev = upCheck;

    bool downCheck = debounce(downButton, 3); // Check if down button is pressed, subtract time

    if (downCheckPrev == false && downCheck == true)
      adjust_time(rowNumber, -1); // Decrement value
 //!     adjustTime(0 - timeChange[rowNumber]);

    downCheckPrev = downCheck;
  }
  else // Exiting set mode
    set_time(tm); // Set requested time
}


//
// Time::adjust_time() -- //!
//
void Time::adjust_time(byte row, int n)
{
  *timeArray[row] += n; // Adjust value in appropriate direction

  if (n > 0 && *timeArray[row] >= timeMax[row]) // Check for overflow
    *timeArray[row] = 0; // Set to minimum

  if (n < 0 && *timeArray[row] <= 0) // Check for underflow
    *timeArray[row] = timeMax[row] - 1; // Set to maximum
}


//
// Class to control barometer.
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
// CBarometer::BarometerRead() -- Store input from barometer
//
void Barometer::BarometerRead()
{
  sensors_event_t barometer; // Create new sensor event
  bmp.getEvent(&barometer);

  if (tm.h - hourPrev == 1) // If one hour has passed...
  {
    for (byte i = 1; i < NUM_ELEMENTS; i++) // Shift all elements down and take sample
      sample[i] = sample[i - 1];

    sample[0] = barometer.pressure;
  }
}
