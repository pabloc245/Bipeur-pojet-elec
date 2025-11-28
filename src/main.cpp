//#include <Arduino.h>
#include "notification.hpp"
#include "ecran.hpp"
#include "interface.hpp"

int  i;
bool selected = false;
 
Buffer buffer_notifications = {0, 32, {}};

/// Setup function
void setup() {
  Serial.begin(9600);
  SPI.begin();

  innitDisplay();
  innitRadio();

  attachInterrupt(digitalPinToInterrupt(pinA), encoder, RISING);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(A6, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
  delay(2000);
}



/// Loop function
void loop() {


  if(analogRead(A6)==0){
    Serial.println("Bouton");   
    selected = !selected; 
    delay(500);
  }
  if(selected){
    tone(BUZZER, 5, 100);
  }
  affichage();
}




