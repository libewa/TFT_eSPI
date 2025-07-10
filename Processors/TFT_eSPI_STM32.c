        ////////////////////////////////////////////////////
        // TFT_eSPI Driver functions for STM32 processors //
        ////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
// Global variables
////////////////////////////////////////////////////////////////////////////////////////

#if defined (TFT_PARALLEL_8_BIT)
  // No globals
#else
  // Use STM32 default SPI port
  #if !defined (TFT_MOSI) || !defined (TFT_MISO) || !defined (TFT_SCLK)
    SPIClass& spi = SPI;
  #else
    SPIClass spi(TFT_MOSI, TFT_MISO, TFT_SCLK);
  #endif
  // SPI HAL peripheral handle
  SPI_HandleTypeDef spiHal;
#endif

#ifdef STM32_DMA
  // DMA HAL handle
  DMA_HandleTypeDef dmaHal;
#endif

////////////////////////////////////////////////////////////////////////////////////////
#if defined (TFT_SDA_READ) && !defined (TFT_PARALLEL_8_BIT)
////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************############# UNTESTED ###################
** Function name:           tft_Read_8
** Description:             STM32 software SPI to read bidirectional SDA line
***************************************************************************************/
unsigned char TFT_eSPI::tft_Read_8(void)
{
  unsigned char  ret = 0;
  unsigned int reg = 0;

  for (unsigned char i = 0; i < 8; i++) {  // read results
    ret <<= 1;
    SCLK_L;
    if (digitalRead(TFT_MOSI)) ret |= 1;
    SCLK_H;
  }

  return ret;
}

/***************************************************************************************############# UNTESTED ###################
** Function name:           beginSDA
** Description:             Detach SPI from pin to permit software SPI
***************************************************************************************/
void TFT_eSPI::begin_SDA_Read(void)
{
  // Release configured SPI port for SDA read
  spi.end();// Code missing here!                                                      <<<<<<<<<<<<<<Missing code<<<<<<<<<<<<<<<<<
}

/***************************************************************************************############# UNTESTED ###################
** Function name:           endSDA
** Description:             Attach SPI pins after software SPI
***************************************************************************************/
void TFT_eSPI::end_SDA_Read(void)
{
  // Configure SPI port ready for next TFT access
  spi.begin();// Code missing here!                                                   <<<<<<<<<<<<<<Missing code<<<<<<<<<<<<<<<<<
}

////////////////////////////////////////////////////////////////////////////////////////
#endif // #if defined (TFT_SDA_READ)
////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////
#if defined (TFT_PARALLEL_8_BIT) // Code for STM32 8-bit parallel
////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           pushBlock - for ESP32 and parallel display
** Description:             Write a block of pixels of the same colour
***************************************************************************************/
void TFT_eSPI::pushBlock(unsigned short color, unsigned int len){
    // Loop unrolling improves speed dramatically graphics test  0.634s => 0.374s
    while (len>31) {
    #if !defined (SSD1963_DRIVER)
      // 32D macro writes 16 bits twice
      tft_Write_32D(color); tft_Write_32D(color);
      tft_Write_32D(color); tft_Write_32D(color);
      tft_Write_32D(color); tft_Write_32D(color);
      tft_Write_32D(color); tft_Write_32D(color);
      tft_Write_32D(color); tft_Write_32D(color);
      tft_Write_32D(color); tft_Write_32D(color);
      tft_Write_32D(color); tft_Write_32D(color);
      tft_Write_32D(color); tft_Write_32D(color);
    #else
      tft_Write_16(color); tft_Write_16(color); tft_Write_16(color); tft_Write_16(color);
      tft_Write_16(color); tft_Write_16(color); tft_Write_16(color); tft_Write_16(color);
      tft_Write_16(color); tft_Write_16(color); tft_Write_16(color); tft_Write_16(color);
      tft_Write_16(color); tft_Write_16(color); tft_Write_16(color); tft_Write_16(color);
      tft_Write_16(color); tft_Write_16(color); tft_Write_16(color); tft_Write_16(color);
      tft_Write_16(color); tft_Write_16(color); tft_Write_16(color); tft_Write_16(color);
      tft_Write_16(color); tft_Write_16(color); tft_Write_16(color); tft_Write_16(color);
      tft_Write_16(color); tft_Write_16(color); tft_Write_16(color); tft_Write_16(color);
    #endif
      len-=32;
    }

    while (len>7) {
    #if !defined (SSD1963_DRIVER)
      tft_Write_32D(color); tft_Write_32D(color);
      tft_Write_32D(color); tft_Write_32D(color);
    #else
      tft_Write_16(color); tft_Write_16(color); tft_Write_16(color); tft_Write_16(color);
      tft_Write_16(color); tft_Write_16(color); tft_Write_16(color); tft_Write_16(color);
    #endif
      len-=8;
    }

  while (len--) {tft_Write_16(color);}
}


