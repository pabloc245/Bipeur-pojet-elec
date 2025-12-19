#include "interface.hpp"

#define SET_BIT(X, Y)  X |= (1 << Y)
#define RESET_BIT(X, Y)   X &= ~(1 << Y)
#define TOGGLE_BIT(X, Y)  X ^= (1 << Y)
#define READ_PIN(X) ((PIND & (1 << X))!=0)

uint8_t boutonState;
volatile int pos = 0;
bool play = false;


//B1: bouton SW3 (analogique)
//B2: potentiometre SW2 (digital)

const uint16_t melodies[3][6] PROGMEM = {
  {670, 555, 555, 495, 555, 660},
  {392, 392, 392, 311, 555, 660}, 
  {262, 392, 349, 330, 294, 523}, 
};

const uint16_t durations[3][6] PROGMEM = {
  {80, 80, 350, 110, 110, 180},
  {250, 250, 250, 1000, 0, 0},
  {500, 500, 125, 125, 125, 500},
};

const uint8_t pauses[3][6] PROGMEM = {
  {40, 0, 5, 5, 5, 0},
  {50, 50, 50, 250, 0, 0},
  {50, 50, 30, 30, 30, 50},
}; 


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


void alerte(){
  static uint8_t count = 0;
  static unsigned long temp = 0;
  if(play && millis() - temp > 700){
    for (uint8_t i = 0; i < 6; i++) { 
      uint16_t freq = pgm_read_word(&melodies[param.alerte][i]);
      uint16_t duration = pgm_read_word(&durations[param.alerte][i]);
      uint8_t pause = pgm_read_byte(&pauses[param.alerte][i]);
      
      Serial.print(freq);
      Serial.print(duration);
      Serial.println(pause == 250 ? 500 : pause);
      tone(BUZZER, freq);  // Plus de division par 3
      delay(duration);   
      noTone(BUZZER);        
      if (pause > 0) {
        delay(pause);    
      }
    }
    temp =millis();
    count++;
  }
  if(bouton(1, ETAT) || bouton(2, ETAT) || count > 5){
    play = false;
    count = 0;
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

void resetEvents(){
  RESET_BIT(boutonState, 0);
  RESET_BIT(boutonState, 1);
  RESET_BIT(boutonState, 6);
  RESET_BIT(boutonState, 7);
}
