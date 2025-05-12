#include "LightSensor.h"
#include "WiFiManager.h"
#include "AWSClient.h"
#include "certificates.h"

const char* WIFI_SSID = "TECHLAB";
const char* WIFI_PASS = "catolica11";

const char* MQTT_BROKER = "a8zrduyiqifqz-ats.iot.us-east-2.amazonaws.com";
const int MQTT_PORT = 8883;
const char* CLIENT_ID = "ESP32-SolarPanel";
const char* UPDATE_TOPIC = "$aws/things/Panel_Objeto/shadow/update";
const char* GET_TOPIC = "$aws/things/Panel_Objeto/shadow/get";

LightSensor lightSensor(34);
WiFiManager wifiManager(WIFI_SSID, WIFI_PASS);
AWSClient awsClient(MQTT_BROKER, MQTT_PORT, CLIENT_ID, UPDATE_TOPIC, GET_TOPIC);

void setup() {
    Serial.begin(115200);
    wifiManager.connect();
    awsClient.setupCertificates(AMAZON_ROOT_CA1, CERTIFICATE, PRIVATE_KEY);
    awsClient.initialize();

    lightSensor.read();
    Serial.print("Initial light level: ");
    Serial.println(lightSensor.getLevel());
}

void loop() {
    if (!awsClient.isConnected()) {
        awsClient.initialize();
    }
    awsClient.loop();

    if (lightSensor.shouldRead()) {
        lightSensor.read();
        awsClient.updateShadowIfChanged(lightSensor);
    }
}