#include "notification.hpp"
#include "ecran.hpp"
#include "radio.hpp"
#include "stockage.hpp"

const static uint8_t PIN_RADIO_CE = 7;
const static uint8_t PIN_RADIO_CSN = 8;
Etats stt = IDLE;
Parametre param = {"use12", 0, 0};
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
  noTone(BUZZER);

  clearEeprom();
  loadCounter();

  // messageBuffer.adresse = 0xFFFFFFF;
  // writeMessage(&messageBuffer);
  // writeMessage(&messageBuffer);
  // writeMessage(&messageBuffer);
  // messageBuffer.adresse = 0xFFFFF;
  // writeMessage(&messageBuffer);
  strcpy(messageBuffer.pseudo, "user");
  strcpy(messageBuffer.message, "testttttttttt");
  //envoyerMessage(&messageBuffer, LOW_P);
}

static unsigned long timer =0;
/// Loop 
void loop() {
  updateBouton();
  //writeMessage(&messageBuffer);
  //readMessage(&messageBuffer, 0);
  switch(stt){
    case IDLE:
      stt = menu();
      break;
    case CLAVIER_M:
      stt = clavier(CLAVIER_M);
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
    case CLAVIER_P:
      stt = clavier(CLAVIER_P);
    default:
      break;
  }

  if(millis() - timer > 1000 || getpack()>0){
    lireMessage(&messageBuffer);
    timer = millis();
  }
  alerte();
  afficher();
  resetEvents();  
}

