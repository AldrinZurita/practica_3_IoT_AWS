#include "LightSensor.h"

LightSensor::LightSensor(int pin) {
    this->pin = pin;
    this->intensity = 0;
    this->level = "medium";
    this->previousLevel = "";
    this->lastReadTime = 0;
}

bool LightSensor::shouldRead() {
    return millis() - lastReadTime >= interval;
}

void LightSensor::read() {
    lastReadTime = millis();
    intensity = analogRead(pin);
    previousLevel = level;

    if (intensity < 300) {
        level = "low";
    } else if (intensity < 700) {
        level = "medium";
    } else {
        level = "high";
    }
}

bool LightSensor::hasLevelChanged() const {
    return level != previousLevel;
}

int LightSensor::getIntensity() const {
    return intensity;
}

String LightSensor::getLevel() const {
    return level;
}