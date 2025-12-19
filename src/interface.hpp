#ifndef INTERFACE_H
#define INTERFACE_H

#include <SPI.h>
#include "type.h"

const static uint8_t  pinA = 3;
const static uint8_t  pinB = 4;
const static uint8_t  BUZZER = 10;

extern bool play;

enum Methode{
  ETAT = 0,
  SELECTION = 2,
  DOUBLE = 6
};

void updateEncoder(void);

void initStates(void);

void updateBouton(void);

uint8_t getEncoder(void);

bool bouton(uint8_t bouton, uint8_t select);

void alerte();

void resetEvents(void);

#endif