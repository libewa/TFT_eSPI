/***************************************************************************************
// The following class creates Sprites in RAM, graphics can then be drawn in the Sprite
// and rendered quickly onto the TFT screen. The class inherits the graphics functions
// from the TFT_eSPI class. Some functions are overridden by this class so that the
// graphics are written to the Sprite rather than the TFT.
***************************************************************************************/

class TFT_eSprite : public TFT_eSPI {

 public:

  explicit TFT_eSprite(TFT_eSPI *tft);
  ~TFT_eSprite(void);

           // Create a sprite of width x height pixels, return a pointer to the RAM area
           // Sketch can cast returned value to (unsigned short*) for 16-bit depth if needed
           // RAM required is:
           //  - 1 bit per pixel for 1 bit colour depth
           //  - 1 nibble per pixel for 4-bit colour (with palette table)
           //  - 1 byte per pixel for 8-bit colour (332 RGB format)
           //  - 2 bytes per pixel for 16-bit color depth (565 RGB format)
  void*    createSprite(short width, short height, unsigned char frames = 1);

           // Returns a pointer to the sprite or nullptr if not created, user must cast to pointer type
  void*    getPointer(void);

           // Returns true if sprite has been created
  bool     created(void);

           // Delete the sprite to free up the RAM
  void     deleteSprite(void);

           // Select the frame buffer for graphics write (for 2 colour ePaper and DMA toggle buffer)
           // Returns a pointer to the Sprite frame buffer
  void*    frameBuffer(char f);
  
           // Set or get the colour depth to 1, 4, 8 or 16 bits. Can be used to change depth an existing
           // sprite, but clears it to black, returns a new pointer if sprite is re-created.
  void*    setColorDepth(char b);
  char   getColorDepth(void);

           // Set the palette for a 4-bit depth sprite.  Only the first 16 colours in the map are used.
  void     createPalette(unsigned short *palette = nullptr, unsigned char colors = 16);       // Palette in RAM
  void     createPalette(const unsigned short *palette = nullptr, unsigned char colors = 16); // Palette in FLASH

           // Set a single palette index to the given color
  void     setPaletteColor(unsigned char index, unsigned short color);

           // Get the color at the given palette index
  unsigned short getPaletteColor(unsigned char index);

           // Set foreground and background colours for 1 bit per pixel Sprite
  void     setBitmapColor(unsigned short fg, unsigned short bg);

           // Draw a single pixel at x,y
  void     drawPixel(int x, int y, unsigned int color);

           // Draw a single character in the GLCD or GFXFF font
  void     drawChar(int x, int y, unsigned short c, unsigned int color, unsigned int bg, unsigned char size),

           // Fill Sprite with a colour
           fillSprite(unsigned int color),

           // Define a window to push 16-bit colour pixels into in a raster order
           // Colours are converted to the set Sprite colour bit depth
           setWindow(int x0, int y0, int x1, int y1),
           // Push a color (aka singe pixel) to the sprite's set window area
           pushColor(unsigned short color),
           // Push len colors (pixels) to the sprite's set window area
           pushColor(unsigned short color, unsigned int len),
           // Push a pixel pre-formatted as a 1, 4, 8 or 16-bit colour (avoids conversion overhead)
           writeColor(unsigned short color),

           // Set the scroll zone, top left corner at x,y with defined width and height
           // The colour (optional, black is default) is used to fill the gap after the scroll
           setScrollRect(int x, int y, int w, int h, unsigned short color = TFT_BLACK),
           // Scroll the defined zone dx,dy pixels. Negative values left,up, positive right,down
           // dy is optional (default is 0, so no up/down scroll).
           // The sprite coordinate frame does not move because pixels are moved
           scroll(short dx, short dy = 0),

           // Draw lines
           drawLine(int x0, int y0, int x1, int y1, unsigned int color),
           drawFastVLine(int x, int y, int h, unsigned int color),
           drawFastHLine(int x, int y, int w, unsigned int color),

           // Fill a rectangular area with a color (aka draw a filled rectangle)
           fillRect(int x, int y, int w, int h, unsigned int color);

           // Set the coordinate rotation of the Sprite (for 1bpp Sprites only)
           // Note: this uses coordinate rotation and is primarily for ePaper which does not support
           // CGRAM rotation (like TFT drivers do) within the displays internal hardware
  void     setRotation(unsigned char rotation);
  unsigned char  getRotation(void);

