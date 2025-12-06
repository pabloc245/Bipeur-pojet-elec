#ifndef TYPE_H
#define TYPE_H

typedef unsigned char uint8_t;

enum Etats {
  IDLE = -1,
  CLAVIER = 0,
  DISPLAY_NOTIF = 1,
  PARAMETRE = 2
};

typedef struct {
    uint8_t id;
    int timestamp;
    char message[13];
} Notification;

typedef struct{
    uint8_t taille;
    uint8_t limite_max;
    Notification notifications[8];

}Buffer;

#endif 