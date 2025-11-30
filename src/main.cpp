#include "notification.hpp"
#include "ecran.hpp"
#include "interface.hpp"

int  i;
bool selected = false;
 
Buffer buffer_notifications = {0, 32, {}};

enum Etats {
  IDLE,
  CLAVIER,
  DISPLAY_NOTIF,
  BUZZER_ON
};


Etats state = CLAVIER;

/// Setup 
void setup() {
  Serial.begin(9600);
  SPI.begin();

  innitDisplay();
  innitRadio();
  innitStates();

  attachInterrupt(digitalPinToInterrupt(pinA), updateEncoder, RISING);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(A6, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
  digitalWrite(BUZZER, LOW);
  delay(2000);
}


/// Loop 
void loop() {
  updateButton();


  switch(state){
    case IDLE:
      testRF();
      break;
    case CLAVIER:
      clavier();
      break;
    case DISPLAY_NOTIF:
      selected = !selected;
      tone(BUZZER, 1000, 200);
      break;
    case BUZZER_ON:
      digitalWrite(BUZZER, HIGH); 
      delay(500);
      break;
    default:
      break;
  }  
  
  resetEvents();
}