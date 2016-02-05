#include "Arduino.h"

class Matrix
{
  public:
  Matrix(byte dataIn, byte ledNum, byte Brightness);
};

class Time
{
  public:
  Time();
  void ChangeTime(byte setButton, byte rowButton, byte upButton, byte downButton);

  private:
  byte _rowNumber;
  unsigned int _pSecond;
};

int main()
{
  
}
