
#include "ecran.hpp"
#include "interface.hpp"

#define LARGEUR_CASE 16
#define HAUTEUR_CASE 8

const uint8_t hauteurRect = 7;
const uint8_t largeurRect = 45;
uint8_t cursor_pos = 0;
char message[64] = {0};
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
    display.print(F("0 Messages"));
    display.drawBitmap(SCREEN_WIDTH - 20, 0, bitmap, 16, 8, BLACK);
  }
  display.setTextColor(WHITE);

}

Etats clavier(){
  top();
  int curseur[2] = {getEncoder() & 0b111, (getEncoder() >>3)& 0b11};
  display.setCursor(0,10);
  //display.print(">>");
  display.print(message);
  uint8_t majuscule = bouton(0, SELECTION) ? 0 : 32;


  if(bouton(1, ETAT)){
    char d = pgm_read_byte(&AZERTY[curseur[1]][curseur[0]]);
    message[cursor_pos] = d+majuscule;
    cursor_pos++;
  }
  for (uint8_t i = 0; i < 4; i++){
    for(uint8_t j= 0; j < 8; j++){
      display.setCursor(j*LARGEUR_CASE , i*HAUTEUR_CASE+20);
      char c = pgm_read_byte(&AZERTY[i][j]);
            
      if(j==abs(curseur[0]) && i==abs(curseur[1])){
        display.setTextColor(BLACK, WHITE);
        display.print((char)(c+majuscule));
        display.setTextColor(WHITE);
      }else{
        display.print((char)(c+majuscule));
      }
    }
  }   
  if(bouton(0,DOUBLE)){
    return IDLE;
  }else{
    return CLAVIER;
  }
}

Etats menu(){
  top();
  uint8_t taille = sizeof(options) / sizeof(options[0]);
  int shift = ((getEncoder() & 0b11) *4) + 10;
  for(int i = 0; i < taille; i++){
    char buffer[20];
    strcpy_P(buffer, (char*)pgm_read_word(&(options[i])));

    display.setCursor(5, 13*i + 14);
    display.setTextColor(WHITE);
    display.print(buffer);
    if((getEncoder() & 0b11 ) == i){
      display.drawRoundRect(2, 9*i  + shift, largeurRect*2, hauteurRect*2, 2, WHITE);  
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
  Notification notif = buffer->notifications[buffer->taille-1];
  display.setTextColor(BLACK);
  display.setCursor(ll, 1);

  uint8_t taille = 28+strlen(notif.message);
  char tempBuf[taille]; 
  uint8_t idx = 0;

  strncpy(&tempBuf[idx], "MSG recu de ", 12);
  idx += 12;

  itoa(notif.id, &tempBuf[idx], 2);
  idx = strlen(tempBuf);
  
  strcpy_P(&tempBuf[idx], PSTR(" - "));
  idx += 3;

  itoa(notif.timestamp, &tempBuf[idx], 10);
  idx = strlen(tempBuf);

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


/// @brief Les paramètre doivent être ecrit dans l'EPROM


Etats parametre(){
  //AFFICHAGE BARRE DU HAUT
  top();
  display.setCursor(0, 10);
  //EXEMPLE DE GUI
  display.println(F("Canal radio: "));
  display.println(F("Pseudo: "));
  display.println(F("Son alerte: "));


  /*! FONCTION bouton()
      @brief  permet de savoir dans quel etat son les boutons
      @param  bouton  0 = gros bouton  1 = bouton potentionmètre
      @param  select  ETAT = ON ou OFF, 
      SELECTION = flip à chaque foi que l'on appuie dessus,
      DOUBLE = double clique
  */



  //RETOUR AU MENU PRINCIPALE SI DOUBLE CLICK
  if(bouton(0,DOUBLE)){
    return IDLE;
  }else{
    return PARAMETRE;
  }  
}


Etats messages(){
  top();
  display.setCursor(0, 20);
  display.println(F("messages"));

  if(bouton(0,DOUBLE)){
    return IDLE;
  }else{  
    return DISPLAY_NOTIF;
  }  
}

void afficher(){
  display.display();
}