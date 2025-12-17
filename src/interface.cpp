#include "interface.hpp"

#define SET_BIT(X, Y)  X |= (1 << Y)
#define RESET_BIT(X, Y)   X &= ~(1 << Y)
#define TOGGLE_BIT(X, Y)  X ^= (1 << Y)
#define READ_PIN(X) ((PIND & (1 << X))!=0)

uint8_t boutonState;
volatile int pos = 0;

//B1: bouton SW3 (analogique)
//B2: potentiometre SW2 (digital)


uint8_t getEncoder(void) {
  return pos;
}

void initStates(void){
  boutonState = 0;
  SET_BIT(boutonState, 4);
  SET_BIT(boutonState, 5);
}

void updateEncoder(void) {
  if((READ_PIN(pinA)) != LOW){  
    if(READ_PIN(pinB) != HIGH){
      pos--;
    }else {
      pos++;
    }
  }
}


//VAR BOUTON:  
// bit0: state B1, 
// bit1: state B2, 
// bit2: selection B1, 
// bit3: selection B2, 
// bit4: lastState B1, 
// bit5: lastState B2,
// bit6: double click B1,
// bit7: double click B2
void updateBouton(){
  static unsigned long vitesse1 = 0, vitesse2 = 0;
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
  return (boutonState & (1 << int(bouton+select))) != 0;
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

void testBoutons() {
  updateBouton();
  Serial.print(F("ENCODER: "));
  Serial.print(getEncoder()); 
  Serial.print(F("  |  "));

  // Bouton SW3 (bouton = 0)
  Serial.print(F("SW3: "));
  Serial.print(bouton(0, 0) ? F("PRESSED ") : F("------- "));
  Serial.print(bouton(0, 2) ? F("SELECT ") : F("------ "));
  Serial.print(bouton(0, 6) ? F("DOUBLE ") : F("------ "));
  Serial.print(F("| "));

  // Bouton SW2 (bouton = 1)
  Serial.print(F("SW2: "));
  Serial.print(bouton(1, 0) ? F("PRESSED ") : F("------- "));
  Serial.print(bouton(1, 2) ? F("SELECT " ): F("------ "));
  Serial.print(bouton(1, 6) ? F("DOUBLE ") : F("------ "));

  Serial.println();
  
  // Efface les événements pour éviter les répétitions
  resetEvents();
}