#ifndef STOCKAGE_H
#define STOCKAGE_H

#include <EEPROM.h>
#include "type.h"
#include "radio.hpp"

#define MESSAGE_COUNTER_ADDRESS (EEPROM.length() - sizeof(uint8_t))
#define CONATACT_COUNTER_ADDRESS (EEPROM.length() - sizeof(uint8_t)*2)

#define START_MESSAGE   0
#define MAX_MESSAGES    20
#define END_MESSAGE     (START_MESSAGE + MAX_MESSAGES*sizeof(Message))

#define MAX_CONTACT     10
#define START_CONTACT   (END_MESSAGE + 1)
#define END_CONTACT     (START_CONTACT + sizeof(Contact) * MAX_CONTACT)

#define START_PARAMETRE (END_CONTACT + 1)
#define END_PARAMETRE   (START_PARAMETRE + sizeof(Parametre))


void clearEeprom();
uint8_t writeMessage(Message *const msg);
void readMessage(Message *msg, uint8_t offset);
void lireContacts(Contact *contact, const uint8_t offset);
uint8_t newContact(Contact *const contact);
uint8_t filtreMessage(uint8_t *liste, uint32_t adresse);
void clearEeprom();
void loadCounter();

extern uint8_t nbMessage;
extern uint8_t nbContact;

#endif