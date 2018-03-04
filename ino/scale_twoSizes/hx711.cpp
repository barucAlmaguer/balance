/*
 * Hx711.cpp
 *
 *  Created on: Oct 31, 2012
 *      Author: agu
 *
 *  Modified on: Sep 07, 2017
 *    Author: Baruc
 */

#include "hx711.h"

Hx711::Hx711(uint8_t pin_dout, uint8_t pin_slk) :
    _pin_dout(pin_dout), _pin_slk(pin_slk)
{
  pinMode(_pin_slk, OUTPUT);
  pinMode(_pin_dout, INPUT);

  digitalWrite(_pin_slk, HIGH);
  delayMicroseconds(100);
  digitalWrite(_pin_slk, LOW);

  setReadTimes(8);
  averageValue(); //Lecturas iniciales para estabilizar sensor
}

Hx711::~Hx711()
{

}

long Hx711::averageValue()
{
  long sum = 0;
  for (byte i = 0; i < _read_times; i++)
  {
    sum += getValue();
  }

  return sum / (long)_read_times;
}

long Hx711::getValue()
{
  byte data[3];

  while (digitalRead(_pin_dout))
    ;

  for (byte j = 3; j--;)
  {
    for (char i = 8; i--;)
    {
      digitalWrite(_pin_slk, HIGH);
      bitWrite(data[j], i, digitalRead(_pin_dout));
      digitalWrite(_pin_slk, LOW);
    }
  }

  digitalWrite(_pin_slk, HIGH);
  digitalWrite(_pin_slk, LOW);

  data[2] ^= 0x80;

  return ((uint32_t) data[2] << 16) | ((uint32_t) data[1] << 8)
      | (uint32_t) data[0];
}

void Hx711::setOffset(float offset)
{
  _offset = offset;
}

void Hx711::setScale(float scale)
{
  _scale = scale;
}

float Hx711::getGram()
{
  long val = averageValue();
  return (float) val * _scale + _offset;
}

void Hx711::setReadTimes(byte times){
  _read_times = times > 0 ? times : 1;
}

void Hx711::calibrate(float x1, float y1, float x2, float y2){
  float m = (y2-y1)/(x2-x1);
  float b = y2 - m*x2;
  setScale(m);
  setOffset(b);
}

float Hx711::getScale()
{
  return _scale;
}

float Hx711::getOffset()
{
  return _offset;
}
