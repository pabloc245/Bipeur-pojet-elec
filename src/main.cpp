//#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <NRFLite.h>


const static uint8_t  pinA = 3;
const static uint8_t  pinB = 4;
const static uint8_t  SCREEN_WIDTH = 128; 
const static uint8_t  SCREEN_HEIGHT = 64; 
const static uint8_t  BUZZER = 10;

const static uint8_t RADIO_ID = 0;
const static uint8_t DESTINATION_RADIO_ID = 1; // The radio ID of the destination radio.
const static uint8_t PIN_RADIO_CE = 7;
const static uint8_t PIN_RADIO_CSN = 8;
const static uint8_t PIN_RADIO_MOSI = 11;
const static uint8_t PIN_RADIO_MISO = 12;
const static uint8_t PIN_RADIO_SCK = 13;

int dernierA, i;
double delaie=0, delai2=0;
volatile int compteur = 0;
bool selected = false;

#define DIRECTION_CW 0   // clockwise direction
#define DIRECTION_CCW 1  // counter-clockwise direction


struct __attribute__((packed)) RadioPacket // Note the packed attribute.
{
    uint8_t FromRadioId;
    uint32_t OnTimeMillis;
    uint32_t FailedTxCount;
};

NRFLite _radio; 
RadioPacket _radioData;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
uint8_t _data;

void testRF();  
void affichage();  
void encoder();

/// Setup function
void setup() {
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }else{
    Serial.println(F("SSD1306 allocation successful"));
  }

  SPI.begin();

  if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN)){
    Serial.println("Cannot communicate with radio");
    while (1); // Wait here forever.
  }else{
    Serial.println(F("NRF24XX allocation successful"));
  }

  _radioData.FromRadioId = RADIO_ID;

  attachInterrupt(digitalPinToInterrupt(pinA), encoder, RISING);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(A6, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
  dernierA = digitalRead(pinA);
  delay(2000);
}

/// Loop function
void loop() {
  //testRF();
  int pin[3]={5,6,9};
  
  if(millis() - delaie > 500){
    digitalWrite(pin[i%3], LOW);
    delaie = millis();
    i++;
    digitalWrite(pin[i%3], HIGH);
  }


  if(analogRead(A6)==0){
    Serial.println("Bouton");   
    selected = !selected; 
    delay(500);
  }
  if(selected){
    tone(BUZZER, compteur*5, 100);
  }
  affichage();
}




void testRF(){
  
  #if ENV_MODE==1 // Server mode
  _radioData.OnTimeMillis = millis();

  Serial.print("Sending ");
  Serial.print(_radioData.OnTimeMillis);
  Serial.print(" ms");
  if (_radio.send(0, &_radioData, sizeof(_radioData))){ // Note how '&' must be placed in front of the variable name.
    Serial.println("...Success");
  }  else{
    Serial.println("...Failed");
    _radioData.FailedTxCount++;
  }

  delay(1000);
  #elif ENV_MODE==0 // Client mode
  while (_radio.hasData()){
    _radio.readData(&_radioData); // Note how '&' must be placed in front of the variable name.

    String msg = "Radio ";
    msg += _radioData.FromRadioId;
    msg += ", ";
    msg += _radioData.OnTimeMillis;
    msg += " ms, ";
    msg += _radioData.FailedTxCount;
    msg += " Failed TX";

    Serial.println(msg);
  }

 
  #endif
}

void affichage(){  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(SCREEN_WIDTH/2 - 55, 33);
  display.fillRoundRect(5, 5, compteur * ((SCREEN_WIDTH-5)/1023.0), 25, 1, WHITE);
  display.drawRect(5, 5, SCREEN_WIDTH-5, 25, WHITE);
  display.print("frequence: ");
  display.print(compteur*5);
  display.print("Hz\n");
  display.setCursor(SCREEN_WIDTH/2 - 25, 45);
  display.invertDisplay(selected);
  display.print(selected ? "ACTIVE" : "DESACTIVE");
  display.display();
}

void encoder(){
  
  if(digitalRead(pinA) != dernierA){  
    if(digitalRead(pinB)==HIGH){
      compteur+=60;
    }else {
      compteur-=60;
    }
    delai2=millis();
  }
  
  dernierA = LOW;
}
