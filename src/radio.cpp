#include "radio.hpp"

#define VIDE 0x7A89C197
#define MSG 0x80000000

static uint32_t sync = 0x7CD215D8;
static uint32_t idle = 0x7A89C197;

const uint8_t adressePipe[5] = {0xDE,0xAD,0xBE,0xEF,0x01};

CBCH3121 verif;

RF24 radio(CE_PIN, CSN_PIN);



Message messageBuffer = {0, 0};

void initRadio(){
    if (!radio.begin(CE_PIN, CSN_PIN)) {
        Serial.println(F("radio hardware is not responding!!"));
        for(;;);
    }
    //radio.setAddressWidth(3);
    radio.setPALevel(RF24_PA_MAX);  
    //radio.disableDynamicPayloads();
    radio.setPayloadSize(sizeof(uint8_t)*4);  
    radio.setAutoAck(true);
    setRole(RX);
}


uint32_t hashPseudo(const char* pseudo) {
    uint32_t hash = ((uint32_t)pseudo[0] << 24) | ((uint32_t)pseudo[1] << 16) | ((uint32_t)pseudo[2] << 8) | pseudo[3];
    hash ^= ((uint32_t)pseudo[4] << 20) | ((uint32_t)pseudo[5] << 10) | pseudo[6];
    hash = (hash << 16) ^ ((uint32_t)DEVICE_ID * 2654435761UL);  
    hash ^= (hash >> 5);
    hash += (hash << 7);
    hash ^= (hash >> 13);
    
    return hash & 0x1FFFFF;
}

bool toggle = true;

uint8_t envoyerMessage(const void *message, const uint8_t priorite){
    uint16_t erreur = 0;
    const uint8_t* data = (const uint8_t*)message;
    uint8_t i = 0;
    uint32_t addr = hashPseudo(param.pseudo);
    

    Serial.println(F("\n\n\nenvoie message"));
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
            Serial.print(j);
            Serial.print(F(" : "));
            Serial.println(payload,HEX);
           
            //Serial.print(F(" ADDR: "));
            Serial.println(((payload >> 13) & 0x3FFFF) << 3 | (j/2), HEX);
    
            verif.decode(payload, erreur);
            radio.write(&addr, sizeof(uint32_t));
        }else{
            if(!toggle) { 
                payload = 
                    ((uint32_t)(data[i] & 0x0F) << 27) | 
                    ((uint32_t)data[i+1] << 19) | 
                    ((uint32_t)data[i+2] << 11);  
                Serial.println(payload,BIN);

                i+=3;         
                toggle = !toggle;
            } else {    
                payload = 
                    ((uint32_t)data[i] << 23) | 
                    ((uint32_t)data[i+1] << 15) | 
                    ((uint32_t)(data[i+2] & 0xF0) << 7); 
                i += 2;
                toggle = !toggle;
            } 
            if(payload == 0){
                Serial.print(j);
                Serial.print(F(" : "));
                Serial.println(payload,BIN);
                radio.write(&idle, sizeof(uint32_t));
                verif.decode(idle, erreur);
            }else{
                payload |= 0x80000000;
                verif.encode(payload);
                Serial.print(j);
                Serial.print(" : ");
                Serial.println(payload, HEX);
                radio.write(&payload, sizeof(uint32_t));
                payload &= 0x7FFFFFFF;
                verif.decode(payload, erreur);
            }
                      
        }   
        delay(500);
    }
    return 1;
    Serial.println("\n\nFIN PAQUET\n\n\n\n\n");
}


void setRole(bool R){
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


uint8_t nbPaquet = 0;
void lireMessage(Message *message){
    uint8_t pipe;
    uint16_t error; 
    static uint8_t index = 0;
    static bool read =false;
    uint8_t* data = (uint8_t*)message;
    while (radio.available(&pipe)) {              
      uint8_t bytes = radio.getPayloadSize(); 
      uint32_t payload;

      radio.read(&payload, bytes);
      verif.decode(payload, error);

      if(read){
        Serial.print(nbPaquet);
        Serial.print(" : ");
        Serial.println(payload, HEX);
        if(0){////ADRESSE
            Serial.print(nbPaquet);
            Serial.print(F("ADDR: "));
            Serial.println(((payload<<1)>>10),HEX);
        }else if(payload == idle){////IDLE
            if(toggle) {
                toggle = !toggle;
                index+= 2;
            } else {
                toggle = !toggle;
                index+= 3;
            }
            Serial.println("IDLE");
        }else{///DONNE
            payload &= 0x7FFFFFFF;
            if(toggle) {
                data[index] = (payload >> 23) & 0xFF;
                data[index+1] = (payload >> 15) & 0xFF;
                data[index+2] = (payload >> 7) & 0xF0;
                Serial.print(F("text_p: "));
                Serial.print((char)data[index]);
                Serial.println((char)data[index+1]);
                toggle = !toggle;
                index+= 2;
            } else {
                data[index] |= (payload >> 27) & 0x0F;
                data[index+1] = (payload >> 19) & 0xFF;
                data[index+2] = (payload >> 11) & 0xFF;
                Serial.print(F("text_imp: "));
                Serial.print((char)data[index]);
                Serial.print((char)data[index+1]);
                Serial.println((char)data[index+2]);
                toggle = !toggle;
                index+= 3;
            }
        }
        nbPaquet++;
      }
      if(nbPaquet > 15){
        payload = 0;
        message = (Message*)data;
        Serial.print(F("Pseudo: "));
        Serial.println(message->pseudo);
        Serial.print(F("Message: "));
        Serial.println(message->message);
        writeMessage(message);
      }

      if(payload==sync){
        index = 0;
        nbPaquet = 0;
        Serial.println(F("\n\n\n\nSYNC"));
        toggle = true;
        read=true;
      }
    }
}
uint8_t getpack(){
    return nbPaquet;
}

//uint8_t ete;
// void Radio(){
//     if(R){
//         lireMessage(&messageBuffer);
//     }else{
//         envoyerMessage(&messageBuffer, LOW_P);
//     }
//     if(bouton(0,ETAT))
//     {if(bouton(0,SELECTION)) R= !R; setRole();}

// }