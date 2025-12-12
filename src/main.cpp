#include "notification.hpp"
#include "ecran.hpp"
#include "radio.hpp"
#include "stockage.hpp"

const static uint8_t PIN_RADIO_CE = 7;
const static uint8_t PIN_RADIO_CSN = 8;
Etats stt = IDLE;

/// Setup 
void setup() {
  attachInterrupt(digitalPinToInterrupt(pinA), updateEncoder, CHANGE);
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
  delay(1000);
  for(uint8_t i = 0; i <10; i++){
    Contact tmp ;
    strcpy(tmp.pseudo, "user1");
    tmp.adresse = (uint32_t)i;
    newContact(&tmp) ;
  }
}

/// Loop 
void loop() {
  testBoutons();

  // updateBouton();
  // //envoyerMessage(&messageBuffer, LOW_P);
  
  // //Radio();
  // switch(stt){
  //   case IDLE:
  //     stt = menu();
  //     break;
  //   case CLAVIER:
  //     stt = clavier();
  //     break;
  //   case CONTACT:
  //     stt = contact();
  //     break;
  //   case MESSAGE:
  //     stt = messages();
  //     break;
  //   case PARAMETRE:
  //     stt = parametre();
  //     break;
  //   default:
  //     break;
  // }
  
  // afficher();
  // resetEvents();  
}

