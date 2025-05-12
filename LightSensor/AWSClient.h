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

public:
    AWSClient(const char* broker, int port, const char* clientId, const char* updateTopic, const char* getTopic);
    void setupCertificates(const char* rootCA, const char* cert, const char* privateKey);
    void initialize();
    void loop();
    bool isConnected();
    void updateShadowIfChanged(const LightSensor& sensor);
};

#endif