#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <Arduino.h>
#include <NRFLite.h>


typedef struct {
    int id;
    double timestamp;
    char message[128];
} Notification;

typedef struct{
    uint8_t taille;
    uint8_t limite_max;
    Notification notifications[8];

}Buffer;

int add(Buffer *buffer, Notification new_notification);
void afficherNotifications(Buffer *buffer);
void testRF();
void innitRadio();

#endif