// Arc drawing example - draw a colour wheel

#include <TFT_eSPI.h>       // Include the graphics library
TFT_eSPI tft = TFT_eSPI();  // Create object "tft"

unsigned short colors[12];

// -------------------------------------------------------------------------
// Setup
// -------------------------------------------------------------------------
void setup(void) {
  Serial.begin(115200);
  tft.init();
  tft.fillScreen(TFT_BLACK);

  // Create the outer ring colours
  for (unsigned char c = 0; c < 2; c++) {
    colors[c + 10] = tft.alphaBlend(128 + c * 127, TFT_RED,     TFT_MAGENTA);
    colors[c +  8] = tft.alphaBlend(128 + c * 127, TFT_MAGENTA, TFT_BLUE);
    colors[c +  6] = tft.alphaBlend(128 + c * 127, TFT_BLUE,    TFT_GREEN);
    colors[c +  4] = tft.alphaBlend(128 + c * 127, TFT_GREEN,   TFT_YELLOW);
    colors[c +  2] = tft.alphaBlend(128 + c * 127, TFT_YELLOW,  TFT_ORANGE);
    colors[c +  0] = tft.alphaBlend(128 + c * 127, TFT_ORANGE,  TFT_RED);
  }
}

// -------------------------------------------------------------------------
// Main loop
// -------------------------------------------------------------------------
void loop() {
  unsigned short rDelta = (tft.width() - 1) / 10;
  unsigned short x = tft.width() / 2;
  unsigned short y = tft.height() / 2;
  bool smooth = true;

  // Draw rings as a series of arcs, increasingly blend colour with white towards middle
  for (unsigned short i = 5; i > 0; i--) {
    for (unsigned short angle = 0; angle <= 330; angle += 30) {
      unsigned short radius = i * rDelta;
      unsigned short wheelColor = tft.alphaBlend((i * 255.0)/5.0, colors[angle / 30], TFT_WHITE);
      tft.drawArc(x, y, radius, radius - rDelta, angle, angle + 30, wheelColor, TFT_BLACK, smooth);
    }
    smooth = false;  // Only outer ring is smooth
  }

  while (1) delay(100);
}
