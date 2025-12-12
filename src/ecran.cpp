
#include "ecran.hpp"
#include "interface.hpp"
#include <EEPROM.h>

#define LARGEUR_CASE 16
#define HAUTEUR_CASE 8
#define paramCount 3

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

const char prefix[] PROGMEM = "msg recu de ";

const char option0[] PROGMEM = "Envoyer";
const char option1[] PROGMEM = "Messages";
const char option2[] PROGMEM = "Parametre";
const char option3[] PROGMEM = "Sortie";
const char* const options[] PROGMEM = {option0, option1, option2, option3};

bool vartop=true;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const uint8_t PROGMEM bitmap[] = {
  0x00, 0x00,  // Ligne 1
  0x00, 0x03,  // Ligne 2
  0x00, 0x33,  // Ligne 3
  0x00, 0x33,  // Ligne 4
  0x03, 0x33,  // Ligne 5
  0x03, 0x33,  // Ligne 6
  0x33, 0x33,  // Ligne 7
  0x33, 0x33   // Ligne 8
};


void innitDisplay(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }else{
    Serial.println(F("SSD1306 allocation successful"));

    // --- EEPROM ---
void sauvegarder(){
  EEPROM.put(0, pseudo);
  EEPROM.put(20, alerteSon);
  EEPROM.put(30, canalRadio);
}

void charger() {
  EEPROM.get(0, pseudo);
  EEPROM.get(20, alerteSon);
  EEPROM.get(30, canalRadio);
  if (canalRadio < 1 || canalRadio > 10) canalRadio = 1; // sécurité
}
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
  display.print(">>");
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
  static uint8_t ll = SCREEN_WIDTH+30;  
  
  if(buffer->taille == 0) return;
  vartop=false;
  Notification notif = buffer->notifications[buffer->taille-1];
  display.setTextColor(BLACK);
  display.setCursor(ll, 0);

  char tempBuf[22]; 
  uint8_t idx = 0;
  uint8_t start = ll;

  for (uint8_t i = 0; i < sizeof(prefix); i++) {
    char c = pgm_read_byte(&prefix[i]);
    Serial.println(sizeof(prefix));
    tempBuf[idx++] = c;
  }
  itoa(notif.timestamp, &tempBuf[idx], 10);
  idx = strlen(tempBuf);
  
  strcpy_P(&tempBuf[idx], PSTR(" - "));
  idx += 3;

  strncpy(&tempBuf[idx], notif.message, 30);
  tempBuf[idx + 30] = '\0';

  display.print(tempBuf);

  if(ll > 0){  
    --ll;
  }else{
    buffer->taille--;
    ll = SCREEN_WIDTH;
    vartop = true;
  }
}


/// @brief Les paramètre doivent être ecrit dans l'EPROM
Etats parametre(){
  // --- Types de paramètres ---
enum ParamType { TYPE_STRING, TYPE_BOOL, TYPE_INT };

struct Param {
  const char* label; // Nom affiché
  ParamType type;    // Type de donnée
  void* value;       // Pointeur vers la valeur
};

// --- Variables des paramètres ---
char pseudo[10] = "User";
bool alerteSon = true;
int canalRadio = 1;

// --- Tableau des paramètres ---
Param params[paramCount] = {
  {"Pseudo", TYPE_STRING, pseudo},
  {"Alerte son", TYPE_BOOL, &alerteSon},
  {"Canal radio", TYPE_INT, &canalRadio}
};

// --- États ---
enum Mode { MODE_MENU, MODE_EDIT };
Mode mode = MODE_MENU;
int selection = 0;

// --- Affichage générique ---
void afficher(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("=== Parametres ===");

  for (int i = 0; i < paramCount; i++) {
    int y = 16 + i * 12;

    // Surbrillance si sélectionné
    if (i == selection) {
      display.fillRect(0, y, SCREEN_WIDTH, 10, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
    } else {
      display.setTextColor(SSD1306_WHITE);
    }

    // Nom du paramètre
    display.setCursor(2, y);
    display.print(params[i].label);

    // Valeur à droite
    display.setCursor(90, y);
    if (params[i].type == TYPE_STRING) display.print((char*)params[i].value);
    if (params[i].type == TYPE_BOOL) display.print(*(bool*)params[i].value ? "ON" : "OFF");
    if (params[i].type == TYPE_INT) display.print(*(int*)params[i].value);

    display.setTextColor(SSD1306_WHITE);
  }
}
// --- Modification générique ---
void modifierParam(int dir) {
  Param &p = params[select];

  if (p.type == TYPE_STRING) {
    int i=0
    char* str = (char*)p.value;
    str[i] += dir;
    if (str[i] < 'A') str[i] = 'Z';
    if (str[i] > 'Z') str[i] = 'A';
    if (bouton(1,ETAT)) i++;
  }
  else if (p.type == TYPE_BOOL) {
    *(bool*)p.value = !*(bool*)p.value;
  }
  else if (p.type == TYPE_INT) {
    int &val = *(int*)p.value;
    val += dir;
    if (val < 1) val = 15;
    if (val > 15) val = 1;
  }
}
lireEncodeur(){
int currentCLK = DigitalRead(pinA);
    if (currentCLK != lastCLK ) {
     if(digitalRead(pinB) != currentCLK) {
dir= 1;
}else{
dir = -1;
}
      lastCLK = currentCLK;
      if (mode == MODE_MENU) {
        selection += dir;
        if (selection < 0) selection = paramCount - 1;
        if (selection >= paramCount) selection = 0;
        afficher();
      } 
      else if (mode == MODE_EDIT) {
        modifierParam(dir);
        afficher();
        lastButtonState = buttonState;
      }
    }
  }

  // Lecture bouton

  if(bouton(0,ETAT)){
    if (mode == MODE_MENU) {
      mode = MODE_EDIT;
    } else {
      mode = MODE_MENU;
      sauvegarder();
    }
    afficher();
    delay(200); // anti-rebond
  }


  
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