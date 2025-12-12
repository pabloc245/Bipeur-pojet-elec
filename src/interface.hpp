#ifndef INTERFACE_H
#define INTERFACE_H

#include <Arduino.h>

const static uint8_t  pinA = 3;
const static uint8_t  pinB = 4;
const static uint8_t  BUZZER = 10;

enum Methode{
  ETAT = 0,
  SELECTION = 2,
  DOUBLE = 6
};

/** 
 * @brief  Fonction d'interruption pour lire les pins de l'enocdeur 
*/
void updateEncoder(void);

/** 
 * @brief  Initialise les variable de intetrface.cpp
*/
void initStates(void);

/** 
 * @brief  Lit les pins des boutons et update les états
*/
void updateBouton(void);

/** 
 * @brief  Retourne le nombre de fois que l'on tourne l'encoder
*/
uint8_t getEncoder(void);

/** 
 * @brief  Test toutes les fonctions de interface.cpp
*/
void testBoutons() ;

/** 
 * @brief  permet de savoir dans quel etat son les boutons
 * @param  bouton  0 = gros bouton  1 = bouton potentionmètre
 * @param  select  ETAT = ON ou OFF, SELECTION = flip à chaque foi que l'on appuie dessus, DOUBLE = double clique
 * @note retourne vraie ou faux
*/
bool bouton(uint8_t bouton, uint8_t select);


void resetEvents(void);
uint8_t brut();
#endif