
#include "ecran.hpp"
#include "interface.hpp"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

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
  display.print("frequence: ");
  display.print(5);
  display.print("Hz\n");
  display.setCursor(SCREEN_WIDTH/2 - 25, 45);
 // display.invertDisplay(selected);
 // display.print(selected ? "ACTIVE" : "DESACTIVE");
  display.display();
}