#ifndef RGB_MATRIX_H
#define RGB_MATRIX_H

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

 
class Display   // Matrix control class
{
  public:
    Display(byte dataIn, byte Brightness);
    void DisplayTime();   // Display time subclass

  private:
    byte pSecond;
};


class Time    // Time control class
{
  public:  
    Time() {}

    void init();
    void set_time(tm tm);
    void ChangeTime(byte setButton, byte rowButton, byte upButton, byte downButton); // Time adjustment subclass
    void adjust_time(byte row, int n);

  private:
    byte rowNumber;
};


class Barometer   // Barometer control class
{
  public:
    Barometer();
    void BarometerRead(); // Barometer input subclass
};

#endif // RGB_MATRIX_H

