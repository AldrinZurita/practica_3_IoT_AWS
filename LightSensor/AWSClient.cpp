#include "AWSClient.h"

AWSClient::AWSClient(const char* broker, int port, const char* clientId, const char* updateTopic, const char* getTopic)
    : broker(broker), port(port), clientId(clientId), updateTopic(updateTopic), getTopic(getTopic), client(net) {}

void AWSClient::setupCertificates(const char* rootCA, const char* cert, const char* privateKey) {
    net.setCACert(rootCA);
    net.setCertificate(cert);
    net.setPrivateKey(privateKey);
}

void AWSClient::initialize() {
    client.setServer(broker, port);
    while (!client.connected()) {
        if (client.connect(clientId)) {
            client.subscribe(getTopic);
        } else {
            delay(1000);
        }
    }
}

void AWSClient::loop() {
    client.loop();
}

bool AWSClient::isConnected() {
    return client.connected();
}

void AWSClient::updateShadowIfChanged(const LightSensor& sensor) {
    if (!sensor.hasLevelChanged()) return;

    StaticJsonDocument<200> doc;
    doc["state"]["reported"]["lightLevel"] = sensor.getLevel();
    doc["state"]["reported"]["intensity"] = sensor.getIntensity();
    doc["state"]["reported"]["timestamp"] = millis();

    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer);
    client.publish(updateTopic, jsonBuffer);
}
