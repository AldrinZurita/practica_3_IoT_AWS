#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <Arduino.h>

class LightSensor {
private:
    int pin;
    int intensity;
    String level;
    String previousLevel;
    unsigned long lastReadTime;
    const int interval = 333;

public:
    LightSensor(int pin);
    void read();
    bool hasLevelChanged() const;
    int getIntensity() const;
    String getLevel() const;
    bool shouldRead();
};

#endif