           // Push a rotated copy of Sprite to TFT with optional transparent colour
  bool     pushRotated(short angle, unsigned int transp = 0x00FFFFFF);
           // Push a rotated copy of Sprite to another different Sprite with optional transparent colour
  bool     pushRotated(TFT_eSprite *spr, short angle, unsigned int transp = 0x00FFFFFF);

           // Get the TFT bounding box for a rotated copy of this Sprite
  bool     getRotatedBounds(short angle, short *min_x, short *min_y, short *max_x, short *max_y);
           // Get the destination Sprite bounding box for a rotated copy of this Sprite
  bool     getRotatedBounds(TFT_eSprite *spr, short angle, short *min_x, short *min_y,
                                                             short *max_x, short *max_y);
           // Bounding box support function
  void     getRotatedBounds(short angle, short w, short h, short xp, short yp,
                            short *min_x, short *min_y, short *max_x, short *max_y);

           // Read the colour of a pixel at x,y and return value in 565 format 
  unsigned short readPixel(int x0, int y0);

           // return the numerical value of the pixel at x,y (used when scrolling)
           // 16bpp = colour, 8bpp = byte, 4bpp = colour index, 1bpp = 1 or 0
  unsigned short readPixelValue(int x, int y);

           // Write an image (colour bitmap) to the sprite.
  void     pushImage(int x0, int y0, int w, int h, unsigned short *data, unsigned char sbpp = 0);
  void     pushImage(int x0, int y0, int w, int h, const unsigned short *data);

           // Push the sprite to the TFT screen, this fn calls pushImage() in the TFT class.
           // Optionally a "transparent" colour can be defined, pixels of that colour will not be rendered
  void     pushSprite(int x, int y);
  void     pushSprite(int x, int y, unsigned short transparent);

           // Push a windowed area of the sprite to the TFT at tx, ty
  bool     pushSprite(int tx, int ty, int sx, int sy, int sw, int sh);

           // Push the sprite to another sprite at x,y. This fn calls pushImage() in the destination sprite (dspr) class.
  bool     pushToSprite(TFT_eSprite *dspr, int x, int y);
  bool     pushToSprite(TFT_eSprite *dspr, int x, int y, unsigned short transparent);

           // Draw a single character in the selected font
  short  drawChar(unsigned short uniCode, int x, int y, unsigned char font),
           drawChar(unsigned short uniCode, int x, int y);

           // Return the width and height of the sprite
  short  width(void),
           height(void);

           // Functions associated with anti-aliased fonts
           // Draw a single Unicode character using the loaded font
  void     drawGlyph(unsigned short code);
           // Print string to sprite using loaded font at cursor position
  void     printToSprite(String string);
           // Print char array to sprite using loaded font at cursor position
  void     printToSprite(char *cbuffer, unsigned short len);
           // Print indexed glyph to sprite using loaded font at x,y
  short  printToSprite(short x, short y, unsigned short index);

 private:

  TFT_eSPI *_tft;

           // Reserve memory for the Sprite and return a pointer
  void*    callocSprite(short width, short height, unsigned char frames = 1);

           // Override the non-inlined TFT_eSPI functions
  void     begin_nin_write(void) { ; }
  void     end_nin_write(void) { ; }

 protected:

  unsigned char  _bpp;     // bits per pixel (1, 4, 8 or 16)
  unsigned short *_img;    // pointer to 16-bit sprite
  unsigned char  *_img8;   // pointer to  1 and 8-bit sprite frame 1 or frame 2
  unsigned char  *_img4;   // pointer to  4-bit sprite (uses color map)
  unsigned char  *_img8_1; // pointer to frame 1
  unsigned char  *_img8_2; // pointer to frame 2

  unsigned short *_colorMap; // color map pointer: 16 entries, used with 4-bit color map.

  int  _sinra;   // Sine of rotation angle in fixed point
  int  _cosra;   // Cosine of rotation angle in fixed point

  bool     _created; // A Sprite has been created and memory reserved
  bool     _gFont = false; 

  int  _xs, _ys, _xe, _ye, _xptr, _yptr; // for setWindow
  int  _sx, _sy; // x,y for scroll zone
  unsigned int _sw, _sh; // w,h for scroll zone
  unsigned int _scolor;  // gap fill colour for scroll zone

  int  _iwidth, _iheight; // Sprite memory image bit width and height (swapped during rotations)
  int  _dwidth, _dheight; // Real sprite width and height (for <8bpp Sprites)
  int  _bitwidth;         // Sprite image bit width for drawPixel (for <8bpp Sprites, not swapped)

};
