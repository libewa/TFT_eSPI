// Example for drawArc function. This is intended for arc based meters.
// (See arcMeter example)

// Draws arcs without smooth ends, suitable for dynamically changing arc
// angles to avoid residual anti-alias pixels at the arc segment joints.

// The sides of the arc can optionally be smooth or not. Smooth arcs have
// a much better appearance, especially at small sizes.

#include <TFT_eSPI.h>       // Include the graphics library
TFT_eSPI tft = TFT_eSPI();  // Create object "tft"

// -------------------------------------------------------------------------
// Setup
// -------------------------------------------------------------------------
void setup(void) {
  Serial.begin(115200);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
}

// -------------------------------------------------------------------------
// Main loop
// -------------------------------------------------------------------------
void loop()
{
  static unsigned int count = 0;

  unsigned short fg_color = random(0x10000);
  unsigned short bg_color = TFT_BLACK;       // This is the background colour used for smoothing (anti-aliasing)

  unsigned short x = random(tft.width());  // Position of centre of arc
  unsigned short y = random(tft.height());

  unsigned char radius       = random(20, tft.width() / 4); // Outer arc radius
  unsigned char thickness    = random(1, radius / 4);     // Thickness
  unsigned char inner_radius = radius - thickness;        // Calculate inner radius (can be 0 for circle segment)

  // 0 degrees is at 6 o'clock position
  // Arcs are drawn clockwise from start_angle to end_angle
  // Start angle can be greater than end angle, the arc will then be drawn through 0 degrees
  unsigned short start_angle = random(361); // Start angle must be in range 0 to 360
  unsigned short end_angle   = random(361); // End angle must be in range 0 to 360

  bool smooth = random(2); // true = smooth sides, false = no smooth sides

  tft.drawArc(x, y, radius, inner_radius, start_angle, end_angle, fg_color, bg_color, smooth);

  count++;
  if (count < 30) delay(500); // After 15s draw as fast as possible!
}
