/***************************************************************************************
** Code for the GFX button UI element
** Grabbed from Adafruit_GFX library and enhanced to handle any label font
***************************************************************************************/
TFT_eSPI_Button::TFT_eSPI_Button(void) {
  _gfx       = nullptr;
  _xd        = 0;
  _yd        = 0;
  _textdatum = MC_DATUM;
  _label[9]  = '\0';
  currstate = false;
  laststate = false;
}

// Classic initButton() function: pass center & size
void TFT_eSPI_Button::initButton(
 TFT_eSPI *gfx, short x, short y, unsigned short w, unsigned short h,
 unsigned short outline, unsigned short fill, unsigned short textcolor,
 char *label, unsigned char textsize)
{
  // Tweak arguments and pass to the newer initButtonUL() function...
  initButtonUL(gfx, x - (w / 2), y - (h / 2), w, h, outline, fill,
    textcolor, label, textsize);
}

// Newer function instead accepts upper-left corner & size
void TFT_eSPI_Button::initButtonUL(
 TFT_eSPI *gfx, short x1, short y1, unsigned short w, unsigned short h,
 unsigned short outline, unsigned short fill, unsigned short textcolor,
 char *label, unsigned char textsize)
{
  _x1           = x1;
  _y1           = y1;
  _w            = w;
  _h            = h;
  _outlinecolor = outline;
  _fillcolor    = fill;
  _textcolor    = textcolor;
  _textsize     = textsize;
  _gfx          = gfx;
  strncpy(_label, label, 9);
}

// Adjust text datum and x, y deltas
void TFT_eSPI_Button::setLabelDatum(short x_delta, short y_delta, unsigned char datum)
{
  _xd        = x_delta;
  _yd        = y_delta;
  _textdatum = datum;
}

void TFT_eSPI_Button::drawButton(bool inverted, String long_name) {
  unsigned short fill, outline, text;

  if(!inverted) {
    fill    = _fillcolor;
    outline = _outlinecolor;
    text    = _textcolor;
  } else {
    fill    = _textcolor;
    outline = _outlinecolor;
    text    = _fillcolor;
  }

  unsigned char r = min(_w, _h) / 4; // Corner radius
  _gfx->fillRoundRect(_x1, _y1, _w, _h, r, fill);
  _gfx->drawRoundRect(_x1, _y1, _w, _h, r, outline);

  if (_gfx->textfont == 255) {
    _gfx->setCursor(_x1 + (_w / 8),
                    _y1 + (_h / 4));
    _gfx->setTextColor(text);
    _gfx->setTextSize(_textsize);
    _gfx->print(_label);
  }
  else {
    _gfx->setTextColor(text, fill);
    _gfx->setTextSize(_textsize);

    unsigned char tempdatum = _gfx->getTextDatum();
    _gfx->setTextDatum(_textdatum);
    unsigned short tempPadding = _gfx->getTextPadding();
    _gfx->setTextPadding(0);

    if (long_name == "")
      _gfx->drawString(_label, _x1 + (_w/2) + _xd, _y1 + (_h/2) - 4 + _yd);
    else
      _gfx->drawString(long_name, _x1 + (_w/2) + _xd, _y1 + (_h/2) - 4 + _yd);

    _gfx->setTextDatum(tempdatum);
    _gfx->setTextPadding(tempPadding);
  }
}

bool TFT_eSPI_Button::contains(short x, short y) {
  return ((x >= _x1) && (x < (_x1 + _w)) &&
          (y >= _y1) && (y < (_y1 + _h)));
}

void TFT_eSPI_Button::press(bool p) {
  laststate = currstate;
  currstate = p;
}

bool TFT_eSPI_Button::isPressed()    { return currstate; }
bool TFT_eSPI_Button::justPressed()  { return (currstate && !laststate); }
bool TFT_eSPI_Button::justReleased() { return (!currstate && laststate); }
