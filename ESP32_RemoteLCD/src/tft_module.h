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


extern Adafruit_ST7789 TFT_SCREEN;