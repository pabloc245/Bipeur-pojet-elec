#ifndef RADIO_H
#define RADIO_H

#include <SPI.h>
#include "RF24.h"
#include "type.h"
#include "bch.hpp"
#include "interface.hpp"

#ifndef DEVICE_ID
#define DEVICE_ID 0x1234
#endif

#define TX 0
#define RX 1

void initRadio();

extern Message messageBuffer;
extern Parametre param;

uint32_t hashPseudo(const char* pseudo);
uint8_t envoyerMessage(const void *message, const uint8_t priorite);
void lireMessage(Message *message);
void setRole(bool R);


#endif