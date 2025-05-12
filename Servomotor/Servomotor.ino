#include "WiFiManager.h"
#include "MQTTClientHandler.h"
#include "ServoController.h"
#include "certificates.h"

const char* WIFI_SSID = "TECHLAB";
const char* WIFI_PASS = "catolica11";

const char* MQTT_BROKER = "a8zrduyiqifqz-ats.iot.us-east-2.amazonaws.com";
const int MQTT_PORT = 8883;
const char* CLIENT_ID = "ESP32-ServoController";
const char* THING_NAME = "Panel_Objeto";

const char* SHADOW_GET_TOPIC = "$aws/things/Panel_Objeto/shadow/get";
const char* SHADOW_GET_ACCEPTED_TOPIC = "$aws/things/Panel_Objeto/shadow/get/accepted";
const char* SHADOW_UPDATE_TOPIC = "$aws/things/Panel_Objeto/shadow/update";
const char* SHADOW_DELTA_TOPIC = "$aws/things/Panel_Objeto/shadow/update/delta";

WiFiManager wifiManager(WIFI_SSID, WIFI_PASS);
ServoController servoController(13);
MQTTClientHandler mqttClient(MQTT_BROKER, MQTT_PORT, CLIENT_ID,
                              SHADOW_GET_TOPIC, SHADOW_GET_ACCEPTED_TOPIC,
                              SHADOW_UPDATE_TOPIC, SHADOW_DELTA_TOPIC,
                              servoController);

void setup() {
    Serial.begin(115200);
    servoController.initialize();
    wifiManager.connect();
    mqttClient.setupCertificates(AMAZON_ROOT_CA1, CERTIFICATE, PRIVATE_KEY);
    mqttClient.initialize();
}

void loop() {
    if (!mqttClient.isConnected()) {
        mqttClient.initialize();
    }
    mqttClient.loop();
    delay(100);
}