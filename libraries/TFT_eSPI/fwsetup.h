// ST7789 240 x 240 display with no chip select line
#define USER_SETUP_ID 24

#define ST7789_DRIVER     // Configure all registers

#define TFT_WIDTH  240
#define TFT_HEIGHT 240

//#define TFT_RGB_ORDER TFT_RGB  // Colour order Red-Green-Blue
//#define TFT_RGB_ORDER TFT_BGR  // Colour order Blue-Green-Red

//#define TFT_INVERSION_ON
//#define TFT_INVERSION_OFF

// DSTIKE stepup
#define TFT_DC    1
#define TFT_RST   0
#define TFT_CS    -1 // Not connected
#define TFT_MOSI  6
#define TFT_MISO  5
#define TFT_SCLK  4

// #define SPI_FREQUENCY  27000000
#define SPI_FREQUENCY  40000000

#define SPI_READ_FREQUENCY  20000000

#define SPI_TOUCH_FREQUENCY  2500000

// #define SUPPORT_TRANSACTIONS
