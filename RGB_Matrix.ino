#include <Time.h>
#include "RGB_Matrix.h"

/*
   Name   : Jeffrey Goldsmith
   Teacher: Mr. D'Arcy
   Date   : 20 January 2016
   Course : TEI3M

   Description: LED RGB 1 line serial in binary clock matrix
*/

static const byte DATA_IN_PIN = 6;         // Data in pin for RGB Matrix
static const byte LED_BRIGHTNESS = 30;     // Brightness of LEDs
static const byte NUM_BUTTONS = 6;         // Number of buttons
static const byte SET_BUTTON = 13;         // Pin number for set button
static const byte ROW_BUTTON = 12;         // Pin number for row button
static const byte UP_BUTTON = 11;          // Pin number for up button
static const byte DOWN_BUTTON = 10;        // Pin number for down button

Time rtc;
Display rgb(DATA_IN_PIN, LED_BRIGHTNESS);

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  rgb.DisplayTime();    // Display time for second, minute, hour, etc.
  rtc.ChangeTime(SET_BUTTON, ROW_BUTTON, UP_BUTTON, DOWN_BUTTON);
}



