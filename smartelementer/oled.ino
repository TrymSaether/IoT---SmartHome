/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

 int Alex = 0;
 int Trym = 2;
 int Isak = 2;
 int Eirik = 1;
 int Lorenz = 2;
 int Total = 0;
void setup() {
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
}

void loop() {
  Total = Alex + Trym + Isak + Eirik + Lorenz;
   display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  // Display static text
  display.println("Antall personer!");
  display.setCursor(10, 16);
  display.print("Alex   ");
  display.println(Alex);
  display.setCursor(10, 24);
  display.print("Trym   ");
  display.println(Trym);
  display.setCursor(10, 32);
  display.print("Eirik  ");
  display.println(Eirik);
  display.setCursor(10, 40);
  display.print("Lorenz ");
  display.println(Lorenz);
  display.setCursor(10, 40);
  display.print("Isak ");
  display.println(Isak);
  display.setCursor(70, 16);
  display.println("Total");
  display.setCursor(70, 24);
  display.setTextSize(3);
  display.println(Total);
  
  display.display(); 
  delay(2000);
}