#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <Arduino.h>
#include "type.h"

int add(Buffer *buffer, Message new_notification);
void innitRadio();

#endif