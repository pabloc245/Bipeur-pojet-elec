#ifndef INTERFACE_H
#define INTERFACE_H

#include <Arduino.h>

const static uint8_t  pinA = 3;
const static uint8_t  pinB = 4;
const static uint8_t  BUZZER = 10;


void updateEncoder(void);
void innitStates(void);
void updateButton(void);
int getEncoder(void);
void updateButton();
bool bouton(uint8_t bouton, uint8_t select);
void resetEvents(void);
uint8_t brut();
#endif