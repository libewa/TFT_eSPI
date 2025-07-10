// Bodmer's BMP image rendering function

void drawBmp(const char *filename, short x, short y) {

  if ((x >= tft.width()) || (y >= tft.height())) return;

  fs::File bmpFS;

  // Open requested file on SD card
  bmpFS = SPIFFS.open(filename, "r");

  if (!bmpFS)
  {
    Serial.print("File not found");
    return;
  }

  unsigned int seekOffset;
  unsigned short w, h, row, col;
  unsigned char  r, g, b;

  unsigned int startTime = millis();

  if (read16(bmpFS) == 0x4D42)
  {
    read32(bmpFS);
    read32(bmpFS);
    seekOffset = read32(bmpFS);
    read32(bmpFS);
    w = read32(bmpFS);
    h = read32(bmpFS);

    if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) && (read32(bmpFS) == 0))
    {
      y += h - 1;

      bool oldSwapBytes = tft.getSwapBytes();
      tft.setSwapBytes(true);
      bmpFS.seek(seekOffset);

      unsigned short padding = (4 - ((w * 3) & 3)) & 3;
      unsigned char lineBuffer[w * 3 + padding];

      for (row = 0; row < h; row++) {
        
        bmpFS.read(lineBuffer, sizeof(lineBuffer));
        unsigned char*  bptr = lineBuffer;
        unsigned short* tptr = (unsigned short*)lineBuffer;
        // Convert 24 to 16-bit colours
        for (unsigned short col = 0; col < w; col++)
        {
          b = *bptr++;
          g = *bptr++;
          r = *bptr++;
          *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        }

        // Push the pixel row to screen, pushImage will crop the line if needed
        // y is decremented as the BMP image is drawn bottom up
        tft.pushImage(x, y--, w, 1, (unsigned short*)lineBuffer);
      }
      tft.setSwapBytes(oldSwapBytes);
      Serial.print("Loaded in "); Serial.print(millis() - startTime);
      Serial.println(" ms");
    }
    else Serial.println("BMP format not recognized.");
  }
  bmpFS.close();
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

unsigned short read16(fs::File &f) {
  unsigned short result;
  ((unsigned char *)&result)[0] = f.read(); // LSB
  ((unsigned char *)&result)[1] = f.read(); // MSB
  return result;
}

unsigned int read32(fs::File &f) {
  unsigned int result;
  ((unsigned char *)&result)[0] = f.read(); // LSB
  ((unsigned char *)&result)[1] = f.read();
  ((unsigned char *)&result)[2] = f.read();
  ((unsigned char *)&result)[3] = f.read(); // MSB
  return result;
}

