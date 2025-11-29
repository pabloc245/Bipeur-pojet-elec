
#include "ecran.hpp"
#include "interface.hpp"

#define LARGEUR_CASE 16
#define HAUTEUR_CASE 12
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
char message[126] = {0};
uint8_t cursor_pos = 0;
const uint8_t AZERTY[4][8] PROGMEM = {
  { 'E','A','I','S','N','R','T','O' },
  { 'L','U','D','C','M','p','g','B' },
  { 'V','H','F','Q','Y','X','J','K' },
  { 'W',' ',' ',' ',' ',' ',' ','Z' }
};
void innitDisplay(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }else{
    Serial.println(F("SSD1306 allocation successful"));
  }
}
void affichage(){  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(SCREEN_WIDTH/2 - 55, 33);
  display.fillRoundRect(5, 5, ((SCREEN_WIDTH-5)/1023.0), 25, 1, WHITE);
  display.drawRect(5, 5, SCREEN_WIDTH-5, 25, WHITE);
  display.print(F("frequence: "));
  display.print(5);
  display.print("Hz\n");
  display.setCursor(SCREEN_WIDTH/2 - 25, 45);
 // display.invertDisplay(selected);
 // display.print(selected ? "ACTIVE" : "DESACTIVE");
  display.display();
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
  if(bouton() & (1<<0) && cursor_pos < 126){
    message[cursor_pos] = AZERTY[curseur[1]][curseur[0]];
    cursor_pos++;
  }
  uint8_t majuscule = bouton() & (1<<2)? 0 : 32;
  for (uint8_t i = 0; i < 4; i++){
    for(uint8_t j= 0; j< 8; j++){
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