#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include "config.h"

// Inizializzazione SPI software
Adafruit_ST7789 TFT_SCREEN = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void initializeTFT() {
  Serial.println("Inizializzazione TFT...");
  SPI.begin(TFT_SCL, -1, TFT_SDA); // SCK, MISO (none), MOSI
  TFT_SCREEN.init(SCREEN_HEIGHT, SCREEN_WIDTH);
  TFT_SCREEN.setRotation(ROTATION);
  TFT_SCREEN.fillScreen(ST77XX_BLACK);
  TFT_SCREEN.setTextColor(ST77XX_WHITE);
  TFT_SCREEN.setTextSize(TEXT_SIZE);
}

void clearTFT() {
  TFT_SCREEN.fillScreen(ST77XX_BLACK);
  TFT_SCREEN.setCursor(0, 0);
  TFT_SCREEN.setTextColor(ST77XX_WHITE);
  TFT_SCREEN.setTextSize(TEXT_SIZE);
}

void showMessageOnTFT(String msg) {
  TFT_SCREEN.fillScreen(ST77XX_BLACK);
  TFT_SCREEN.setCursor(0, 0);
  TFT_SCREEN.setTextColor(ST77XX_WHITE);
  TFT_SCREEN.setTextSize(TEXT_SIZE);
  TFT_SCREEN.println(msg);
}

void showError(String error) {
    TFT_SCREEN.fillScreen(ST77XX_BLACK);
    TFT_SCREEN.setCursor(0, 0);
    TFT_SCREEN.setTextColor(ST77XX_RED);
    TFT_SCREEN.setTextSize(3);
    TFT_SCREEN.println(error);
}