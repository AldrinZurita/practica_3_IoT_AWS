#include "WiFiManager.h"

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

// AWSClient.h
#ifndef AWS_CLIENT_H
#define AWS_CLIENT_H

#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "LightSensor.h"

class AWSClient {
private:
    const char* broker;
    int port;
    const char* clientId;
    const char* updateTopic;
    const char* getTopic;

    WiFiClientSecure net;
    PubSubClient client;

    void connectToAWS();
    static void callback(char* topic, byte* payload, unsigned int length);
    static AWSClient* instance;

public:
    AWSClient(const char* broker, int port, const char* clientId, const char* updateTopic, const char* getTopic);
    void setupCertificates(const char* ca, const char* cert, const char* key);
    void initialize();
    void updateShadow(const LightSensor& sensor);
    void loop();
};

#endif
