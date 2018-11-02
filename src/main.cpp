#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
byte note[8] = {
  B00100,
  B00110,
  B00101,
  B00101,
  B00100,
  B11100,
  B11100,
  B11100
};
byte clear[8] ={
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
int sensorValue = 0, lastSensorValue= 0;
long time;
int bpm;
char buffer[4];
LiquidCrystal_I2C lcd(0x3F,20,4);
void setup() {
  // initialize digital pin 13 as an output.
  lcd.init();
  lcd.backlight();
  lcd.print("[    PRESET # 1    ]");
  lcd.setCursor(10, 1);
  lcd.print("Tempo:");
  lcd.createChar(0,note);
  lcd.createChar(1,clear);
  pinMode(2,INPUT_PULLUP);
  Serial.begin(9600);
}
void calculateTap(){
  if(sensorValue == LOW && sensorValue != lastSensorValue){
    long diff = 0;
    if(time != 0){
      diff = millis() - time;
      if(diff > 7000){
        time = millis();
        return;
      }
      time = millis();
    }
    else{
      time = millis();
    }
    bpm = (60 / (diff/1000.0));
  }
}

void loop() {
  lcd.setCursor(0, 1);
  sensorValue = digitalRead(2);
  if(bpm != 0){
    lcd.setCursor(16, 1);
    sprintf(buffer, "%03db", bpm);
    lcd.print(buffer);
  }
  calculateTap();
  lastSensorValue = sensorValue;
  delay(100);
}
