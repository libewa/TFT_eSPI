// Here are the callback functions that the decPNG library
// will use to open files, fetch data and close the file.

File pngfile;

void * pngOpen(const char *filename, int *size) {
  Serial.printf("Attempting to open %s\n", filename);
  pngfile = FileSys.open(filename, "r");
  *size = pngfile.size();
  return &pngfile;
}

void pngClose(void *handle) {
  File pngfile = *((File*)handle);
  if (pngfile) pngfile.close();
}

int pngRead(PNGFILE *page, unsigned char *buffer, int length) {
  if (!pngfile) return 0;
  page = page; // Avoid warning
  return pngfile.read(buffer, length);
}

int pngSeek(PNGFILE *page, int position) {
  if (!pngfile) return 0;
  page = page; // Avoid warning
  return pngfile.seek(position);
}
