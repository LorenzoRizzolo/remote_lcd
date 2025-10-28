#include <WiFi.h>
#include <ESPmDNS.h>
#include "config.h"

String WIFI_SSID = "";

// ===================================================
// Initialize WiFi connection and start mDNS
// ===================================================
int initWiFi() {
  Serial.println("🔍 Searching for saved WiFi networks...");

  WiFi.mode(WIFI_STA);

  Serial.println("Hostname before rename: " + String(WiFi.getHostname()));
  WiFi.setHostname(WIFI_HOSTNAME);

  // Set WiFi transmission power
  WiFi.setTxPower(WIFI_POWER_8_5dBm);

  for (int i = 0; i < WIFI_COUNT; i++) {
    Serial.printf("Attempting to connect to '%s'...\n", WIFI_SSIDS[i]);

    WiFi.begin(WIFI_SSIDS[i], WIFI_PASSWORDS[i]);

    unsigned long startAttemptTime = millis();

    // Wait up to 10 seconds for each network
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIME_OUT) {
      Serial.print(".");
      delay(500);
    }

    if (WiFi.status() == WL_CONNECTED) {
      WIFI_SSID = WIFI_SSIDS[i];

      Serial.println("\n✅ Connection successful!");
      Serial.print("SSID: "); Serial.println(WIFI_SSIDS[i]);
      Serial.print("Hostname: "); Serial.println(WiFi.getHostname());
      Serial.print("IP: "); Serial.println(WiFi.localIP());
      Serial.print("RSSI: "); Serial.println(WiFi.RSSI());

      // ===== Start mDNS =====
      if (MDNS.begin(WiFi.getHostname())) {
        Serial.println("🌐 mDNS started successfully!");
        Serial.printf("You can access the device at: http://%s\n", WiFi.getHostname());
      } else {
        Serial.println("⚠️ Failed to start mDNS!");
      }

      return 200; // ✅ WiFi connection successful
    } else {
      Serial.println("\n❌ Connection failed, trying next network...");
    }
  }

  // No network worked
  Serial.println("⚠️ No available network, no connection established.");
  return 500;
}
