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

Etats clavier(Etats exit){
  top();
  int curseur[2] = {getEncoder() & 0b111, (getEncoder() >>3)& 0b11};
  display.setCursor(0,10);
  display.print(">");
  display.print(messageBuffer.message);
  static uint8_t state = 0;

  if(bouton(1, ETAT)) {
    state = (state + 1) & 0b11;
  }
  signed char majuscule = (state & 0b10) ? ((state & 1) ? -32 : 32) : 0;

  if(bouton(0, DOUBLE) && exit == CLAVIER_M){
    display.clearDisplay();
    display.setCursor(15, 30);
    display.print(F("ENVOIE MESSAGE"));  
    for(uint8_t i = 0; i < 14; i++){
      display.fillRect(15+6*i, 40, 5, 5, WHITE);
      afficher();
    }
    setRole(TX);
    envoyerMessage(&messageBuffer, LOW_P);
    setRole(RX);  
    display.clearDisplay();
    display.setCursor(15,30);
    display.print(F("MESSAGE ENVOYE")); 
    afficher();
    delay(700);
    return IDLE;
  }else if(bouton(0, DOUBLE) && exit == CLAVIER_P){
    for(uint8_t i = 0; i < 7 ; i++){
      param.pseudo[i] = messageBuffer.message[i];
    }
    return PARAMETRE;
  }

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
    return exit;
  }
}

Etats menu(){
  top();
  uint8_t taille = sizeof(options) / sizeof(options[0]);
  //display.drawBitmap(SCREEN_WIDTH>>1, SCREEN_HEIGHT>>1, speaker8x8, 16, 8, WHITE);
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

void afficherNotifications() {
  static int ll = SCREEN_WIDTH;  
  static uint8_t fin = 0;
  // if(buffer->taille == 0) return;
  // vartop=false;
  // Message notif = buffer->notifications[buffer->taille-1];

  display.setTextColor(BLACK);
  display.setCursor(ll, 1);

  uint8_t taille = 28+strlen(messageBuffer.message)+8;
  char tempBuf[taille]; 
  uint8_t idx = 0;

  strncpy(&tempBuf[idx], "MSG recu de ", 12);
  idx += 12;

  strncpy(&tempBuf[idx], messageBuffer.pseudo, strlen(messageBuffer.pseudo));
  idx +=8;
  
  strcpy_P(&tempBuf[idx], PSTR(" - "));
  idx += 3;

  strncpy(&tempBuf[idx], messageBuffer.message, strlen(messageBuffer.message));
  idx = strlen(tempBuf);

  tempBuf[fin] = '\0';
  display.print(tempBuf);

  if(ll > -taille*4){
    fin = (SCREEN_WIDTH-ll)/6; 
    ll--;
  }else{
    //buffer->taille--;
    ll = SCREEN_WIDTH;
    vartop = true;
  }
}

Etats parametre(){
  top();
  uint8_t curs = getEncoder()%3;

  display.setCursor(3, 15);
  display.print(F("Canal radio < "));
  if(curs == 0){
    display.setTextColor(BLACK, WHITE);
    if(bouton(1, ETAT)){
      param.canal+=1;
      param.canal &= 0b111;
    }
  }
  display.print(param.canal);
  display.setTextColor(WHITE);
  display.println(" >");

  display.setCursor(3, 25);
  display.print(F("Pseudo "));
  if(curs == 1){
    display.setTextColor(BLACK, WHITE);
    if(bouton(1, ETAT)){
      return CLAVIER_P;
    }
  }
  display.drawRoundRect(40, 23, 8*7, 11, 2, WHITE);
  display.println(param.pseudo);
  display.setTextColor(WHITE);


  display.setCursor(3, 35);
  display.drawBitmap(8*6, 35, speaker8x8, 16, 8, WHITE);
  display.print(F("Alerte     < "));
  if(curs == 2){
    display.setTextColor(BLACK, WHITE);
    if(bouton(1, ETAT)){
      param.alerte+=1;
      param.alerte &= 0b1;
      play = true;
    }
  }
  display.print(param.alerte);
  display.setTextColor( WHITE);
  display.print(F(" >"));

  if(bouton(0,DOUBLE)){
    EEPROM.put(START_PARAMETRE, param);
    return IDLE;
  }else{
    return PARAMETRE;
  }  
}


Etats messages(bool first){
  top();

  static uint8_t n = 0;
  static uint8_t lastn = 0;
  static uint8_t taille = 0;
  if(first || n!=lastn){
    uint8_t liste[5];
    Contact tmp;
    lireContacts(&tmp, curseur);

    taille = filtreMessage(liste, tmp.adresse);
    Serial.print(F("liste"));
    for(uint8_t i = 0; i < taille; i++){
      Serial.print(liste[i]);
    }
    Serial.print(F("Contact: "));
    Serial.print(tmp.pseudo);
    Serial.print(F(" addr: "));
    Serial.println(tmp.adresse, HEX);


    Serial.print(F("Il y a "));
    Serial.print(taille);
    Serial.println(F(" messages"));
    if(n>0){
      readMessage(&messageBuffer, liste[n]);
    } 
  }

  lastn = n;
  if(taille>0){
    display.setCursor(5,11);
    display.print(n+1);
    display.print(F(" message de "));
    display.println(messageBuffer.pseudo);
    display.println(messageBuffer.message);
    display.drawRoundRect(5,21,SCREEN_WIDTH-10,SCREEN_HEIGHT-25,3,WHITE);
  }else{
    display.setCursor(13, SCREEN_HEIGHT>>1);
    display.print(F("Il n'y a pas de message de "));
    display.println(messageBuffer.pseudo);
  }
  if(bouton(1, ETAT)){
    if((n+1)<taille){
      n++;
    }
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
  uint8_t shift = curseur >= 5 ? 64 : 0;

  display.setCursor(10, 12);
  display.print(F("PSEUDO     ADRESSE"));
  display.drawRect(5, 10, SCREEN_WIDTH-10, 11, WHITE);
  for(uint8_t i = 0; i < nbContact; i++){
    Contact tmp;
    lireContacts(&tmp, i);
    display.drawRect(5, 10+(i+1)*11, SCREEN_WIDTH-10, 10, WHITE);
    display.setTextColor(WHITE);
    display.setCursor(10, 23 + i*11 - shift);
    display.print(tmp.pseudo);
    display.setCursor(75, 23 + i*11 - shift);
    display.print(tmp.adresse, HEX);
    display.setCursor(0, 23 + 11 * curseur - shift);
    display.print(">");
  }


  if(bouton(1, ETAT)){
    return IDLE;
  }else if(bouton(0,ETAT)){
    messages(true);
    return MESSAGE;
  }else{  
    return CONTACT;
  } 
}

void afficher(){
  display.display();
}