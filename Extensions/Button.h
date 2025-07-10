/***************************************************************************************
// The following button class has been ported over from the Adafruit_GFX library so
// should be compatible.
// A slightly different implementation in this TFT_eSPI library allows the button
// legends to be in any font, allow longer labels and to adjust text positioning
// within button
***************************************************************************************/

class TFT_eSPI_Button
{
 public:
  TFT_eSPI_Button(void);
  // "Classic" initButton() uses centre & size
  void     initButton(TFT_eSPI *gfx, short x, short y,
  unsigned short w, unsigned short h, unsigned short outline, unsigned short fill,
  unsigned short textcolor, char *label, unsigned char textsize);

  // New/alt initButton() uses upper-left corner & size
  void     initButtonUL(TFT_eSPI *gfx, short x1, short y1,
  unsigned short w, unsigned short h, unsigned short outline, unsigned short fill,
  unsigned short textcolor, char *label, unsigned char textsize);
  
  // Adjust text datum and x, y deltas
  void     setLabelDatum(short x_delta, short y_delta, unsigned char datum = MC_DATUM);
  
  void     drawButton(bool inverted = false, String long_name = "");
  bool     contains(short x, short y);

  void     press(bool p);
  bool     isPressed();
  bool     justPressed();
  bool     justReleased();

 private:
  TFT_eSPI *_gfx;
  short  _x1, _y1; // Coordinates of top-left corner of button
  short  _xd, _yd; // Button text datum offsets (wrt centre of button)
  unsigned short _w, _h;   // Width and height of button
  unsigned char  _textsize, _textdatum; // Text size multiplier and text datum for button
  unsigned short _outlinecolor, _fillcolor, _textcolor;
  char     _label[10]; // Button text is 9 chars maximum unless long_name used

  bool  currstate, laststate; // Button states
};
