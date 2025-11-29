#include "interface.hpp"

#define SET_BIT(X, Y)  X |= (1 << Y)
#define RESET_BIT(X, Y)   X &= ~(1 << Y)
#define TOGGLE_BIT(X, Y)  X ^= (1 << Y)
#define READ_PIN(X) PIND & (1 << X)

unsigned long vitesse1, vitesse2;;

typedef struct {
    volatile int position;
    volatile uint8_t bouton;
    volatile bool lastState;
}Encoder;

volatile Encoder encoderData = {0, false, LOW};
volatile int  pos = 0;
int dernierA = LOW;

int getEncoder(void) {
  return encoderData.position;
}

void updateEncoder(void) {
    
  if((READ_PIN(pinA)) != dernierA){  
    if(digitalRead(pinB) != HIGH){
      encoderData.position--;
    }else {
      encoderData.position++;
    }
  }
  dernierA = LOW;  
}

void updateButton(){
  int SW2 = READ_PIN(2);
  int SW3 = analogRead(A6);

  if(encoderData.bouton>>1 & 1){
    TOGGLE_BIT(encoderData.bouton, 3);
  }
  if(encoderData.bouton>>0 & 1){
    TOGGLE_BIT(encoderData.bouton, 2);
  }

  if (SW3==0 && (millis() - vitesse1 > 500)){
    SET_BIT(encoderData.bouton, 0);
    vitesse1 = millis();
  }else{
    RESET_BIT(encoderData.bouton, 0);
  }

  if (SW2==LOW && (millis() - vitesse2 > 500)){
    SET_BIT(encoderData.bouton, 1); 
    vitesse2 = millis();
  }else{
    RESET_BIT(encoderData.bouton, 1);
  }
}

uint8_t bouton(){
  if(encoderData.bouton & (1<<0)){
    Serial.println(F("Bouton appuye"));
  }
  if (encoderData.bouton & (1<<1)){
    Serial.println(F("Bouton potentiometere selection"));
  }
  if(encoderData.bouton & (1<<2)){
    Serial.println(F("Selection"));
  }
  if(encoderData.bouton & (1<<3)){
    Serial.println(F("selection potentiometere"));
  }
  return encoderData.bouton;
}

  //int pin[3]={5,6,9};
  
  // if(millis() - delaie > 500){
  //   analogWrite(pin[i%3], 0);
  //   delaie = millis();
  //   i++;
  //   analogWrite(pin[i%3], 255);
  // }
