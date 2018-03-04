#include "hx711.h"
#include <Deadband.h>
#define laser 5
#define sp(x) Serial.print(x)
#define spln(x) Serial.println(x)
#define SCALE1_DAT_PIN     A1
#define SCALE1_CLK_PIN     A0
#define SCALE2_DAT_PIN     A4
#define SCALE2_CLK_PIN     A5
#define CAL_M1  1.633714642
#define CAL_B1  -13276900.25
#define CAL_M2  -0.791499924
#define CAL_B2  6568464.743
Hx711 scale1(SCALE1_DAT_PIN, SCALE1_CLK_PIN); //3KG
Hx711 scale2(SCALE2_DAT_PIN, SCALE2_CLK_PIN); //1KG
DeadbandFilter *filter = NULL;

void setup(){
    Serial.begin(250000);
    pinMode(5, OUTPUT);
    pinMode(SCALE1_DAT_PIN, INPUT);
    pinMode(SCALE1_CLK_PIN, OUTPUT);
    pinMode(SCALE2_DAT_PIN, INPUT);
    pinMode(SCALE2_CLK_PIN, OUTPUT);
    int widthOfDeadbandInBits = 1;
    filter = createDeadbandFilter(widthOfDeadbandInBits);
    scale1.setReadTimes(1);
    scale1.setScale(CAL_M1);
    scale1.setOffset(CAL_B1);
    scale2.setReadTimes(1);
    scale2.setScale(CAL_M2);
    scale2.setOffset(CAL_B2);
}
void loop(){
  float g1 = scale1.getGram()/1000.0;
  float g2 = scale2.getGram()/1000.0;
  sp("p1="); sp(g1); sp(" ");
  sp("p2="); sp(g2); sp(" ");
  sp("d="); sp(g2-g1); spln("");
  delay(100);
}
