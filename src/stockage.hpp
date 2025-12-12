#ifndef STOCKAGE_H
#define STOCKAGE_H

#include <EEPROM.h>
#include "type.h"
#include <Arduino.h>
#include "radio.hpp"

#define START_MESSAGE   0
#define MAX_MESSAGES    25
#define END_MESSAGE     (START_MESSAGE + MAX_MESSAGES*37)

#define MAX_CONTACT     10
#define START_CONTACT   (END_MESSAGE + 1)
#define END_CONTACT     (START_CONTACT + 11 * MAX_CONTACT)

#define START_PARAMETRE (END_CONTACT + 1)
#define END_PARAMETRE   (START_PARAMETRE + 9)

void clearEeprom();
uint8_t writeMessage(const Message *msg);
void readMessage(Message *msg, uint8_t offset);
void lireContacts(Contact *contact, const uint8_t offset);
uint8_t newContact(const Contact *contact);
uint8_t filtreMessage(uint8_t *liste, char *pseudo);
extern int nbMessage;
extern int nbContact;

#endif