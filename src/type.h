#ifndef TYPE_H
#define TYPE_H

typedef unsigned char uint8_t;


enum Etats {
  IDLE = -1,
  CLAVIER_M = 0,
  CONTACT = 1,
  PARAMETRE = 2,
  MESSAGE = 5,
  CLAVIER_P = 12
};

enum Priorite{
    LOW_P = 0,
    MEDIUM_P = 0b11,
    HIGH_P = 0b10,
};

typedef  struct __attribute__((packed)){
    char pseudo[7];
    char message[30];
    uint32_t adresse;
} Message;

typedef struct{
    uint8_t taille;
    uint8_t limite_max;
    Message notifications[1];
}Buffer;

typedef struct  __attribute__((packed)){
  char pseudo[7];
  uint8_t canal;
  uint8_t alerte;
}Parametre;

typedef struct __attribute__((packed)){
  char pseudo[7];  
  uint32_t adresse;      
} Contact; 

extern Parametre param;

#endif 