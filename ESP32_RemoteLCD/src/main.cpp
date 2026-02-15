#include <WiFi.h>
#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <ESPmDNS.h>
#include <DHT.h>
#include <DHT_U.h>
#include <SPIFFS.h>
#include <time.h>

#include "config.h"
#include "tft_module.h"
#include "wifi_module.h"

// Web server running on port 80
AsyncWebServer server(80);
// Preferences for NVS storage
Preferences prefs;
// Message to display on TFT
String message = "";
// LED Blink Task handle
TaskHandle_t ledTaskHandle = NULL;
// DHT22 sensor instance
DHT dht(DHTPIN, DHTTYPE);
float temperature = 0;
float humidity = 0;

// Global current time strings (updated from NTP)
String currentTimeStr = "";      // e.g., "2025-12-07 13:53"
String currentDateStr = "";      // e.g., "2025-12-07"
String currentTimeOnlyStr = "";  // e.g., "13:53"

// state of LCD backlight
bool backlightOn = true;

static void updateCurrentTimeStr() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo, 1000)) { // wait up to 1s
    char bufDT[20]; // DD/MM/YYYY HH:MM
    strftime(bufDT, sizeof(bufDT), "%d/%m/%Y %H:%M", &timeinfo);
    currentTimeStr = String(bufDT);

    char bufD[11]; // DD/MM/YYYY
    strftime(bufD, sizeof(bufD), "%d/%m/%Y", &timeinfo);
    currentDateStr = String(bufD);

    char bufT[6]; // HH:MM
    strftime(bufT, sizeof(bufT), "%H:%M", &timeinfo);
    currentTimeOnlyStr = String(bufT);
  }
}

void printOnLCDScreen(String msg){
  // Update current time strings
  updateCurrentTimeStr();
  clearTFT();
  TFT_SCREEN.setTextSize(2);
  TFT_SCREEN.setTextColor(ST77XX_MAGENTA);
  if(WIFI_SSID!=""){
    // TFT_SCREEN.println("\n" + WiFi.localIP().toString());
    TFT_SCREEN.print("\nhttp://"); TFT_SCREEN.println(WiFi.getHostname());
    // Draw RSSI bars under IP
    int currentY = TFT_SCREEN.getCursorY();
    Serial.printf("WiFi RSSI: %d dBm\n", WiFi.RSSI());
    drawRSSIBars(WiFi.RSSI(), SCREEN_WIDTH-60, currentY+30, 6, 4, 2);
  }else{
    TFT_SCREEN.println("\n" + (String)NO_WIFI_MSG);
  }
  TFT_SCREEN.setTextColor(ST77XX_GREEN);
  TFT_SCREEN.print("\n");
  
  if(USE_MIDDLELINE){
    drawHorizontalLine(TFT_SCREEN.getCursorY(), ST77XX_WHITE, 2, 10);
    TFT_SCREEN.print("\n");
  }

  //  Print current time
  // TFT_SCREEN.print(" ");
  TFT_SCREEN.setTextColor(ST77XX_WHITE);
  if(currentTimeOnlyStr=="" || currentDateStr=="" || WIFI_SSID==""){
    TFT_SCREEN.print((String)NO_CLOCK_MSG);
  }else{
    TFT_SCREEN.print(currentTimeOnlyStr + " - " + currentDateStr);
  }

  TFT_SCREEN.setTextSize(2);
  TFT_SCREEN.print("\n\n");

  // TFT_SCREEN.print(" ");
  TFT_SCREEN.setTextSize(4);

  // Print temperature with color coding
  if (temperature <= 12) {
      TFT_SCREEN.setTextColor(ST77XX_BLUE);
  } else if (temperature >= 13 && temperature <= 16) {
      TFT_SCREEN.setTextColor(ST77XX_GREEN);
  } else if (temperature >= 16) {
      TFT_SCREEN.setTextColor(ST77XX_ORANGE);
  }
  TFT_SCREEN.print(temperature, 1);
  TFT_SCREEN.setTextSize(2);
  TFT_SCREEN.setTextColor(ST77XX_WHITE);
  TFT_SCREEN.print(" C");

  TFT_SCREEN.print("\n\n");
  TFT_SCREEN.setTextColor(ST77XX_YELLOW);
  // TFT_SCREEN.print(" ");
  TFT_SCREEN.print(humidity, 1);
  TFT_SCREEN.setTextColor(ST77XX_WHITE);
  TFT_SCREEN.println(" %");

  if(USE_MIDDLELINE){
    TFT_SCREEN.print("\n");
    drawHorizontalLine(TFT_SCREEN.getCursorY(), ST77XX_WHITE, 2, 10);
  }

  // print the message
  TFT_SCREEN.setTextSize(3);
  TFT_SCREEN.print("\n");
  printWithNewlines(msg);
}

