#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <Arduino.h>
#include <NRFLite.h>
#include "type.h"

int add(Buffer *buffer, Notification new_notification);
void testRF();
void innitRadio();

#endif