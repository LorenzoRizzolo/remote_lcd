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

void newMessageScreen() {
  TFT_SCREEN.fillScreen(ST77XX_BLACK);

  // Testo
  const char* text = "Nuovo Messaggio";
  TFT_SCREEN.setTextColor(ST77XX_CYAN);
  TFT_SCREEN.setTextSize(3);

  int16_t x1, y1;
  uint16_t w, h;
  TFT_SCREEN.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

  int textX = (TFT_SCREEN.width() - w) / 2;
  int textY = (TFT_SCREEN.height() - h) / 2; // leggermente più in basso per fare spazio all’immagine

  // Mostra il testo centrato
  TFT_SCREEN.setCursor(textX, textY);
  TFT_SCREEN.println(text);
}

void drawHorizontalLine(int y, uint16_t color = ST77XX_WHITE, int thickness = 1, int margin = 10) {
  int x = margin;
  int width = TFT_SCREEN.width() - 2 * margin;

  // Se vuoi una linea più spessa, disegna più linee vicine
  for (int i = 0; i < thickness; i++) {
    TFT_SCREEN.drawFastHLine(x, y + i, width, color);
  }
}
