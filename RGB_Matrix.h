#include "Arduino.h"
#ifndef RGB_MATRIX_H
#define RGB_MATRIX_H

class Display
{
  public:
  Display(byte dataIn, byte Brightness);
  void DisplayTime();

  private:
  byte pSecond;
};

class Time
{
  public:
  Time();
  void ChangeTime(byte setButton, byte rowButton, byte upButton, byte downButton);

  private:
  byte rowNumber;
};

class Barometer
{
  public:
  Barometer();
  void BarometerRead();
};

#endif
