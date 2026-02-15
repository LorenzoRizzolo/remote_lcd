#ifndef CONFIG_FILE
  #define CONFIG_FILE

  // general config
  #define START_DEVICE_MSG "Avvio dispositivo..."
  #define NO_WIFI_MSG "No WiFi"
  #define NO_CLOCK_MSG "Orario non disponibile"
  #define NEW_MESSAGE_MSG "Nuovo messaggio"

  // LCD config
  #define SCREEN_WIDTH 320
  #define SCREEN_HEIGHT 240
  #define TEXT_SIZE 2
  #define ROTATION 3
  #define DELAY_UPDATE_SCREEN 5000

  // LCD TFT pins
  #define TFT_CS   8
  #define TFT_DC   9
  #define TFT_RST  21
  #define TFT_SDA  6   
  #define TFT_SCL  4 
  #define USE_MIDDLELINE false // Use a middle line to separate sections  
  #define START_X 0
  #define START_Y 0
  #define PIN_LCD_BACKLIGHT 10

  // LED config for blink on message update
  #define LED_BLINK_PIN 10
  #define LED_BLINK_DURATION 2000 // milliseconds

  // DHT22 sensor config
  #define DHTPIN 2          // Pin digitale collegato al DHT22
  #define DHTTYPE DHT22     // Tipo sensore: DHT11, DHT22, DHT21
  #define TEMP_ERROR -2     // di quanto va corretto la temperatura letta

  // Wi-Fi config
  // Declare (not define) the arrays here
  #define WIFI_COUNT 3
  extern const char* WIFI_SSIDS[WIFI_COUNT];
  extern const char* WIFI_PASSWORDS[WIFI_COUNT];
  #define WIFI_TIME_OUT 5000
  #define WIFI_POWER WIFI_POWER_2dBm
  #define WIFI_HOSTNAME "esp32c3"
  #define GET_FROM_API true
  #define API_URL "https://rizzoloimpianti.it/file_esp"

#endif
