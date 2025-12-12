#include "radio.hpp"

#define VIDE 0x7A89C197
#define MSG 0x80000000

static uint32_t sync = 0x7CD215D8;
static uint32_t idle = 0x7A89C197;
const static uint8_t CE_PIN = 7;
const static uint8_t CSN_PIN = 8;
const uint8_t adressePipe[5] = {0xDE,0xAD,0xBE,0xEF,0x01};

CBCH3121 verif;

// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);

Message messageBuffer = {
    "plo",
    "butvfdjlsbuyroz"
};

Parametre param =  {"user12", 0, 0};


// For this example, we'll be using 6 addresses; 1 for each TX node
// It is very helpful to think of an address as a path instead of as
// an identifying device destination
// Notice that the last byte is the only byte that changes in the last 5
// addresses. This is a limitation of the nRF24L01 transceiver for pipes 2-5
// because they use the same first 4 MSBytes from pipe 1.

bool R = false;



void envoyerMessage(const void *message, const uint8_t priorite);
void setRole();
void lireMessage(Message *message);

void initRadio(){
    if (!radio.begin(CE_PIN, CSN_PIN)) {
        Serial.println(F("radio hardware is not responding!!"));
        for(;;);
    }
    radio.setAddressWidth(3);
    radio.setPALevel(RF24_PA_MAX);  
    radio.disableDynamicPayloads();
    radio.setPayloadSize(sizeof(uint8_t)*4);  
    radio.setAutoAck(false);
    setRole();
}


uint32_t hashPseudo(const char* pseudo) {
    uint32_t sum = 0;
    uint8_t count = 0;

    while (pseudo[count] != '\0') {
        sum += (uint8_t)pseudo[count];
        count++;
    }

    uint16_t avg = (count > 0) ? (sum / count) : 0;

    uint32_t hash = avg;
    hash = (hash << 16) ^ (DEVICE_ID * 2654435761UL);  
    hash ^= (hash >> 5);
    hash += (hash << 7);
    hash ^= (hash >> 13);

    return hash & 0x1FFFFF;  
}



void envoyerMessage(const void *message, const uint8_t priorite){
    static uint16_t erreur= 0;
    const uint8_t* data = (const uint8_t*)message;
    uint8_t i = 0;
    uint32_t addr = hashPseudo(param.pseudo);

    Serial.print("SYNC: ");
    Serial.println(sync,HEX);
    radio.write(&sync, sizeof(uint8_t)*4);
    verif.decode(sync, erreur);
    for(uint8_t j = 0; j < 16; j++){
        uint32_t payload = 0;
        uint8_t addr_pos = addr & 0b111; 
        if( ((j & 1)==0) && (addr_pos == (j >> 1))){
            Serial.println(addr,HEX);
            payload = (uint32_t)(((addr>>3) << 13) | (priorite << 11));
            verif.encode(payload);
            Serial.println(payload,HEX);
            Serial.print(j);
            Serial.print(F(" ADDR: "));
            Serial.println(((payload >> 13) & 0x3FFFF) << 3 | (j/2), HEX);
            verif.decode(payload, erreur);
            radio.write(&addr, sizeof(uint32_t));
        }else{
            if(j & 1) { 
                payload = ((uint32_t)(data[i] & 0x0F) << 26) | ((uint32_t)data[i+1] << 18) | ((uint32_t)data[i+2] << 10);  
                i+=3;         
            } else {    
                payload = ((uint32_t)data[i] << 22) | ((uint32_t)data[i+1] << 14) | ((uint32_t)(data[i+2] & 0xF0) << 6); 
                i += 2;
            } 
            if(payload == 0){
                radio.write(&idle, sizeof(uint32_t));
                verif.decode(idle, erreur);
            }else{
                Serial.println(payload, BIN);
                verif.encode(payload);
                payload |= 0x80000000;
                radio.write(&payload, sizeof(uint32_t));
                Serial.print(j);
                Serial.print(": ");
                Serial.println(payload, BIN);
                payload &= 0x7FFFFFFF;
                verif.decode(payload, erreur);
            }
                      
        }   
    }
    Serial.println("\n\nFIN PAQUET\n\n\n\n\n");
}


void setRole(){
    if(R){
        Serial.println(F("Lecture"));
        radio.openReadingPipe(0, adressePipe);
        radio.startListening();  
        //messageBuffer = {0, 0};
    }else{
        Serial.println(F("Ecriture"));
        radio.stopListening(adressePipe);
        radio.setRetries(((3 * 3) % 12) + 3, 15); 
        //messageBuffer = {0, 0}; 
    }
}



void lireMessage(Message *message){
    uint8_t pipe;
    uint16_t error;
    static uint8_t nbPaquet = 0;
    uint8_t* data = (uint8_t*)message;
    static bool read =false;
    while (radio.available(&pipe)) {              
      uint8_t bytes = radio.getPayloadSize(); 
      uint32_t payload;
      radio.read(&payload, bytes);
      if(read){
        verif.decode(payload, error);
        if(!(payload&0x80000000)) {
            Serial.print(nbPaquet);
            Serial.print(F("ADDR: "));
            Serial.println((payload>>10),HEX);
        }else{
            data[nbPaquet] = ((char)(payload >> 24) & 0b1111111);
            data[nbPaquet+1] = ((char)(payload >> 17) & 0b1111111);
            data[nbPaquet+2] = ((char)(payload >> 10) & 0b1111111);
            //Serial.println(data[nbPaquet]);
            Serial.print(nbPaquet);
            Serial.print(": ");
            Serial.println(payload, HEX);
        }
        nbPaquet++;
      }
      if(payload==sync){
        nbPaquet = 0;
        Serial.println("\n\n\n\nSYNC");
        read=true;
      }

    }
}

uint8_t ete;
void Radio(){
    if(R){
        lireMessage(&messageBuffer);
    }else{
        envoyerMessage(&messageBuffer, LOW_P);
    }
    if(bouton(0,ETAT))
    {if(bouton(0,SELECTION)) R= !R; setRole();}

}