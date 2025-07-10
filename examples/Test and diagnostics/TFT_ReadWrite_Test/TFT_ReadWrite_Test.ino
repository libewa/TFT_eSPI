// Walking 1 write and read pixel test

#include <TFT_eSPI.h>
#include <SPI.h>

#define TDELAY 500

TFT_eSPI tft = TFT_eSPI(); 

void setup() {
  Serial.begin(115200);

  tft.init();
  tft.fillScreen(0xF81F);
}

void loop() {
  static unsigned int wr = 1;
  static unsigned int rd = 0xFFFFFFFF;

  delay(TDELAY);

  tft.drawPixel(30,30,wr);
  Serial.print(" Pixel value written = ");Serial.println(wr,HEX);
  
  rd = tft.readPixel(30,30);
                Serial.print(" Pixel value read    = ");Serial.println(rd,HEX);

  if (rd!=wr) {
    Serial.println(" ERROR                 ^^^^");
    //while(1) yield();
  }
  else Serial.println(" PASS ");
  // Walking 1 test
  wr = wr<<1;
  if (wr >= 0x10000) wr = 1;
}
