#include "interface.hpp"

#define SET_BIT(X, Y)  X |= (1 << Y)
#define RESET_BIT(X, Y)   X &= ~(1 << Y)
#define TOGGLE_BIT(X, Y)  X ^= (1 << Y)
#define READ_PIN(X) ((PIND & (1 << X))!=0)

int dernierA = LOW;
unsigned long vitesse1, vitesse2;

volatile uint8_t boutonState;
volatile uint8_t pos = 0;




uint8_t getEncoder(void) {
  return pos;
}

void innitStates(void){
  SET_BIT(boutonState, 4);
  SET_BIT(boutonState, 5);
}

void updateEncoder(void) {
  if((READ_PIN(pinA)) != LOW){  
    if(digitalRead(pinB) != HIGH){
      pos--;
    }else {
      pos++;
    }
  }
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
  if (SW3 > 100 && ((boutonState & (1<<4)) == 0)) {// Front montant
    TOGGLE_BIT(boutonState, 2);
    RESET_BIT(boutonState, 0);
    if (millis() - vitesse1 < 400) {
      SET_BIT(boutonState, 6);
    }
    vitesse1 = millis();
  }else if (SW3 < 100 && (boutonState & (1<<4))) {// Front DESCENDANT
    SET_BIT(boutonState, 0);
  }


  int SW2 = READ_PIN(2);
  //bouton SW2
  if (SW2 && ((boutonState & (1<<5)) == 0)) {// Front montant
    TOGGLE_BIT(boutonState, 3);
    RESET_BIT(boutonState, 1);
    if (millis() - vitesse2 < 400) {
      SET_BIT(boutonState, 7);
    }
    vitesse2 = millis();
  }else if (!SW2 && (boutonState & (1<<5))) {// Front DESCENDANT
    SET_BIT(boutonState, 1);
  }
  
  SW3 > 100 ? SET_BIT(boutonState, 4) : RESET_BIT(boutonState, 4);
  SW2 == HIGH ? SET_BIT(boutonState, 5) : RESET_BIT(boutonState, 5);
}

bool bouton(uint8_t bouton, uint8_t select){
  if (bouton & (1<<int(bouton+select))){
    return true;
  }
  return false;
}

uint8_t brut(){
  return boutonState;
}
void resetEvents(){
  RESET_BIT(boutonState, 0);
  RESET_BIT(boutonState, 1);
  RESET_BIT(boutonState, 6);
  RESET_BIT(boutonState, 7);
}


