#ifndef RTCMATRIX_BAROMETER_H
#define RTCMATRIX_BAROMETER_H

#include "Arduino.h"


struct tm
{
  int y;    // Year
  int mon;  // Month
  int d;    // Day
  int wd;   // Weekday
  int h;    // Hour
  int m;    // Minute
  int s;    // Second
};

class Time
{
  public:
    Time();
    void Sync();
    void UpdateTime();
};

class Display  
{
  public:
    Display(byte dataIn, byte Brightness);
    void DisplayTime();  
};

class Barometer
{
  public:
    Barometer();
    void BarometerRead();
    void BarometerDisplay();
};


#endif // RTCMATRIX_BAROMETER_H
