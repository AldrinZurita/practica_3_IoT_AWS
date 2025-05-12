#ifndef MQTT_CLIENT_HANDLER_H
#define MQTT_CLIENT_HANDLER_H

#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "ServoController.h"

class MQTTClientHandler {
private:
    const char* broker;
    int port;
    const char* clientId;
    const char* getTopic;
    const char* getAcceptedTopic;
    const char* updateTopic;
    const char* deltaTopic;

    WiFiClientSecure net;
    PubSubClient client;
    ServoController& servoController;
    bool initialShadowReceived;

    static MQTTClientHandler* instance;
    static void mqttCallback(char* topic, byte* payload, unsigned int length);

    void handleDeltaMessage(const JsonDocument& doc);
    void handleGetAcceptedMessage(const JsonDocument& doc);
    void updateShadowReported();

public:
    MQTTClientHandler(const char* broker, int port, const char* clientId,
                      const char* getTopic, const char* getAcceptedTopic,
                      const char* updateTopic, const char* deltaTopic,
                      ServoController& servoController);

    void setupCertificates(const char* rootCA, const char* cert, const char* privateKey);
    void initialize();
    void loop();
    bool isConnected();
};

#endif