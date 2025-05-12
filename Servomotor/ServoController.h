#ifndef SERVO_CONTROLLER_H
#define SERVO_CONTROLLER_H

#include <ESP32Servo.h>

class ServoController {
private:
    Servo servo;
    int pin;
    int currentAngle;

public:
    ServoController(int pin);
    void initialize();
    void moveTo(int angle);
    int getAngle() const;
};

#endif