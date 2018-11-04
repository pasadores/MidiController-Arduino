#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

int pulsadores[6] = {2,3,4,5,6,7};

byte const TAP_TEMPO     = 0;
byte const MIDI_STANDARD = 1;
byte const MIDI_LATCHING = 2; //Pensar bien si lo necesitamos
byte const SWITCH_MODE   = 3;
byte const BANK_UP       = 4;
byte const BANK_DOWN     = 5;

int modos[6] = {MIDI_STANDARD,MIDI_STANDARD,MIDI_LATCHING,SWITCH_MODE,BANK_UP,BANK_DOWN};

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
int sensorValue[6] = {0,0,0,0,0,0}, lastSensorValue[6]= {0,0,0,0,0,0};
long time[6];
int bpm;
char buffer[4];
LiquidCrystal_I2C lcd(0x3F,20,4);

void initializeScreen(){
  lcd.init();
  lcd.backlight();
  lcd.print("[    PRESET # 1    ]");
  lcd.setCursor(10, 1);
  lcd.print("Tempo:");
  lcd.createChar(0,note);
  lcd.createChar(1,clear);
}

void setup() {
  initializeScreen();
  pinMode(2,INPUT_PULLUP);
  Serial.begin(38400);
}

void calculateTap(int pulsador){
  long diff = 0;
  if(time[pulsador] != 0){
    diff = millis() - time[pulsador];
    if(diff > 7000){
      time[pulsador] = millis();
      return;
    }
    time[pulsador] = millis();
  }
  else{
    time[pulsador] = millis();
  }
  bpm = (60 / (diff/1000.0));
}

void MIDImessage(int command, int MIDInote, int MIDIvelocity) {
  Serial.write(command);//send note on or note off command
  Serial.write(MIDInote);//send pitch data
  Serial.write(MIDIvelocity);//send velocity data
}

void sendMidiCC(){
  if(sensorValue == LOW && sensorValue != lastSensorValue){
    MIDImessage(0xB0,7,127); //Load wich msg to send
  }
}

void execute(int pulsador){
  if(sensorValue[pulsador] == LOW){
    switch(modos[pulsador]){
      case TAP_TEMPO:
        if(sensorValue[pulsador] != lastSensorValue[pulsador]){
          calculateTap(pulsador);
          Serial.print("TAP TEMPO");
        }
        break;
      case MIDI_STANDARD:
        if(sensorValue[pulsador] != lastSensorValue[pulsador]){
          sendMidiCC(); //TODO, BUSCAR LOS VALORES QUE TIENE QUE ENVIAR.
          Serial.print("MIDI STANDARD");
        }
        break;
      case MIDI_LATCHING:
        if(sensorValue[pulsador] != lastSensorValue[pulsador]){
          //Kinda useless
        }
        break;
      case SWITCH_MODE: break; // TODO
      case BANK_UP: break;     // TODO
      case BANK_DOWN: break;   // TODO
    }

  }
}

void screenRender(){
  if(bpm != 0){
    lcd.setCursor(16, 1);
    sprintf(buffer, "%03db", bpm);
    lcd.print(buffer);
  }
}
void readButtons(){
  for (int i = 0; i < 6; i++){
    sensorValue[i] = digitalRead(pulsadores[i]);
    lastSensorValue[i] = sensorValue[i];
    execute(i);
  }
}
void loop() {
  readButtons();
  screenRender();
  delay(100);
}
