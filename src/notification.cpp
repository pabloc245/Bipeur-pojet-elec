#include "notification.hpp"


int add(Buffer *buffer, Message new_notification) {
    if (buffer->taille >= buffer->limite_max) {
        Serial.println(F("Buffer plein (limite: 255)"));
        return -1;
    }    
    buffer->notifications[buffer->taille] = new_notification;
    buffer->taille++;
    return 0;
}