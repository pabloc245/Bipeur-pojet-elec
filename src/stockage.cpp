#include "stockage.hpp"

uint8_t nbMessage = 0;
uint8_t nbContact = 0;
bool existsInEEPROM(const Contact* msg);

void saveCounter() {
    EEPROM.put(MESSAGE_COUNTER_ADDRESS, nbMessage);
    EEPROM.put(CONATACT_COUNTER_ADDRESS, nbContact);
}

void loadCounter() {
    EEPROM.get(START_PARAMETRE, param);
    Serial.println(F("Destockake de param"));
    Serial.print(F("Pseudo: "));
    Serial.println(param.pseudo);
    Serial.print(F("canal: "));
    Serial.println(param.canal);
    EEPROM.get(MESSAGE_COUNTER_ADDRESS, nbMessage);
    if((unsigned int)nbMessage == 0xFFFF) nbMessage = 0; 
    EEPROM.get(CONATACT_COUNTER_ADDRESS, nbContact);
    if((unsigned int)nbContact == 0xFFFF) nbContact = 0; 
}

uint8_t writeMessage(Message *const msg) {
    int address = nbMessage * sizeof(Message);
    Contact newcont;
    newcont.adresse = msg->adresse;
    strcpy(newcont.pseudo, msg->pseudo);
    newContact(&newcont);
    EEPROM.put(address, *msg);
    nbMessage += nbMessage >= MAX_MESSAGES - 1 ? -(MAX_MESSAGES - 1) : 1;
    saveCounter();
    return 1;
}

void readMessage(Message *msg, const uint8_t offset) {
    if(offset > nbMessage){
        Serial.println(F("OUT of bond"));
        return;
    }
    int address =  ((int(nbMessage) - 1 - int(offset))%MAX_MESSAGES) * sizeof(Message);
    EEPROM.get(address, *msg);
}

uint8_t filtreMessage(uint8_t *liste, uint32_t adresse){
    uint8_t nb = 0;
    for(uint8_t i = 0; i < nbMessage; i ++){
        readMessage(&messageBuffer, i);
        Serial.print(F("compare "));
        Serial.print(messageBuffer.adresse, HEX);
        Serial.print(" et ");
        Serial.println(adresse, HEX);
        if(messageBuffer.adresse == adresse){
            Serial.print(i);
            Serial.println(F(" messages trouvés"));
            liste[nb]=i;
            nb ++;
        }
    }
    return nb;
}

uint8_t newContact(Contact *const contact) {
    if(nbContact + 1 > MAX_CONTACT){
        Serial.println(F("Plus de place pour les contact"));
        return 1;
    }
    if(existsInEEPROM(contact)){
        Serial.println(F("Contact déjà enregistrer"));
        return 1;
    }
    int address = START_CONTACT + nbContact * sizeof(Contact);

    Serial.print(F("New Contact: "));
    Serial.print(contact->pseudo);
    Serial.print(F(" addr: "));
    Serial.print(contact->adresse, HEX);
    Serial.print(F(" à l'adresse "));
    Serial.println(address);

    EEPROM.put(address, *contact);
    nbContact += nbContact >= MAX_CONTACT - 1 ? -(MAX_CONTACT - 1) : 1;
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
    Serial.print(F("offset demandé: "));
    Serial.println(offset);
    
    if(offset >= nbContact) {
        Serial.println(F("ERREUR: offset >= nbContact !"));
        return;
    }
    
    int address = START_CONTACT + offset * sizeof(Contact);
    Serial.print(F("rCaddr: "));
    Serial.println(address);
    
    EEPROM.get(address, *contact);
    
    // Serial.print(F("addre: 0x"));
    // Serial.println(contact->adresse, HEX);
    // Serial.print(F("Pseudo: "));
    // Serial.println(contact->pseudo);
}


void clearEeprom(){
  for (unsigned int i = 0 ; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0);
  }
}