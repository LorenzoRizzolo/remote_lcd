#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include "config.h"

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

void newMessageScreen() {
  TFT_SCREEN.fillScreen(ST77XX_BLACK);

  const char* text = NEW_MESSAGE_MSG;
  TFT_SCREEN.setTextColor(ST77XX_CYAN);
  TFT_SCREEN.setTextSize(3);

  int16_t x1, y1;
  uint16_t w, h;
  TFT_SCREEN.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

  int textX = (TFT_SCREEN.width() - w) / 2;
  int textY = (TFT_SCREEN.height() - h) / 2;

  TFT_SCREEN.setCursor(textX, textY);
  TFT_SCREEN.println(text);
}

void drawHorizontalLine(int y, uint16_t color = ST77XX_WHITE, int thickness = 1, int margin = 10) {
  int x = margin;
  int width = TFT_SCREEN.width() - 2 * margin;

  for (int i = 0; i < thickness; i++) {
    TFT_SCREEN.drawFastHLine(x, y + i, width, color);
  }
}

void drawRSSIBars(int rssi, int x, int y, int barWidth, int barSpacing, int baseHeight) {
  int bars = 0;
  if (rssi >= -40) bars = 5;
  else if (rssi >= -55) bars = 4;
  else if (rssi >= -65) bars = 3;
  else if (rssi >= -75) bars = 2;
  else if (rssi >= -85) bars = 1;
  else bars = 0;

  int maxBars = 5;
  uint16_t colorOn = ST77XX_GREEN;
  uint16_t colorOff = ST77XX_BLUE;
  for (int i = 0; i < maxBars; i++) {
    int height = baseHeight + i * 6;
    int bx = x + i * (barWidth + barSpacing);
    int by = y - height;
    uint16_t color = (i < bars) ? colorOn : colorOff;
    TFT_SCREEN.fillRect(bx, by, barWidth, height, color);
  }
}
