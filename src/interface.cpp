#include "interface.hpp"

double delaie=0, delai2=0;
int dernierA;



void encoder(){
  Serial.println("Encoder");  
  if(digitalRead(pinA) != dernierA){  
    if(digitalRead(pinB)==HIGH){
      //compteur+=60;
    }else {
      //compteur-=60;
    }
    delai2=millis();
  }  
  dernierA = LOW;
}

  //int pin[3]={5,6,9};
  
  // if(millis() - delaie > 500){
  //   analogWrite(pin[i%3], 0);
  //   delaie = millis();
  //   i++;
  //   analogWrite(pin[i%3], 255);
  // }
