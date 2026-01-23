// #include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

/**
 * @param msg is the message to pint into LCD screen
 */
void showMessageOnTFT(String msg);

/** 
 * clear the LCD screen
 */
void clearTFT();

/**
 * initialize the LCD screen
 */
void initializeTFT();

void showError(String error);

void newMessageScreen();

void drawHorizontalLine(int y, uint16_t color = ST77XX_WHITE, int thickness = 1, int margin = 10);

// Draw 5 RSSI bars based on WiFi.RSSI() value
void drawRSSIBars(int rssi, int x = 10, int y = 20, int barWidth = 6, int barSpacing = 4, int baseHeight = 10);

void printWithNewlines(const String& msg);

extern Adafruit_ST7789 TFT_SCREEN;