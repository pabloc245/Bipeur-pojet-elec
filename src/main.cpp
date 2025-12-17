#include "notification.hpp"
#include "ecran.hpp"
#include "radio.hpp"
#include "stockage.hpp"

const static uint8_t PIN_RADIO_CE = 7;
const static uint8_t PIN_RADIO_CSN = 8;
Etats stt = IDLE;

/// Setup 
void setup() {
  attachInterrupt(digitalPinToInterrupt(pinA), updateEncoder, RISING);
  Serial.begin(9600);
  SPI.begin();

  innitDisplay();
  initRadio();
  initStates();

  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(A6, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
  digitalWrite(BUZZER, LOW);
  clearEeprom();
  loadCounter();
  delay(1000);
}

/// Loop 
void loop() {

  updateBouton();
  //Serial.println(getEncoder());

  //readMessage(&messageBuffer, 0);
  //Radio();
  switch(stt){
    case IDLE:
      stt = menu();
      break;
    case CLAVIER:
      stt = clavier();
      break;
    case CONTACT:
      stt = contact();
      break;
    case MESSAGE:
      stt = messages(false);
      break;
    case PARAMETRE:
      stt = parametre();
      break;
    default:
      break;
  }
  afficher();
  resetEvents();  
}

