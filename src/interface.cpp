#include "interface.hpp"

#define SET_BIT(X, Y)  X |= (1 << Y)
#define RESET_BIT(X, Y)   X &= ~(1 << Y)
#define TOGGLE_BIT(X, Y)  X ^= (1 << Y)
#define READ_PIN(X) ((PIND & (1 << X))!=0)

unsigned long vitesse1, vitesse2;

typedef struct {
    volatile int position;
    volatile uint8_t bouton;
}Encoder;

volatile Encoder encoderData = {0, false};
volatile int  pos = 0;
int dernierA = LOW;


void innitStates(void){
  encoderData.position = 0;
  SET_BIT(encoderData.bouton, 4);
  SET_BIT(encoderData.bouton, 5);
}

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

//B1: bouton SW3 (analogique)
//B2: potentiometre SW2 (digital)

//VAR BOUTON:  
// bit0: state B1, 
// bit1: state B2, 
// bit2: selection B1, 
// bit3: selection B2, 
// bit4: lastState B1, 
// bit5: lastState B2,
// bit6: double click B1,
// bit7: double click B2


void updateButton(){
  int SW3 = analogRead(A6);

  //bouton SW3
  if (SW3 > 100 && ((encoderData.bouton & (1<<4)) == 0)) {// Front montant
    TOGGLE_BIT(encoderData.bouton, 2);
    RESET_BIT(encoderData.bouton, 0);
    if (millis() - vitesse1 < 400) {
      SET_BIT(encoderData.bouton, 6);
    }
    vitesse1 = millis();
  }else if (SW3 < 100 && (encoderData.bouton & (1<<4))) {// Front DESCENDANT
    SET_BIT(encoderData.bouton, 0);
  }


  int SW2 = READ_PIN(2);
  //bouton SW2
  if (SW2 && ((encoderData.bouton & (1<<5)) == 0)) {// Front montant
    TOGGLE_BIT(encoderData.bouton, 3);
    RESET_BIT(encoderData.bouton, 1);
    if (millis() - vitesse2 < 400) {
      SET_BIT(encoderData.bouton, 7);
    }
    vitesse2 = millis();
  }else if (!SW2 && (encoderData.bouton & (1<<5))) {// Front DESCENDANT
    SET_BIT(encoderData.bouton, 1);
  }
  
  SW3 > 100 ? SET_BIT(encoderData.bouton, 4) : RESET_BIT(encoderData.bouton, 4);
  SW2 == HIGH ? SET_BIT(encoderData.bouton, 5) : RESET_BIT(encoderData.bouton, 5);
}

bool bouton(uint8_t bouton, uint8_t select){
  if (encoderData.bouton & (1<<int(bouton+select))){
    return true;
  }
  return false;
}

uint8_t brut(){
  return encoderData.bouton;
}
void resetEvents(){
  RESET_BIT(encoderData.bouton, 0);
  RESET_BIT(encoderData.bouton, 1);
  RESET_BIT(encoderData.bouton, 6);
  RESET_BIT(encoderData.bouton, 7);
}
