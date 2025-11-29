#ifndef ECRAN_H
#define ECRAN_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const static uint8_t  SCREEN_WIDTH = 128; 
const static uint8_t  SCREEN_HEIGHT = 64;

void affichage();
void innitDisplay();
void clavier();

#endif