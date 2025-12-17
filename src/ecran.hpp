#ifndef ECRAN_H
#define ECRAN_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "type.h"
#include "radio.hpp"
#include "stockage.hpp"
#include "interface.hpp"

const static uint8_t  SCREEN_WIDTH = 128; 
const static uint8_t  SCREEN_HEIGHT = 64;


void affichage();
Etats menu();
void innitDisplay();
Etats clavier();
void afficherNotifications(Buffer *buffer);
Etats parametre();
Etats messages(bool first);
void afficher();
Etats contact();



#endif