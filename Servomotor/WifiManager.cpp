#include "WiFiManager.h"
#include <Arduino.h>

WiFiManager::WiFiManager(const char* ssid, const char* password) {
    this->ssid = ssid;
    this->password = password;
}

void WiFiManager::connect() {
    Serial.print("Connecting to WiFi");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected.");
}

bool WiFiManager::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}