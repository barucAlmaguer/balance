#define laser 5

void setup(){
    Serial.begin(250000);
    pinMode(5, OUTPUT);
}
void loop(){
    for (int i=0; i<10; i++){
        analogWrite(5, i);
        delay(250);
    }
    for (int i=0; i<10; i++){
        analogWrite(5, 10 - i);
        delay(250);
    }
}
