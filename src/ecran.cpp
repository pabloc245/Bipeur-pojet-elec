
#include "ecran.hpp"
#include "interface.hpp"

#define LARGEUR_CASE 16
#define HAUTEUR_CASE 8




Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
const uint8_t hauteurRect = 7;
const uint8_t largeurRect = 45;
uint8_t cursor_pos = 0;
char message[126] = {0};
const uint8_t AZERTY[4][8] PROGMEM = {
  { 'E','A','I','S','N','R','T','O' },
  { 'L','U','D','C','M','P','G','B' },
  { 'V','H','F','Q','Y','X','J','K' },
  { 'W',' ',' ',' ',' ',' ',' ','Z' }
};

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
  }
}

void clavier(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print(">>");
  int curseur[2] = {getEncoder() & 0b111, (getEncoder() >>3)& 0b11};
  display.setCursor(15, 0);
  display.print(message);
  uint8_t majuscule = bouton(0, SELECTION) ? 0 : 32;


  if(bouton(1, ETAT)){
    char d = pgm_read_byte(&AZERTY[curseur[1]][curseur[0]]);
    message[cursor_pos] = d+majuscule;
    cursor_pos++;
    //Serial.println(d);
  }
  for (uint8_t i = 0; i < 4; i++){
    for(uint8_t j= 0; j < 8; j++){
      display.setCursor(j*LARGEUR_CASE , i*HAUTEUR_CASE+15);
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
  display.display();
}

void top(){
  display.fillRect(0, 0, SCREEN_WIDTH+8, 9, WHITE);
  display.setTextColor(BLACK);
  display.setCursor(30,0);
  display.print(F("0 Messages"));
  display.drawBitmap(SCREEN_WIDTH - 20, 0, bitmap, 16, 8, BLACK);
}

Etats menu(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);


  String options[] = {
    "Envoyer",
    "Messages",
    "Parametre",
    "Sortie"
  };
  uint8_t taille = sizeof(options) / sizeof(options[0]);
  
  int shift = ((getEncoder() & 0b11) *4) + 10;
  Serial.println(shift);
  for(int i = 0; i < taille; i++){
    uint8_t col = WHITE;    
    display.setCursor(5, 13*i + 14);
    display.setTextColor(col);
    display.print(options[i&0b11]);
    if((getEncoder() & 0b11 ) == i){
        display.drawRoundRect(2, 9*i  + shift, largeurRect*2, hauteurRect*2, 2, col);  
    } 
  }
  top();
  display.display();
  if(bouton(1, ETAT)){
    return getEncoder() & 0b11;
  }else{
    return IDLE;
  }
}