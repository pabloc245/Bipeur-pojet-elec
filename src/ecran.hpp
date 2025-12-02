#ifndef ECRAN_H
#define ECRAN_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const static uint8_t  SCREEN_WIDTH = 128; 
const static uint8_t  SCREEN_HEIGHT = 64;

enum Etats {
  IDLE = -1,
  CLAVIER = 0,
  DISPLAY_NOTIF = 1,
  BUZZER_ON
};


void affichage();
Etats menu();
void innitDisplay();
void clavier();

#endif