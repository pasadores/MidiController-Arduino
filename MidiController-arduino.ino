#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

int pulsadores[6] = {2,3,4,5,6,7};

byte const TAP_TEMPO     = 0;
byte const MIDI_CC       = 1;
byte const MIDI_LATCHING = 2; //Pensar bien si lo necesitamos
byte const MIDI_PC       = 3;
byte const SWITCH_MODE   = 4;

int modos[6] = {TAP_TEMPO,MIDI_CC,MIDI_CC,MIDI_CC,MIDI_CC,MIDI_CC};

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
int sensorValue[6] = {HIGH,HIGH,HIGH,HIGH,HIGH,HIGH},
lastSensorValue[6] = {HIGH,HIGH,HIGH,HIGH,HIGH,HIGH};
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
  pinMode(3,INPUT_PULLUP);
  pinMode(4,INPUT_PULLUP);
  pinMode(5,INPUT_PULLUP);
  pinMode(6,INPUT_PULLUP);
  pinMode(7,INPUT_PULLUP);
  
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

void sendMidiCC(int command){  
   MIDImessage(0xB0,command,127); //Load wich msg to send  
}

void execute(int pulsador){
  if(sensorValue[pulsador] == LOW){
    //Serial.write("Entrada");
    switch(modos[pulsador]){
      case TAP_TEMPO:
        if(sensorValue[pulsador] != lastSensorValue[pulsador]){
          calculateTap(pulsador);
          sendMidiCC(20);
          //Serial.write("TAP TEMPO\n");
      }
      break;
      case MIDI_CC:
        if(sensorValue[pulsador] != lastSensorValue[pulsador]){
          sendMidiCC(16); //TODO, BUSCAR LOS VALORES QUE TIENE QUE ENVIAR.
          //Serial.print("MIDI STANDARD");
        }
        break;
      case MIDI_LATCHING:
        if(sensorValue[pulsador] != lastSensorValue[pulsador]){/*Kinda Useless*/}break;
      case SWITCH_MODE: break; // TODO
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
    execute(i);
    lastSensorValue[i] = sensorValue[i];
  }
}
void loop() {
  readButtons();
  screenRender();
  delay(100);
}
