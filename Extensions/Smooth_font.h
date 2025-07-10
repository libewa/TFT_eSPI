 // Coded by Bodmer 10/2/18, see license in root directory.
 // This is part of the TFT_eSPI class and is associated with anti-aliased font functions

 public:

  // These are for the new anti-aliased fonts
  void     loadFont(const unsigned char array[]);
#ifdef FONT_FS_AVAILABLE
  void     loadFont(String fontName, fs::FS &ffs);
#endif
  void     loadFont(String fontName, bool flash = true);
  void     unloadFont( void );
  bool     getUnicodeIndex(unsigned short unicode, unsigned short *index);

  virtual void drawGlyph(unsigned short code);

  void     showFont(unsigned int td);

 // This is for the whole font
  typedef struct
  {
    const unsigned char* gArray;           //array start pointer
    unsigned short gCount;                 // Total number of characters
    unsigned short yAdvance;               // Line advance
    unsigned short spaceWidth;             // Width of a space character
    short  ascent;                 // Height of top of 'd' above baseline, other characters may be taller
    short  descent;                // Offset to bottom of 'p', other characters may have a larger descent
    unsigned short maxAscent;              // Maximum ascent found in font
    unsigned short maxDescent;             // Maximum descent found in font
  } fontMetrics;

fontMetrics gFont = { nullptr, 0, 0, 0, 0, 0, 0, 0 };

  // These are for the metrics for each individual glyph (so we don't need to seek this in file and waste time)
  unsigned short* gUnicode = NULL;  //UTF-16 code, the codes are searched so do not need to be sequential
  unsigned char*  gHeight = NULL;   //cheight
  unsigned char*  gWidth = NULL;    //cwidth
  unsigned char*  gxAdvance = NULL; //setWidth
  short*  gdY = NULL;       //topExtent
  char*   gdX = NULL;       //leftExtent
  unsigned int* gBitmap = NULL;   //file pointer to greyscale bitmap

  bool     fontLoaded = false; // Flags when a anti-aliased font is loaded

#ifdef FONT_FS_AVAILABLE
  fs::File fontFile;
  fs::FS   &fontFS  = SPIFFS;
  bool     spiffs   = true;
  bool     fs_font = false;    // For ESP32/8266 use smooth font file or FLASH (PROGMEM) array

#else
  bool     fontFile = true;
#endif

  private:

  void     loadMetrics(void);
  unsigned int readInt32(void);

  unsigned char* fontPtr = nullptr;

