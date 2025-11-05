#include <WiFi.h>
#include <HTTPClient.h>
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
float humidity = 0;

void printOnLCDScreen(String msg){
  clearTFT();
  TFT_SCREEN.setTextColor(ST77XX_MAGENTA);
  TFT_SCREEN.println((String)WiFi.getHostname() + " - " + WiFi.localIP().toString());
  TFT_SCREEN.setTextColor(ST77XX_GREEN);
  TFT_SCREEN.print("\n");
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
  TFT_SCREEN.print(humidity, 1);
  TFT_SCREEN.setTextColor(ST77XX_WHITE);
  TFT_SCREEN.println(" % di umidita");

  // print the message
  TFT_SCREEN.print("\n" + (String)msg);
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

  // Create LED blink task
  if(ledTaskHandle == NULL){
    xTaskCreate(
      ledTask,          // funzione del task
      "Task LED",       // nome del task
      1024,             // dimensione stack in byte
      NULL,             // parametro (nessuno)
      1,                // priorità
      &ledTaskHandle    // handle
    );
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nAvvio ESP32...");
  delay(1000);

  // ===== Setup LED pin =====
  pinMode(LED_BLINK_PIN, OUTPUT);

  // initialize the LCD screen
  initializeTFT();
  showMessageOnTFT("Inizializzazione del dispositivo...");

  // ===== Initialize DHT22 sensor =====
  dht.begin();

  // ===== Connect to WiFi =====
  if(initWiFi() == 200){
    Serial.println("Connessione WiFi riuscita.");
    showMessageOnTFT("Connesso al WiFi: \n\n" + WIFI_SSID);
    delay(2000);
  }else{
    Serial.println("Connessione WiFi fallita.");
    showMessageOnTFT("Connessione WiFi fallita.");
    delay(2000);
  }

  // ===== Load message from NVS =====
  prefs.begin("remote_lcd", false);
  message = prefs.getString("msg", "Nessun messaggio presente");
  // api_message is used to remember the last message into api endpoint
  String api_message = prefs.getString("api_msg", "Nessun messaggio presente");
  Serial.println("Messaggio caricato: " + message);
  printOnLCDScreen(message);

  // ===== Async WebServer routes =====
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<center>"
                  "<h1>Nuovo Messaggio</h1>"
                  "<form action='/update'>"
                  "<input name='msg'><input type='submit' value='Invia'>"
                  "</form>"
                  "<h1>Messaggio Attuale</h1>"
                  "<p>"+message+"</p>"
                  "<h1>Temperatura</h1>"
                  "<p>"+temperature+" &deg;C</p>"
                  "<h1>Umidit&agrave;</h1>"
                  "<p>"+humidity+" %</p>"
                  "</center>";
    request->send(200, "text/html", html);
  });

  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request){
    if(request->hasParam("msg")){
      message = request->getParam("msg")->value();
      prefs.putString("msg", message);     // Save to NVS

      update_message(message);
      
      Serial.println("Nuovo messaggio da web: " + message);
    }
    request->send(200, "text/html", "Messaggio aggiornato! <br><a href='/'>Torna indietro</a>");
  });

  server.begin();
}

void loop() {

  // Read temperature from DHT22
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Errore nella lettura del sensore DHT22!");
    temperature = 0.0;
    humidity = 0.0;
  }else{
    Serial.println("Temperatura: " + String(temperature) + " C - Umidita: " + String(humidity) + " %");
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

        Serial.println("Nuovo messaggio da API: " + message);
      }
    } else {
      Serial.println("Errore nella richiesta HTTP: " + String(httpCode));
    }
  }

  // Update LCD screen with new temperature
  if(!updated){
    printOnLCDScreen(message);
  }

  delay(DELAY_UPDATE_SCREEN);
}
