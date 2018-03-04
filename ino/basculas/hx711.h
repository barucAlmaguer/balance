/* Arduino library for digital weight scale of hx711
 *
 * hardware design: syyyd
 * available at http://syyyd.taobao.com
 *
 * library design: Weihong Guan (@aguegu)
 * http://aguegu.net
 *
 * library host on
 * https://github.com/aguegu/Arduino
 *
 *  Created on: Oct 31, 2012
 */

#ifndef HX711_H_
#define HX711_H_

#include "Arduino.h"

class Hx711
{
public:
  Hx711(uint8_t pin_din, uint8_t pin_slk);
  virtual ~Hx711();
  long getValue();
  long averageValue();
  void setOffset(float offset = -42449.76f);
  void setScale(float scale = 0.0050922f);
  void setReadTimes(byte times);
  float getGram();
  void calibrate(float x1, float y1, float x2, float y2);
  float getScale();
  float getOffset();

private:
  const uint8_t _pin_dout;
  const uint8_t _pin_slk;
  float _offset;
  float _scale;
  byte _read_times;
};

#endif /* HX711_H_ */
