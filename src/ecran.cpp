#include "ecran.hpp"

#define LARGEUR_CASE 16
#define HAUTEUR_CASE 11
#define HAUTEUR_RECT 7
#define LARGEUR_RECT 45

uint8_t cursor_pos = 0;
uint8_t curseur = 0; 

const uint8_t AZERTY[4][8] PROGMEM = {
  { 'E','A','I','S','N','R','T','O' },
  { 'L','U','D','C','M','P','G','B' },
  { 'V','H','F','Q','Y','X','J','K' },
  { 'W',' ',' ',' ',' ',' ',' ','Z' }
};

const char option0[] PROGMEM = "Envoyer";
const char option1[] PROGMEM = "Messages";
const char option2[] PROGMEM = "Parametre";
const char option3[] PROGMEM = "Sortie";
const char* const options[] PROGMEM = {option0, option1, option2, option3};

bool vartop=true;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


const uint8_t PROGMEM bitmap[] = {
  0x00, 0x00,  
  0x00, 0x03,  
  0x00, 0x33,  
  0x00, 0x33, 
  0x03, 0x33,  
  0x03, 0x33,  
  0x33, 0x33,  
  0x33, 0x33   
};

const uint8_t PROGMEM speaker8x8[] = {
  0b00000010, 0b00001000,
  0b00000110, 0b00100100,
  0b00111110, 0b00010010,
  0b00111110, 0b10010010,
  0b00111110, 0b10010010,
  0b00111110, 0b00010010,
  0b00000110, 0b00100100,
  0b00000010, 0b00001000
};


void innitDisplay(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }else{
    Serial.println(F("SSD1306 allocation successful"));
    ///CHARGE LES PRAMETRES DEPUIS L'EPROM///
  }
}


void top(){
  display.clearDisplay();
  display.setTextSize(1);
  display.fillRect(0, 0, SCREEN_WIDTH+8, 9, WHITE);
  if(vartop){
    display.setTextColor(BLACK);
    display.setCursor(30,0);
    display.print(nbMessage),
    display.print(F(" Message"));
    display.drawBitmap(SCREEN_WIDTH - 20, 0, bitmap, 16, 8, BLACK);
  }
  display.setTextColor(WHITE);

}

Etats clavier(){
  top();
  int curseur[2] = {getEncoder() & 0b111, (getEncoder() >>3)& 0b11};
  display.setCursor(0,10);
  display.print(">");
  display.print(messageBuffer.message);
  static uint8_t state = 0;
  if(bouton(0, ETAT)) {
      state = (state + 1) & 0b11;
  }
  signed char majuscule = (state & 0b10) ? ((state & 1) ? -32 : 32) : 0;


  if(bouton(1, ETAT)){
    char d = pgm_read_byte(&AZERTY[curseur[1]][curseur[0]]);
    messageBuffer.message[cursor_pos] = d+majuscule;
    cursor_pos++;
  }
  for (uint8_t i = 0; i < 4; i++){
    for(uint8_t j= 0; j < 8; j++){
      display.setCursor(j*LARGEUR_CASE + 5 , i*HAUTEUR_CASE+22);
      char c = pgm_read_byte(&AZERTY[i][j]);
            
      if(j==abs(curseur[0]) && i==abs(curseur[1])){
        display.setTextColor(BLACK, WHITE);
        display.print((char)(c+majuscule));
        display.setTextColor(WHITE);
        display.drawRoundRect(j*LARGEUR_CASE-2 , i*HAUTEUR_CASE+20, 16, 12, 2, WHITE);
      }else{
        display.print((char)(c+majuscule));
        display.drawRoundRect(j*LARGEUR_CASE , i*HAUTEUR_CASE+20, 12, 11, 2, WHITE);
      }
    }
  }   
  if(bouton(0, DOUBLE)){
    return IDLE;
  }else{
    return CLAVIER;
  }
}

