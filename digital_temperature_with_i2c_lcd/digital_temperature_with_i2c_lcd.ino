#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define TEMP_PIN A1

LiquidCrystal_I2C lcd(0x20, 8, 2); // set the LCD address to 0x20 for a 16 chars and 2 line display


byte dosi[8] =  {B11100,
                 B10100,
                 B11100,
                 B00000,
                 B00000,
                 B00000,
                 B00000,
                 B00000};

double thermister(int RawADC) {
  double Temp = 0;

  Temp = log(((10240000/RawADC) - 10000));
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
  Temp = Temp - 273.15;            // Convert Kelvin to Celcius
// Temp = (Temp * 9.0)/ 5.0 + 32.0; // Convert Celcius to Fahrenheit

 return (int)Temp;
}

int get_current_temperature() {
  int temp = 0;

  temp = analogRead(TEMP_PIN);

  return thermister(temp);
}

void setup() {
  Serial.begin(9600);
  lcd.init();               // initialize the lcd 
  lcd.createChar(0, dosi);  // create user defined char
  lcd.backlight();
}

void loop() {
  int currTemp = get_current_temperature();

  Serial.println("1");
  lcd.setCursor(0, 0);
  Serial.println("2");
  lcd.print("Temp: ");
  Serial.println(currTemp, DEC);
  lcd.print(currTemp, DEC);
  Serial.println("4");
  lcd.setCursor(0, 1);
  Serial.println("5");
  int i = lcd.write((byte)0);
  Serial.println("6");
  lcd.print("Celcius");
  Serial.println("7");

  Serial.println(i);
  delay(1000);
}

