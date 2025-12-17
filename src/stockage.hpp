#ifndef STOCKAGE_H
#define STOCKAGE_H

#include <EEPROM.h>
#include "type.h"
#include <Arduino.h>
#include "radio.hpp"

#define COUNTER_ADDRESS (EEPROM.length() - sizeof(int))

#define START_MESSAGE   0
#define MAX_MESSAGES    20
#define END_MESSAGE     (START_MESSAGE + MAX_MESSAGES*sizeof(Message))

#define MAX_CONTACT     10
#define START_CONTACT   (END_MESSAGE + 1)
#define END_CONTACT     (START_CONTACT + sizeof(Contact) * MAX_CONTACT)

#define START_PARAMETRE (END_CONTACT + 1)
#define END_PARAMETRE   (START_PARAMETRE + 9)

void clearEeprom();
uint8_t writeMessage(Message *const msg);
void readMessage(Message *msg, uint8_t offset);
void lireContacts(Contact *contact, const uint8_t offset);
uint8_t newContact(Contact *const contact);
uint8_t filtreMessage(uint8_t *liste, uint32_t adresse);
void clearEeprom();
void loadCounter();

extern int nbMessage;
extern int nbContact;

#endif