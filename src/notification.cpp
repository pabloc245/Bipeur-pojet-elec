#include "notification.hpp"

const static uint8_t RADIO_ID = 0;
const static uint8_t DESTINATION_RADIO_ID = 1; // The radio ID of the destination radio.
const static uint8_t PIN_RADIO_CE = 7;
const static uint8_t PIN_RADIO_CSN = 8;
const static uint8_t PIN_RADIO_MOSI = 11;
const static uint8_t PIN_RADIO_MISO = 12;
const static uint8_t PIN_RADIO_SCK = 13;

struct __attribute__((packed)) RadioPacket // Note the packed attribute.
{
    uint8_t FromRadioId;
    uint32_t OnTimeMillis;
    uint32_t FailedTxCount;
};

RadioPacket _radioData;
NRFLite _radio;


void innitRadio(){
  if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN)){
    Serial.println(F("Cannot communicate with radio"));
    while (1); // Wait here forever.
  }else{
    Serial.println(F("NRF24XX allocation successful"));
  }

  _radioData.FromRadioId = RADIO_ID;
}

int add(Buffer *buffer, Notification new_notification) {
    if (buffer->taille >= buffer->limite_max) {
        Serial.println(F("Buffer plein (limite: 255)"));
        return -1;
    }    
    buffer->notifications[buffer->taille] = new_notification;
    buffer->taille++;
    return 0;
}

void afficherNotifications(Buffer *buffer) {
  for (uint8_t i = 0; i < buffer->taille; i++) {
    Notification notif = buffer->notifications[i];
    Serial.print(F("Notification reçu de : "));
    Serial.print(notif.id);
    Serial.print(" - ");
    Serial.print(notif.timestamp);  
    Serial.print(" - ");
    Serial.println(notif.message);
    
  }
  buffer->taille = 0;
  }

void testRF(){
  
  #if ENV_MODE==1 // Server mode
  _radioData.OnTimeMillis = millis();

  Serial.print("Sending ");
  Serial.print(_radioData.OnTimeMillis);
  Serial.print(" ms");
  if (_radio.send(0, &_radioData, sizeof(_radioData))){ // Note how '&' must be placed in front of the variable name.
    Serial.println("...Success");
  }  else{
    Serial.println("...Failed");
    _radioData.FailedTxCount++;
  }

  delay(1000);
  #elif ENV_MODE==0 // Client mode
  while (_radio.hasData()){
    _radio.readData(&_radioData); // Note how '&' must be placed in front of the variable name.

    String msg = "Radio ";
    msg += _radioData.FromRadioId;
    msg += ", ";
    msg += _radioData.OnTimeMillis;
    msg += " ms, ";
    msg += _radioData.FailedTxCount;
    msg += " Failed TX";

    Serial.println(msg);
  }

  #endif
}