void ledTask(void *parameter) {
  digitalWrite(LED_BLINK_PIN, HIGH);
  vTaskDelay(LED_BLINK_DURATION / portTICK_PERIOD_MS); // wait for the specified duration
  digitalWrite(LED_BLINK_PIN, LOW);
  ledTaskHandle = NULL; // Clear the task handle  
  vTaskDelete(NULL); // Delete this task
}

void update_message(String message){
  // Update LCD screen
  printOnLCDScreen(message);

  // Show new message screen
  newMessageScreen();

  // Create LED blink task
  if(ledTaskHandle == NULL){
    xTaskCreate(
      ledTask,          // task function
      "Task LED",       // task name
      1024,             // stack size in bytes
      NULL,             // parameter (none)
      1,                // priority
      &ledTaskHandle    // handle
    );
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nStarting ESP32...");
  delay(1000);

  // ===== Setup LED pin =====
  pinMode(LED_BLINK_PIN, OUTPUT);
  pinMode(PIN_LCD_BACKLIGHT, OUTPUT);
  digitalWrite(PIN_LCD_BACKLIGHT, HIGH); // Turn on backlight
  backlightOn = true;

  // initialize the LCD screen
  initializeTFT();
  showMessageOnTFT("\n " + (String)START_DEVICE_MSG);

  // ===== Initialize DHT22 sensor =====
  dht.begin();

  // ===== Connect to WiFi =====
  if(initWiFi() == 200){
    Serial.println("WiFi connection successful.");
    // Setup timezone and NTP servers using configTzTime (handles TZ internally)
    configTzTime("CET-1CEST,M3.5.0/2,M10.5.0/3", "pool.ntp.org", "time.nist.gov");
    updateCurrentTimeStr();
    delay(2000);
  }else{
    Serial.println("WiFi connection failed.");
    // showMessageOnTFT("Connessione WiFi fallita.");
    delay(2000);
  }

  // ===== Init SPIFFS for static web files =====
  if(!SPIFFS.begin(true)){
    Serial.println("Error initializing SPIFFS");
  }

  // ===== Load message from NVS =====
  prefs.begin("remote_lcd", false);
  message = prefs.getString("msg", "No message available");
  // api_message is used to remember the last message into api endpoint
  String api_message = prefs.getString("api_msg", "No message available");
  Serial.println("Message loaded: " + message);
  // printOnLCDScreen(message);

  // ===== Async WebServer routes =====
  // Serve static files from SPIFFS 'data' folder
  // Disable caching to ensure latest files are served
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html").setCacheControl("no-cache, no-store, must-revalidate");

  server.on("/change", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // JSON status endpoint for the web UI
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
    updateCurrentTimeStr();
    String json = "{\"message\":\"" + message + "\",\"temperature\":" + String(temperature, 1) + ",\"humidity\":" + String(humidity, 1) + ",\"time\":\"" + currentTimeStr + "\",\"rssi\":" + String(WiFi.RSSI()) + ",\"ssid\":\"" + WIFI_SSID + "\",\"backlight_on\":" + String(backlightOn ? "true" : "false") + "}";
    request->send(200, "application/json", json);
  });

  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request){
    if(request->hasParam("msg")){
      message = request->getParam("msg")->value();
      prefs.putString("msg", message);     // Save to NVS

      update_message(message);
      
      Serial.println("New message from web: " + message);
    }
    request->send(200, "text/html", "Message updated!");
  });

  server.on("/toggle_backlight", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Toggling backlight...");
    backlightOn = !backlightOn;
    digitalWrite(PIN_LCD_BACKLIGHT, backlightOn ? HIGH : LOW);
    String json = "{\"backlight_on\":" + String(backlightOn ? "true" : "false") + "}";
    request->send(200, "application/json", json);
  });

  server.begin();
}

void loop() {

  // Read temperature from DHT22
  temperature = dht.readTemperature() + TEMP_ERROR;
  humidity = dht.readHumidity();
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Error reading DHT22 sensor!");
    temperature = 0.0;
    humidity = 0.0;
  }else{
    Serial.println("Temperature: " + String(temperature) + " C - Humidity: " + String(humidity) + " %");
  }

  // check WiFi connection
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("WiFi connection lost. Attempting to reconnect...");
    // try to reconnect
    initWiFi();
  }

  boolean updated = false;
  if(GET_FROM_API){
    // Fetch message from API
    HTTPClient http;
    http.begin(API_URL);
    int httpCode = http.GET();
    if (httpCode == 200) {
      String api_message = http.getString();
      if(api_message != prefs.getString("api_msg", "")){
        message = api_message;
        prefs.putString("msg", message);         // Save new message to NVS
        prefs.putString("api_msg", api_message); // Update last API message

        updated = true;
        update_message(message);

        Serial.println("New message from API: " + message);
      }
    } else {
      Serial.println("Error in HTTP request: " + String(httpCode));
    }
  }

  // Update LCD screen with new temperature just if no message update occurred
  if(!updated){
    printOnLCDScreen(message);
  }

  delay(DELAY_UPDATE_SCREEN);
}
