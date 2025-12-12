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

void initRadio();
void Radio();

extern Message messageBuffer;
extern Parametre param;


void envoyerMessage(const void *message, const uint8_t priorite);

#endif