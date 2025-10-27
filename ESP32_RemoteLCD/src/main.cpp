#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <ESPmDNS.h> 

#include "config.h"
#include "tft_module.h"
#include "wifi_module.h"


// ===== Global objects =====
AsyncWebServer server(80);
Preferences prefs;
String message = "Starting device...";
TaskHandle_t ledTaskHandle = NULL;
int temperature = 0; // Placeholder for temperature value

void printOnLCDScreen(String msg){
  // showMessageOnTFT(
  //   (String)WiFi.getHostname() + " - " + WiFi.localIP().toString() + "\n\n" +
  //   "Temperatura: " + temperature + " C" + "\n\n" +
  //   msg
  // );

  clearTFT();
  TFT_SCREEN.setTextColor(ST77XX_BLUE);
  TFT_SCREEN.println((String)WiFi.getHostname() + " - " + WiFi.localIP().toString());
  TFT_SCREEN.setTextColor(ST77XX_ORANGE);
  TFT_SCREEN.println("\nTemperatura: " + (String)temperature + " C");
  TFT_SCREEN.setTextColor(ST77XX_WHITE);
  TFT_SCREEN.println("\n" + msg);
}

void ledTask(void *parameter) {
  Serial.println("LED Blink Task started.");
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
  temperature+=1;
  delay(DELAY_UPDATE_SCREEN);
  printOnLCDScreen(message);
  // Here read the temperature sensor and update the TFT display
}
