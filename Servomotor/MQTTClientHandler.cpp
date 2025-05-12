#include "MQTTClientHandler.h"

MQTTClientHandler* MQTTClientHandler::instance = nullptr;

MQTTClientHandler::MQTTClientHandler(const char* broker, int port, const char* clientId,
                                     const char* getTopic, const char* getAcceptedTopic,
                                     const char* updateTopic, const char* deltaTopic,
                                     ServoController& servoController)
    : broker(broker), port(port), clientId(clientId),
      getTopic(getTopic), getAcceptedTopic(getAcceptedTopic),
      updateTopic(updateTopic), deltaTopic(deltaTopic),
      client(net), servoController(servoController),
      initialShadowReceived(false) {
    instance = this;
}

void MQTTClientHandler::setupCertificates(const char* rootCA, const char* cert, const char* privateKey) {
    net.setCACert(rootCA);
    net.setCertificate(cert);
    net.setPrivateKey(privateKey);
}

void MQTTClientHandler::initialize() {
    client.setServer(broker, port);
    client.setCallback(mqttCallback);

    while (!client.connected()) {
        if (client.connect(clientId)) {
            client.subscribe(deltaTopic);
            client.subscribe(getAcceptedTopic);
            client.publish(getTopic, "{}");
        } else {
            delay(5000);
        }
    }
}

void MQTTClientHandler::loop() {
    client.loop();
}

bool MQTTClientHandler::isConnected() {
    return client.connected();
}

void MQTTClientHandler::mqttCallback(char* topic, byte* payload, unsigned int length) {
    String topicStr = String(topic);
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload, length);

    if (topicStr == instance->deltaTopic) {
        instance->handleDeltaMessage(doc);
    } else if (topicStr == instance->getAcceptedTopic && !instance->initialShadowReceived) {
        instance->handleGetAcceptedMessage(doc);
    }
}

void MQTTClientHandler::handleDeltaMessage(const JsonDocument& doc) {
    if (doc["state"].containsKey("servoAngle")) {
        int angle = doc["state"]["servoAngle"];
        servoController.moveTo(angle);
        updateShadowReported();
    }
}

void MQTTClientHandler::handleGetAcceptedMessage(const JsonDocument& doc) {
    if (doc["state"]["reported"].containsKey("servoAngle")) {
        int angle = doc["state"]["reported"]["servoAngle"];
        servoController.moveTo(angle);
        initialShadowReceived = true;
    }
}

void MQTTClientHandler::updateShadowReported() {
    StaticJsonDocument<200> doc;
    doc["state"]["reported"]["servoAngle"] = servoController.getAngle();
    doc["state"]["reported"]["timestamp"] = millis();

    char buffer[512];
    serializeJson(doc, buffer);
    client.publish(updateTopic, buffer);
}
