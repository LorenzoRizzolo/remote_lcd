#include <WiFi.h>
#include <ESPmDNS.h>
#include "config.h"
#include "tft_module.h"

String WIFI_SSID = "";

// ===================================================
// Initialize WiFi connection and start mDNS
// ===================================================
int initWiFi() {
  WIFI_SSID = "";
  Serial.println("🔍 Scanning nearby networks...");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();   // clear previous connections
  delay(200);

  // Set transmission power
  WiFi.setTxPower(WIFI_POWER);

  // === Scan networks ===
  int n = WiFi.scanNetworks();
  if (n <= 0) {
    Serial.println("❌ No networks found.");
    return 500;
  }

  Serial.printf("📡 %d networks found:\n", n);

  // === Search among found networks for a saved network ===
  for (int i = 0; i < n; i++) {
    String foundSSID = WiFi.SSID(i);
    Serial.printf(" - %s (%d dBm)\n", foundSSID.c_str(), WiFi.RSSI(i));

    // compare with known networks list
    for (int s = 0; s < WIFI_COUNT; s++) {
      if (foundSSID == WIFI_SSIDS[s]) {
        Serial.printf("🎯 Known network found: %s\n", foundSSID.c_str());
        Serial.println("🔗 Attempting to connect...");
        WiFi.begin(WIFI_SSIDS[s], WIFI_PASSWORDS[s]);
        unsigned long startAttemptTime = millis();

        while (WiFi.status() != WL_CONNECTED &&
               millis() - startAttemptTime < WIFI_TIME_OUT) {
          Serial.print(".");
          delay(500);
        }

        if (WiFi.status() == WL_CONNECTED) {
          WIFI_SSID = foundSSID;
          Serial.println("\n✅ Connection successful!");
          Serial.print("SSID: "); Serial.println(WIFI_SSID);
          Serial.print("IP: "); Serial.println(WiFi.localIP());

          // ===== Start mDNS =====
          if (MDNS.begin(WiFi.getHostname())) {
            Serial.println("🌐 mDNS started!");
            Serial.printf("Access the device at: http://%s\n", WiFi.getHostname());
          } else {
            Serial.println("⚠️ Error starting mDNS!");
          }

          return 200; // OK
        }

        Serial.println("\n❌ Unable to connect to the found network.");
      }
    }
  }

  Serial.println("⚠️ No saved networks found nearby.");
  return 500;
}

