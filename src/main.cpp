#include "notification.hpp"
#include "ecran.hpp"
#include "interface.hpp"
#include "radio.hpp"

Buffer buffer_notifications = {0, 4, {}};
const static uint8_t PIN_RADIO_CE = 7;
const static uint8_t PIN_RADIO_CSN = 8;
Etats stt = IDLE;

/// Setup 
void setup() {
  attachInterrupt(digitalPinToInterrupt(pinA), updateEncoder, CHANGE);
  Serial.begin(9600);
  SPI.begin();

  innitDisplay();
  innitRadio();
  innitStates();

  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(A6, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
  digitalWrite(BUZZER, LOW);
  delay(1000);
}


/// Loop 
void loop() {
  updateButton();
  switch(stt){
    case IDLE:
      stt = menu();
      break;
    case CLAVIER:
      stt = clavier();
      break;
    case DISPLAY_NOTIF:
      stt = messages();
      break;
    case PARAMETRE:
      stt = parametre();
      break;
    default:
      break;
  }
  if(bouton(1, ETAT)){
    Notification notif = {
      0,
      576,
      " tes "
    };
    add(&buffer_notifications, notif);
  }
  Buffer* pBuff = &buffer_notifications;
  afficherNotifications(pBuff);
  afficher();
  resetEvents();  
}