#include <WiFi.h>
#include <ESPmDNS.h>
#include "config.h"
#include "tft_module.h"

String WIFI_SSID = "";

// ===================================================
// Initialize WiFi connection and start mDNS
// ===================================================
int initWiFi() {
  Serial.println("🔍 Connettendo alle reti salvate...");

  WiFi.mode(WIFI_STA);

  Serial.println("Hostname prima della rinomina: " + String(WiFi.getHostname()));
  // WiFi.setHostname(WIFI_HOSTNAME);

  // Set WiFi transmission power
  WiFi.setTxPower(WIFI_POWER);

  for (int i = 0; i < WIFI_COUNT; i++) {
    Serial.printf("Connetendo alla rete '%s' con '%s'...\n", WIFI_SSIDS[i], WIFI_PASSWORDS[i]);
    // showMessageOnTFT("Connettendo a: " + String(WIFI_SSIDS[i]));

    WiFi.begin(WIFI_SSIDS[i], WIFI_PASSWORDS[i]);

    unsigned long startAttemptTime = millis();

    // Wait up to 10 seconds for each network
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIME_OUT) {
      Serial.print(".");
      delay(500);
    }

    if (WiFi.status() == WL_CONNECTED) {
      WIFI_SSID = WIFI_SSIDS[i];

      Serial.println("\n✅ Connessione riuscita!");
      Serial.print("SSID: "); Serial.println(WIFI_SSIDS[i]);
      Serial.print("Hostname: "); Serial.println(WiFi.getHostname());
      Serial.print("IP: "); Serial.println(WiFi.localIP());
      Serial.print("RSSI: "); Serial.println(WiFi.RSSI());

      // ===== Start mDNS =====
      if (MDNS.begin(WiFi.getHostname())) {
        Serial.println("🌐 mDNS avviato!");
        Serial.printf("Accedi al dispositivo a: http://%s\n", WiFi.getHostname());
      } else {
        Serial.println("⚠️ Errore mDNS!");
      }

      return 200; // ✅ WiFi connection successful
    } else {
      Serial.println("\n❌ Connessione fallita...");
    }
  }

  // No network worked
  Serial.println("⚠️ Nessuna rete disponibile.");
  return 500;
}
