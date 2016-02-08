#include <Time.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "RGB_Matrix.h"

/*
   Name   : Jeffrey Goldsmith
   Teacher: Mr. D'Arcy
   Date   : 20 January 2016
   Course : TEI3M

   Description: LED RGB 1 line serial in binary clock matrix
*/

static const byte CENTURY = 2000;          // Century
static const byte DATA_IN_PIN = 13;        // Data in pin for RGB Matrix
static const byte LED_BRIGHTNESS = 30;     // Brightness of LEDs
static const byte NUM_BUTTONS = 6;         // Number of buttons

void setup()
{
  Display matrix(DATA_IN_PIN, LED_BRIGHTNESS);
  
  for (byte i = 0; i < NUM_BUTTONS; i++)
    pinMode(13 - i, INPUT);
}

void loop()
{
  Display DisplayTime();    // Display time for second, minute, hour, etc

}