Etats menu(){
  top();
  uint8_t taille = sizeof(options) / sizeof(options[0]);
  display.drawBitmap(SCREEN_WIDTH>>1, SCREEN_HEIGHT>>1, speaker8x8, 16, 8, WHITE);
  int shift = ((getEncoder() & 0b11) *4) + 10;
  for(int i = 0; i < taille; i++){
    char buffer[20];
    strcpy_P(buffer, (char*)pgm_read_word(&(options[i])));

    display.setCursor(5, 13*i + 14);
    display.setTextColor(WHITE);
    display.print(buffer);
    if((getEncoder() & 0b11 ) == i){
      display.drawRoundRect(2, 9*i  + shift, LARGEUR_RECT*2, HAUTEUR_RECT*2, 2, WHITE);  
    } 
  }
  
  if(bouton(1, ETAT)){
    return static_cast<Etats>(getEncoder() & 0b11);
  }else{
    return IDLE;
  }
}

void afficherNotifications(Buffer *buffer) {
  static int ll = SCREEN_WIDTH;  
  static uint8_t fin = 0;
  if(buffer->taille == 0) return;
  vartop=false;
  Message notif = buffer->notifications[buffer->taille-1];
  display.setTextColor(BLACK);
  display.setCursor(ll, 1);

  uint8_t taille = 28+strlen(notif.message)+8;
  char tempBuf[taille]; 
  uint8_t idx = 0;

  strncpy(&tempBuf[idx], "MSG recu de ", 12);
  idx += 12;

  strncpy(&tempBuf[idx], notif.pseudo, strlen(notif.pseudo));
  idx +=8;
  
  strcpy_P(&tempBuf[idx], PSTR(" - "));
  idx += 3;

  strncpy(&tempBuf[idx], notif.message, strlen(notif.message));
  idx = strlen(tempBuf);

  tempBuf[fin] = '\0';
  display.print(tempBuf);

  if(ll > -taille*4){
    fin = (SCREEN_WIDTH-ll)/6; 
    ll--;
  }else{
    buffer->taille--;
    ll = SCREEN_WIDTH;
    vartop = true;
  }
}


/**
 * @brief Les paramètre doivent être ecrit dans l'EPROM
 */


Etats parametre(){
  //AFFICHAGE BARRE DU HAUT
  top();
  display.setCursor(0, 10);
  //EXEMPLE DE GUI
  display.println(F("Canal radio: "));
  display.println(F("Pseudo: "));
  display.println(F("Son alerte: "));


  //RETOUR AU MENU PRINCIPALE SI DOUBLE CLICK
  if(bouton(0,DOUBLE)){
    return IDLE;
  }else{
    return PARAMETRE;
  }  
}


Etats messages(){
  top();
  uint8_t liste[5];
  Contact tmp;
  lireContacts(&tmp, curseur);
  uint8_t taille = filtreMessage(liste, tmp.pseudo);

  if(taille>0){
      display.setCursor(20,20);
      display.print(F("Il y a "));
      display.print(taille);
      display.print(F(" messages de "));
      display.println(tmp.pseudo);
    }else{
      display.setCursor((SCREEN_WIDTH>>1) - 6*5, SCREEN_HEIGHT>>1);
      display.print(F("Il n'y a pas de message de "));
      display.println(tmp.pseudo);
    }

  if(bouton(0,DOUBLE)){
    return IDLE;
  }else{  
    return MESSAGE;
  }  
}

Etats contact(){
  top();
  curseur = (uint16_t)(getEncoder()%nbContact);
  uint8_t shift = curseur>= 5 ? 64 : 0;

  for(uint8_t i = 0; i < nbContact; i++){
    display.setTextColor(WHITE);
    Contact tmp;
    lireContacts(&tmp, i);
    display.setCursor(15, 15 +i*10 - shift);
    display.print(tmp.pseudo);
    display.setCursor(70, 15 + i*10 - shift);
    display.print(tmp.adresse, HEX);
    display.setCursor(0, 15 + 10 * curseur -shift);
    display.print(">");
  }

  if(bouton(0,ETAT)){
    return MESSAGE;
  }
  if(bouton(1, ETAT)){
    return IDLE;
  }else{  
    return CONTACT;
  } 

}

void afficher(){
  display.display();
}