/***************************************************************************************
** Function name:           pushPixels - for ESP32 and parallel display
** Description:             Write a sequence of pixels
***************************************************************************************/
void TFT_eSPI::pushPixels(const void* data_in, unsigned int len){

  unsigned short *data = (unsigned short*)data_in;

  if(_swapBytes) {
    while (len>1) {tft_Write_16(*data); data++; tft_Write_16(*data); data++; len -=2;}
    if (len) {tft_Write_16(*data);}
    return;
  }

  while (len>1) {tft_Write_16S(*data); data++; tft_Write_16S(*data); data++; len -=2;}
  if (len) {tft_Write_16S(*data);}
}


/***************************************************************************************
** Function name:           GPIO direction control  - supports class functions
** Description:             Set parallel bus to INPUT or OUTPUT
***************************************************************************************/
void TFT_eSPI::busDir(unsigned int mask, unsigned char mode)
{
#if defined (STM_PORTA_DATA_BUS)
  #if defined (STM32F1xx)
    if (mode == OUTPUT) GPIOA->CRL = 0x33333333;
    else GPIOA->CRL = 0x88888888;
  #else
    if (mode == OUTPUT) GPIOA->MODER = (GPIOA->MODER & 0xFFFF0000) | 0x00005555;
    else GPIOA->MODER &= 0xFFFF0000;
  #endif
#elif defined (STM_PORTB_DATA_BUS)
  #if defined (STM32F1xx)
    if (mode == OUTPUT) GPIOB->CRL = 0x33333333;
    else GPIOB->CRL = 0x88888888;
  #else
    if (mode == OUTPUT) GPIOB->MODER = (GPIOB->MODER & 0xFFFF0000) | 0x00005555;
    else GPIOB->MODER &= 0xFFFF0000;
  #endif
#elif defined (STM_PORTC_DATA_BUS)
  #if defined (STM32F1xx)
    if (mode == OUTPUT) GPIOC->CRL = 0x33333333;
    else GPIOC->CRL = 0x88888888;
  #else
    if (mode == OUTPUT) GPIOC->MODER = (GPIOC->MODER & 0xFFFF0000) | 0x00005555;
    else GPIOC->MODER &= 0xFFFF0000;
  #endif
#elif defined (STM_PORTD_DATA_BUS)
  #if defined (STM32F1xx)
    if (mode == OUTPUT) GPIOD->CRL = 0x33333333;
    else GPIOD->CRL = 0x88888888;
  #else
    if (mode == OUTPUT) GPIOD->MODER = (GPIOD->MODER & 0xFFFF0000) | 0x00005555;
    else GPIOD->MODER &= 0xFFFF0000;
  #endif
#else
  if (mode == OUTPUT) {
    LL_GPIO_SetPinMode(D0_PIN_PORT, D0_PIN_MASK, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(D1_PIN_PORT, D1_PIN_MASK, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(D2_PIN_PORT, D2_PIN_MASK, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(D3_PIN_PORT, D3_PIN_MASK, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(D4_PIN_PORT, D4_PIN_MASK, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(D5_PIN_PORT, D5_PIN_MASK, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(D6_PIN_PORT, D6_PIN_MASK, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(D7_PIN_PORT, D7_PIN_MASK, LL_GPIO_MODE_OUTPUT);
  }
  else {
    LL_GPIO_SetPinMode(D0_PIN_PORT, D0_PIN_MASK, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinMode(D1_PIN_PORT, D1_PIN_MASK, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinMode(D2_PIN_PORT, D2_PIN_MASK, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinMode(D3_PIN_PORT, D3_PIN_MASK, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinMode(D4_PIN_PORT, D4_PIN_MASK, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinMode(D5_PIN_PORT, D5_PIN_MASK, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinMode(D6_PIN_PORT, D6_PIN_MASK, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinMode(D7_PIN_PORT, D7_PIN_MASK, LL_GPIO_MODE_INPUT);
  }
#endif
}


/***************************************************************************************
** Function name:           GPIO direction control  - supports class functions
** Description:             Set STM32 GPIO pin to input or output (set high) ASAP
***************************************************************************************/
void TFT_eSPI::gpioMode(unsigned char gpio, unsigned char mode)
{
  PinName pn = digitalPinToPinName(gpio);
  // Push-pull output with no pullup
  if (mode == OUTPUT) pin_function(pn, STM_PIN_DATA(STM_MODE_OUTPUT_PP, GPIO_NOPULL, 0));
  // Input with pullup
  else pin_function(pn, STM_PIN_DATA(STM_MODE_INPUT, GPIO_PULLUP, 0));
}

/***************************************************************************************############# UNTESTED ###################
** Function name:           read byte  - supports class functions
** Description:             Read a byte - parallel bus only
***************************************************************************************/
unsigned char TFT_eSPI::readByte(void)
{
  unsigned char b = 0;

  RD_L;
#if defined (STM_PORTA_DATA_BUS)
  b = GPIOA->IDR;
  b = GPIOA->IDR;
  b = GPIOA->IDR;
  b = (GPIOA->IDR) & 0xFF;
#elif defined (STM_PORTB_DATA_BUS)
  b = GPIOB->IDR;
  b = GPIOB->IDR;
  b = GPIOB->IDR;
  b = (GPIOB->IDR) & 0xFF;
#elif defined (STM_PORTC_DATA_BUS)
  b = GPIOC->IDR;
  b = GPIOC->IDR;
  b = GPIOC->IDR;
  b = (GPIOC->IDR) & 0xFF;
#elif defined (STM_PORTD_DATA_BUS)
  b = GPIOD->IDR;
  b = GPIOD->IDR;
  b = GPIOD->IDR;
  b = (GPIOD->IDR) & 0xFF;
#else
  b  = RD_TFT_D0 | RD_TFT_D0 | RD_TFT_D0 | RD_TFT_D0; //Delay for bits to settle

  b  = RD_TFT_D0 | RD_TFT_D1 | RD_TFT_D2 | RD_TFT_D3;
  b |= RD_TFT_D4 | RD_TFT_D5 | RD_TFT_D6 | RD_TFT_D7;
#endif
  RD_H;

  return b;
}

////////////////////////////////////////////////////////////////////////////////////////
#elif defined (RPI_WRITE_STROBE)  // For RPi TFT with write strobe                        ############# UNTESTED ###################
////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           pushBlock - for ESP32 or STM32 RPi TFT
** Description:             Write a block of pixels of the same colour
***************************************************************************************/
void TFT_eSPI::pushBlock(unsigned short color, unsigned int len)
{
  if(len) { tft_Write_16(color); len--; }
  while(len--) {WR_L; WR_H;}
}

/***************************************************************************************
** Function name:           pushPixels - for ESP32 or STM32 RPi TFT
** Description:             Write a sequence of pixels
***************************************************************************************/
void TFT_eSPI::pushPixels(const void* data_in, unsigned int len)
{
  unsigned short *data = (unsigned short*)data_in;

  if (_swapBytes) while ( len-- ) { tft_Write_16S(*data); data++;}
  else while ( len-- ) {tft_Write_16(*data); data++;}
}

////////////////////////////////////////////////////////////////////////////////////////
#elif defined (SPI_18BIT_DRIVER) // SPI 18-bit colour
////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           pushBlock - for STM32 and 3 byte RGB display
** Description:             Write a block of pixels of the same colour
***************************************************************************************/
#define BUF_SIZE 240*3
void TFT_eSPI::pushBlock(unsigned short color, unsigned int len)
{
  //unsigned char col[BUF_SIZE];
  // Always using swapped bytes is a peculiarity of this function...
  //color = color>>8 | color<<8;
  unsigned char r = (color & 0xF800)>>8; // Red
  unsigned char g = (color & 0x07E0)>>3; // Green
  unsigned char b = (color & 0x001F)<<3; // Blue

  while(len--) {
    TX_FIFO = (r);
    TX_FIFO = (g);
    TX_FIFO = (b);
  }
   SPI_BUSY_CHECK;
}
/***************************************************************************************
** Function name:           pushPixels - for STM32 and 3 byte RGB display
** Description:             Write a sequence of pixels
***************************************************************************************/
void TFT_eSPI::pushPixels(const void* data_in, unsigned int len)
{
  unsigned short *data = (unsigned short*)data_in;

  if(!_swapBytes) {
    while ( len-- ) {
      // Split out the colours
      TX_FIFO = (*data & 0xF8); // Red
      TX_FIFO = (*data & 0xE000)>>11 | (*data & 0x07)<<5; // Green
      TX_FIFO = (*data & 0x1F00)>>5; // Blue
      data++;
    }
  }
  else {
    while ( len-- ) {
      // Split out the colours
      TX_FIFO = (*data & 0xF800)>>8; // Red
      TX_FIFO = (*data & 0x07E0)>>3; // Green
      TX_FIFO = (*data & 0x001F)<<3; // Blue
      data++;
    }
  }
  SPI_BUSY_CHECK;
}

////////////////////////////////////////////////////////////////////////////////////////
#else //                   Standard SPI 16-bit colour TFT                                                 All Tested
////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           pushBlock - for STM32
** Description:             Write a block of pixels of the same colour
***************************************************************************************/
#define BUF_SIZE 480
/*
void TFT_eSPI::pushBlock(unsigned short color, unsigned int len)
{
  unsigned short col[BUF_SIZE];
  // Always using swapped bytes is a peculiarity of this function...
  unsigned short swapColor = color>>8 | color<<8;
  if  (len<BUF_SIZE) {
    for (unsigned int i = 0; i < len; i++) col[i] = swapColor;
    HAL_SPI_Transmit(&spiHal, (unsigned char*)col, len<<1, HAL_MAX_DELAY);
    return;
  }

  for (unsigned int i = 0; i < BUF_SIZE; i++) col[i] = swapColor;
  do {
    HAL_SPI_Transmit(&spiHal, (unsigned char*)col, BUF_SIZE<<1, HAL_MAX_DELAY);
    len -= BUF_SIZE;
  } while ( len>=BUF_SIZE ) ;
  // Send remaining pixels
  if (len) HAL_SPI_Transmit(&spiHal, (unsigned char*)col, len<<1, HAL_MAX_DELAY);
}
 //*/
void TFT_eSPI::pushBlock(unsigned short color, unsigned int len){
    // Loop unrolling improves speed dramatically graphics test  0.634s => 0.374s
    while (len>31) {
    #if !defined (SSD1963_DRIVER)
      // 32D macro writes 16 bits twice
      tft_Write_32D(color); tft_Write_32D(color);
      tft_Write_32D(color); tft_Write_32D(color);
      tft_Write_32D(color); tft_Write_32D(color);
      tft_Write_32D(color); tft_Write_32D(color);
      tft_Write_32D(color); tft_Write_32D(color);
      tft_Write_32D(color); tft_Write_32D(color);
      tft_Write_32D(color); tft_Write_32D(color);
      tft_Write_32D(color); tft_Write_32D(color);
    #else
      tft_Write_16(color); tft_Write_16(color); tft_Write_16(color); tft_Write_16(color);
      tft_Write_16(color); tft_Write_16(color); tft_Write_16(color); tft_Write_16(color);
      tft_Write_16(color); tft_Write_16(color); tft_Write_16(color); tft_Write_16(color);
      tft_Write_16(color); tft_Write_16(color); tft_Write_16(color); tft_Write_16(color);
      tft_Write_16(color); tft_Write_16(color); tft_Write_16(color); tft_Write_16(color);
      tft_Write_16(color); tft_Write_16(color); tft_Write_16(color); tft_Write_16(color);
      tft_Write_16(color); tft_Write_16(color); tft_Write_16(color); tft_Write_16(color);
      tft_Write_16(color); tft_Write_16(color); tft_Write_16(color); tft_Write_16(color);
    #endif
      len-=32;
    }

    while (len>7) {
    #if !defined (SSD1963_DRIVER)
      tft_Write_32D(color); tft_Write_32D(color);
      tft_Write_32D(color); tft_Write_32D(color);
    #else
      tft_Write_16(color); tft_Write_16(color); tft_Write_16(color); tft_Write_16(color);
      tft_Write_16(color); tft_Write_16(color); tft_Write_16(color); tft_Write_16(color);
    #endif
      len-=8;
    }

  while (len--) {tft_Write_16(color);}
}


/***************************************************************************************
** Function name:           pushPixels - for STM32
** Description:             Write a sequence of pixels
***************************************************************************************/
void TFT_eSPI::pushPixels(const void* data_in, unsigned int len)
{
  unsigned short *data = (unsigned short*)data_in;

  if(_swapBytes) {
    while ( len-- ) {
      TX_FIFO = (unsigned char)(*data>>8);
      TX_FIFO = (unsigned char)(*data);
      data++;
    }
  }
  else {
    while ( len-- ) {
      // Split out the colours
      TX_FIFO = (unsigned char)(*data);
      TX_FIFO = (unsigned char)(*data>>8);
      data++;
    }
  }
  SPI_BUSY_CHECK;
}

////////////////////////////////////////////////////////////////////////////////////////
#endif // End of display interface specific functions
////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////
#if defined STM32_DMA && !defined (TFT_PARALLEL_8_BIT) //       DMA FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           dmaBusy
** Description:             Check if DMA is busy (usefully non-blocking!)
***************************************************************************************/
// Use while( tft.dmaBusy() ) {Do-something-useful;}"
bool TFT_eSPI::dmaBusy(void)
{
  //return (dmaHal.State == HAL_DMA_STATE_BUSY);  // Do not use, SPI may still be busy
  return (spiHal.State == HAL_SPI_STATE_BUSY_TX); // Check if SPI Tx is busy
}


/***************************************************************************************
** Function name:           dmaWait
** Description:             Wait until DMA is over (blocking!)
***************************************************************************************/
void TFT_eSPI::dmaWait(void)
{
  //return (dmaHal.State == HAL_DMA_STATE_BUSY);  // Do not use, SPI may still be busy
  while (spiHal.State == HAL_SPI_STATE_BUSY_TX); // Check if SPI Tx is busy
}


/***************************************************************************************
** Function name:           pushPixelsDMA
** Description:             Push pixels to TFT (len must be less than 32767)
***************************************************************************************/
// This will byte swap the original image if setSwapBytes(true) was called by sketch.
void TFT_eSPI::pushPixelsDMA(unsigned short* image, unsigned int len)
{
  if (len == 0) return;

  // Wait for any current DMA transaction to end
  while (spiHal.State == HAL_SPI_STATE_BUSY_TX); // Check if SPI Tx is busy

  if(_swapBytes) {
    for (unsigned int i = 0; i < len; i++) (image[i] = image[i] << 8 | image[i] >> 8);
  }

  HAL_SPI_Transmit_DMA(&spiHal, (unsigned char*)image, len << 1);
}


/***************************************************************************************
** Function name:           pushImageDMA
** Description:             Push image to a window (w*h must be less than 65536)
***************************************************************************************/
// This will clip and also swap bytes if setSwapBytes(true) was called by sketch
void TFT_eSPI::pushImageDMA(int x, int y, int w, int h, unsigned short* image, unsigned short* buffer)
{
  if ((x >= _vpW) || (y >= _vpH)) return;

  int dx = 0;
  int dy = 0;
  int dw = w;
  int dh = h;

  if (x < _vpX) { dx = _vpX - x; dw -= dx; x = _vpX; }
  if (y < _vpY) { dy = _vpY - y; dh -= dy; y = _vpY; }

  if ((x + dw) > _vpW ) dw = _vpW - x;
  if ((y + dh) > _vpH ) dh = _vpH - y;

  if (dw < 1 || dh < 1) return;

  unsigned int len = dw*dh;

  if (buffer == nullptr) {
    buffer = image;
    while (spiHal.State == HAL_SPI_STATE_BUSY_TX); // Check if SPI Tx is busy
  }

  // If image is clipped, copy pixels into a contiguous block
  if ( (dw != w) || (dh != h) ) {
    if(_swapBytes) {
      for (int yb = 0; yb < dh; yb++) {
        for (int xb = 0; xb < dw; xb++) {
          unsigned int src = xb + dx + w * (yb + dy);
          (buffer[xb + yb * dw] = image[src] << 8 | image[src] >> 8);
        }
      }
    }
    else {
      for (int yb = 0; yb < dh; yb++) {
        memcpy((unsigned char*) (buffer + yb * dw), (unsigned char*) (image + dx + w * (yb + dy)), dw << 1);
      }
    }
  }
  // else, if a buffer pointer has been provided copy whole image to the buffer
  else if (buffer != image || _swapBytes) {
    if(_swapBytes) {
      for (unsigned int i = 0; i < len; i++) (buffer[i] = image[i] << 8 | image[i] >> 8);
    }
    else {
      memcpy(buffer, image, len*2);
    }
  }

  setWindow(x, y, x + dw - 1, y + dh - 1);

  // DMA byte count for transmit is only 16 bits maximum, so to avoid this constraint
  // small transfers are performed using a blocking call until DMA capacity is reached.
  // User sketch can prevent blocking by managing pixel count and splitting into blocks
  // of 32767 pixels maximum. (equivalent to an area of ~320 x 100 pixels)
  while(len>0x7FFF) { // Transfer 16-bit pixels in blocks if len*2 over 65534 bytes
    HAL_SPI_Transmit(&spiHal, (unsigned char*)buffer, 0x800<<1, HAL_MAX_DELAY);
    len -= 0x800; buffer+= 0x800; // Arbitrarily send 1K pixel blocks (2Kbytes)
  }
  // Send remaining pixels using DMA (max 65534 bytes)
  HAL_SPI_Transmit_DMA(&spiHal, (unsigned char*)buffer, len << 1);
}

////////////////////////////////////////////////////////////////////////////////////////
// Processor specific DMA initialisation
////////////////////////////////////////////////////////////////////////////////////////

// The DMA functions here work with SPI only (not parallel)
#if defined (STM32F2xx) || defined (STM32F4xx) || defined (STM32F7xx)
/***************************************************************************************
** Function name:           DMAX_StreamX_IRQHandler
** Description:             Override the default HAL stream X interrupt handler
***************************************************************************************/
  #if (TFT_SPI_PORT == 1)
    extern "C" void DMA2_Stream3_IRQHandler();
    void DMA2_Stream3_IRQHandler(void)
  #elif (TFT_SPI_PORT == 2)
    extern "C" void DMA1_Stream4_IRQHandler();
    void DMA1_Stream4_IRQHandler(void)
  #elif (TFT_SPI_PORT == 3)
    extern "C" void DMA1_Stream5_IRQHandler();
    void DMA1_Stream5_IRQHandler(void)
  #endif
  {
    // Call the default end of buffer handler
    HAL_DMA_IRQHandler(&dmaHal);
  }

/***************************************************************************************
** Function name:           initDMA
** Description:             Initialise the DMA engine - returns true if init OK
***************************************************************************************/
// This initialisation is for STM32F2xx/4xx/7xx processors and may not work on others
// Dual core H7xx series not supported yet, they are different and have a DMA MUX: 
// https://electronics.stackexchange.com/questions/379813/configuring-the-dma-request-multiplexer-on-a-stm32h7-mcu
bool TFT_eSPI::initDMA(bool ctrl_cs)
{
  ctrl_cs = ctrl_cs; // Not used for STM32, so stop compiler warning

  #if (TFT_SPI_PORT == 1)
    __HAL_RCC_DMA2_CLK_ENABLE();                           // Enable DMA2 clock
    dmaHal.Init.Channel = DMA_CHANNEL_3;                   // DMA channel 3 is for SPI1 TX
  #elif (TFT_SPI_PORT == 2)
    __HAL_RCC_DMA1_CLK_ENABLE();                           // Enable DMA1 clock
    dmaHal.Init.Channel = DMA_CHANNEL_0;                   // DMA channel 0 is for SPI2 TX
  #elif (TFT_SPI_PORT == 3)
    __HAL_RCC_DMA1_CLK_ENABLE();                           // Enable DMA1 clock
    dmaHal.Init.Channel = DMA_CHANNEL_0;                   // DMA channel 0 is for SPI3 TX
  
  #endif

  dmaHal.Init.Mode =  DMA_NORMAL; //DMA_CIRCULAR;   //   // Normal = send buffer once
  dmaHal.Init.Direction = DMA_MEMORY_TO_PERIPH;          // Copy memory to the peripheral
  dmaHal.Init.PeriphInc = DMA_PINC_DISABLE;              // Don't increment peripheral address
  dmaHal.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; // Peripheral is byte aligned
  dmaHal.Init.MemInc = DMA_MINC_ENABLE;                  // Increment memory address
  dmaHal.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;    // Memory is byte aligned

  if (HAL_DMA_Init(&dmaHal) != HAL_OK){                  // Init DMA with settings
    // Insert error message here?
    return DMA_Enabled = false;
  };
  #if (TFT_SPI_PORT == 1)
    HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);  // Enable DMA end interrupt handler
  #elif (TFT_SPI_PORT == 2)
    HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);  // Enable DMA end interrupt handler
  #elif (TFT_SPI_PORT == 3)
    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  #endif

  __HAL_LINKDMA(&spiHal, hdmatx, dmaHal);   // Attach DMA engine to SPI peripheral

  return DMA_Enabled = true;
}

#elif defined (STM32F1xx) // Supports "Blue Pill" boards
/***************************************************************************************
** Function name:           DMA1_ChannelX_IRQHandler
** Description:             Override the default HAL stream 3 interrupt handler
***************************************************************************************/
  #if (TFT_SPI_PORT == 1)
    extern "C" void DMA1_Channel3_IRQHandler();
    void DMA1_Channel3_IRQHandler(void)
  #elif (TFT_SPI_PORT == 2)
    extern "C" void DMA1_Channel5_IRQHandler();
    void DMA1_Channel5_IRQHandler(void)
  #endif
  {
    // Call the default end of buffer handler
    HAL_DMA_IRQHandler(&dmaHal);
  }

//*/
/***************************************************************************************
** Function name:           initDMA
** Description:             Initialise the DMA engine - returns true if init OK
***************************************************************************************/
bool TFT_eSPI::initDMA(bool ctrl_cs)
{
  ctrl_cs = ctrl_cs; // Not used for STM32, so stop compiler warning

  __HAL_RCC_DMA1_CLK_ENABLE();                           // Enable DMA1 clock

  dmaHal.Init.Mode =  DMA_NORMAL; //DMA_CIRCULAR;   //   // Normal = send buffer once
  dmaHal.Init.Direction = DMA_MEMORY_TO_PERIPH;          // Copy memory to the peripheral
  dmaHal.Init.PeriphInc = DMA_PINC_DISABLE;              // Don't increment peripheral address
  dmaHal.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; // Peripheral is byte aligned
  dmaHal.Init.MemInc = DMA_MINC_ENABLE;                  // Increment memory address
  dmaHal.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;    // Memory is byte aligned
  dmaHal.Init.Priority = DMA_PRIORITY_LOW;               // Added this line - needed ?

  __HAL_LINKDMA(&spiHal, hdmatx, dmaHal); // Attach DMA engine to SPI peripheral

  if (HAL_DMA_Init(&dmaHal) != HAL_OK){                  // Init DMA with settings
    // Insert error message here?
    return DMA_Enabled = false;
  };

  #if (TFT_SPI_PORT == 1)
    HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);  // Enable DMA end interrupt handler
  #elif (TFT_SPI_PORT == 2)
    HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);  // Enable DMA end interrupt handler
  #endif

  return DMA_Enabled = true;
}
#endif // End of STM32F1/2/4/7xx

/***************************************************************************************
** Function name:           deInitDMA
** Description:             Disconnect the DMA engine from SPI
***************************************************************************************/
void TFT_eSPI::deInitDMA(void)
{
  HAL_DMA_DeInit(&dmaHal);
  DMA_Enabled = false;
}

////////////////////////////////////////////////////////////////////////////////////////
#endif // End of DMA FUNCTIONS    
////////////////////////////////////////////////////////////////////////////////////////
