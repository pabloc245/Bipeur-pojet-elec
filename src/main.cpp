#include "notification.hpp"
#include "ecran.hpp"
#include "interface.hpp"
#include <Arduino.h>
int  i;
bool selected = false;
 
Buffer buffer_notifications = {0, 32, {}};

Etats stt = IDLE;
volatile uint8_t ctn = 0;

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
  Serial.println(getEncoder());
  updateButton();


  if(digitalRead(pinA))
  Serial.println("yvgbuhk");
  switch(stt){
    case IDLE:
      stt = menu();
      break;
    case CLAVIER:
      clavier();
      break;
    case DISPLAY_NOTIF:
      selected = !selected;
      break;
    case BUZZER_ON:
      break;
    default:
      break;
  }
  resetEvents();  
}