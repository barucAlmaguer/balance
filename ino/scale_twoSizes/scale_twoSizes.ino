//SELECCIONAR TIPO DE BASCULA
#define HIGH_PRECISION  0
#define HIGH_RANGE      1
//SELECCIONAR TIPO DE DISPLAY
#define NO_DISPLAY    0
#define DISPLAY_OLED  1
#define DISPLAY_LCD   2
//MODIFICAR PARA REPROGRAMAR ARDUINOS -----------------------------------------------------------------
#define TIPO_BASCULA  HIGH_PRECISION //Opciones: HIGH_PRECISION / HIGH_RANGE (Poco peso / Mucho peso) -
#define TIPO_DISPLAY  DISPLAY_OLED  //Opciones: NO_DISPLAY, DISPLAY_OLED, DISPLAY_LCD                 -
#define SCALE_ID  1   //Opciones: 1, 2 (1ra y 2da bascula)                                            -
#define DEBUGGING_WEIGHT          // Imprime peso Raw tambien (Actualmente no es opcional)            -
//-----------------------------------------------------------------------------------------------------

//Calibraciones para diferentes basculas
#if SCALE_ID == 1
  #define CAL_SCALE_SMALL   0.00511548624
  #define CAL_SCALE_BIG     -0.045602141998
  #define CAL_TARA_SMALL    -46282.92765
  #define CAL_TARA_BIG      374593.464
  #define SCALE_DAT_PIN     3
  #define SCALE_CLK_PIN     2
#elif SCALE_ID == 2
  #define CAL_SCALE_SMALL   0.004989478
  #define CAL_SCALE_BIG     -0.045602141998
  #define CAL_TARA_SMALL    -46282.92765
  #define CAL_TARA_BIG      374593.464

  #define SCALE_DAT_PIN     A2
  #define SCALE_CLK_PIN     A0
#endif

#if TIPO_BASCULA == HIGH_PRECISION //Bascula chica
  #define CALIBRATION_M   CAL_SCALE_SMALL    
  #define CALIBRATION_B   CAL_TARA_SMALL     
  char scale_size = 'c';                     
#else //Bascula grande                       
  #define CALIBRATION_M   CAL_SCALE_BIG      
  #define CALIBRATION_B   CAL_TARA_BIG       
  char scale_size = 'g';                     
#endif                                       

//Librerias
#include "hx711.h"
#include <Deadband.h>
#if TIPO_DISPLAY == DISPLAY_OLED
  #include <Wire.h>
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>
#elif TIPO_DISPLAY == DISPLAY_LCD
  #include <Wire.h> 
  #include <LiquidCrystal_I2C.h>
#endif

#define TARA_TIME 10000 //milisegundos entre cada posible tara

#if TIPO_DISPLAY == DISPLAY_OLED
  #define OLED_RESET 4
  Adafruit_SSD1306 display(OLED_RESET);
#elif TIPO_DISPLAY == DISPLAY_LCD
  LiquidCrystal_I2C lcd(0x3F,16, 2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
#endif

//Los pines estan cruzados en las 2 versiones de basculas
#if TIPO_BASCULA == HIGH_PRECISION 
  #define SCALE_RES_GRAMS 5
  Hx711 scale(SCALE_DAT_PIN, SCALE_CLK_PIN); //Pinout disruptivo, puede cambiar en nuevas versiones
#else
  #define SCALE_RES_GRAMS 50
  Hx711 scale(2, 3); //Pinout openScale, no cambia
#endif
DeadbandFilter *filter = NULL;
byte times = 1;
int tara;
long last_tara = 0;
boolean sending_weight = true; //indica si el peso se enviara o no por serial

void setup()
{
  Serial.begin(9600);
  #if TIPO_BASCULA == HIGH_PRECISION 
  int widthOfDeadbandInBits = 1;
  #else
  int widthOfDeadbandInBits = 4;
  #endif
  filter = createDeadbandFilter(widthOfDeadbandInBits);
  scale.setReadTimes(1);

  #if TIPO_DISPLAY == DISPLAY_OLED
    //Pantalla
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("Scale");
    display.println("Calibrating...");
  #elif TIPO_DISPLAY == DISPLAY_LCD
    lcd.init();                      // initialize the lcd 
    // Print a message to the LCD.
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Scale");
    lcd.setCursor(0, 1);
    lcd.print("calibrating...");
  #endif
  //calibracion para 20kg
  scale.setScale(CALIBRATION_M);
  scale.setOffset(CALIBRATION_B);
  tara = scale.getGram();
  last_tara = millis();
  #if TIPO_DISPLAY == DISPLAY_OLED
  display.display();
  #endif
  delay(250);

  #if TIPO_DISPLAY == DISPLAY_OLED
    display.setTextSize(2);
    display.setTextColor(WHITE);
  #elif TIPO_DISPLAY == DISPLAY_LCD
    lcd.clear();
  #endif
}

void loop()
{
  long actual_time = millis();
  long elapsed = millis() - last_tara;
  //Logica de auto-tarificacion
  if (elapsed >= TARA_TIME){
    last_tara = actual_time;
    int grams = scale.getGram() - tara;
    int possible_tara = grams + tara;
    //Aplica Tara si subió o bajó un poco
    #if TIPO_BASCULA == HIGH_PRECISION 
      if (grams > 0 && grams < 5 || grams < 0 && grams > -5){
        tara = possible_tara;
      }
    #endif
  }
  int grams = scale.getGram();
  int grams_tara = grams - tara; //gramos con tara
  int grams_db = deadband(filter, grams_tara); //gramos con filtro 
  
  int printable_grams = grams_db - (grams_db % SCALE_RES_GRAMS);
  float kg = (float)printable_grams / 1000.0;

  //Envia peso
  if(sending_weight){
    #ifdef DEBUGGING_WEIGHT
      Serial.print(scale.getValue());
      Serial.print(" ");
    #endif
    Serial.print(kg, 3);
    Serial.print(" kg ");
    Serial.println(scale_size);
  }

  #if TIPO_DISPLAY == DISPLAY_OLED
    display.clearDisplay();
    display.setCursor(0,7);
    display.print(kg, 3);
    display.print(" kg");
    display.display();
  #elif TIPO_DISPLAY == DISPLAY_LCD
    if((int)kg >= 10 || kg < 0.0f)
      lcd.setCursor(3,0);
    else{
      lcd.setCursor(9,0);
      lcd.print(" ");
      lcd.setCursor(3,0);
      lcd.print(" ");
    }
    lcd.print(kg, 3);
    lcd.setCursor(10,0);
    lcd.print("kg");
  #endif

  //Ejecuta comandos de serial
  while (Serial.available()){
    char cmd = (char)Serial.read();
    switch(cmd){
      case 't':
      case 'T':
        tara = scale.getGram();
        last_tara = millis();
        break;
      case 's':
      case 'S':
        sending_weight = false;
        break;
      case 'r':
      case 'R':
        sending_weight = true;
        break;
      case 'w':
      case 'W':
        Serial.println(scale_size);
        break;
    }
  }
  delay(100);
}

