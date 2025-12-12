#include "stockage.hpp"

int nbMessage = 0;
int nbContact = 0;
bool existsInEEPROM(const Contact* msg);

uint8_t writeMessage(const Message *msg) {
    nbMessage ++; 
    int address = ((nbMessage - 1)% MAX_MESSAGES )* sizeof(Message);
    EEPROM.put(address, *msg);
    return 1;
}

void readMessage(Message *msg, const uint8_t offset) {
    if(offset > nbMessage){
        Serial.println(F("OUT of bond"));
        return;
    }
    int address =  ((int(nbMessage) - int(offset))%MAX_MESSAGES) * sizeof(Message);
    EEPROM.get(address, *msg);
}

uint8_t filtreMessage(uint8_t *liste, char *pseudo){
    uint8_t nb = 0;
    for(uint8_t i =0; i >nbMessage; i ++){
        readMessage(&messageBuffer, i);
        if((strcmp(messageBuffer.pseudo, pseudo) == 0)){
            liste[nb]=i;
            nb ++;
        }
    }
    return nb;
}

uint8_t newContact(const Contact *contact) {
    if(nbContact + 1 > MAX_CONTACT){
        Serial.println(F("Plus de place pour les contact"));
        return 1;
    }
    if(existsInEEPROM(contact)){
        Serial.println(F("Contact déjà enregistrer"));
        return 1;
    }
    int address = START_CONTACT + nbContact * sizeof(Contact);
    EEPROM.put(address, *contact);
    nbContact ++; 
    return 0;
}

bool existsInEEPROM(const Contact* msg) {
    Contact tmp;
    for (uint8_t i = 0; i < nbContact; i++) {
        int addr = START_CONTACT + i * sizeof(Contact);
        EEPROM.get(addr, tmp);
        if (tmp.adresse == msg->adresse) {
            return true; 
        }
    }
    return false; 
}

void lireContacts(Contact *contact, const uint8_t offset) {
    int address = START_CONTACT + offset * sizeof(*contact);
    EEPROM.get(address, *contact);
}

void clearEeprom(){
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
}