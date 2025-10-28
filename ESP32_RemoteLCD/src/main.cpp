#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <ESPmDNS.h>
#include <DHT.h>
#include <DHT_U.h>

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

void printOnLCDScreen(String msg){
  clearTFT();
  TFT_SCREEN.setTextColor(ST77XX_BLUE);
  TFT_SCREEN.println((String)WiFi.getHostname() + " - " + WiFi.localIP().toString());
  TFT_SCREEN.setTextColor(ST77XX_GREEN);
  TFT_SCREEN.setTextSize(4);
  TFT_SCREEN.print("\n");

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
  TFT_SCREEN.print(" gradi");
  TFT_SCREEN.setTextColor(ST77XX_WHITE);

  // print the message
  TFT_SCREEN.print("\n\n\n" + (String)msg);
}

void ledTask(void *parameter) {
  digitalWrite(LED_BLINK_PIN, HIGH);
  vTaskDelay(LED_BLINK_DURATION / portTICK_PERIOD_MS); // wait for the specified duration
  digitalWrite(LED_BLINK_PIN, LOW);
  ledTaskHandle = NULL; // Clear the task handle  
  vTaskDelete(NULL); // Delete this task
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nAvvio ESP32...");
  delay(1000);

  // ===== Setup LED pin =====
  pinMode(LED_BLINK_PIN, OUTPUT);

  // initialize the LCD screen
  initializeTFT();
  showMessageOnTFT("Initializing device...");

  // ===== Connect to WiFi =====
  if(initWiFi() == 200){
    Serial.println("WiFi connected successfully.");
    showMessageOnTFT("Connected to WiFi: \n\n" + WIFI_SSID);
    delay(2000);
  }else{
    Serial.println("WiFi connection failed.");
    showMessageOnTFT("WiFi connection failed.");
    delay(2000);
  }

  // ===== Load message from NVS =====
  prefs.begin("remote_lcd", false);
  message = prefs.getString("msg", "Insert a message");
  Serial.println("\nLoaded message: " + message);
  printOnLCDScreen(message);

  // ===== Async WebServer routes =====
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<form action='/update'><input name='msg'><input type='submit'></form>";
    html += "<p>Current message: " + message + "</p>";
    request->send(200, "text/html", html);
  });

  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request){
    if(request->hasParam("msg")){
      message = request->getParam("msg")->value();
      prefs.putString("msg", message);     // Save to NVS

      // Update LCD screen
      printOnLCDScreen(message);

      // Create LED blink task
      if(ledTaskHandle == NULL){
        xTaskCreate(
          ledTask,          // funzione del task
          "LED Task",       // nome del task
          1024,             // dimensione stack in byte
          NULL,             // parametro (nessuno)
          1,                // priorità
          &ledTaskHandle    // handle
        );
      }
      Serial.println("New message: " + message);
    }
    request->send(200, "text/html", "Message updated! <br><a href='/'>Back</a>");
  });

  server.begin();
}

void loop() {

  // Read temperature from DHT22
  temperature = dht.readTemperature();
  temperature = random(0, 30); // Simulated temperature for testing
  if (isnan(temperature)) {
    Serial.println("Errore nella lettura del sensore DHT22!");
    showError("Errore sensore\nDHT"+String(DHTTYPE));
    delay(2000);
    return;
  }

  // Update LCD screen with new temperature
  printOnLCDScreen(message);

  delay(DELAY_UPDATE_SCREEN);
}
