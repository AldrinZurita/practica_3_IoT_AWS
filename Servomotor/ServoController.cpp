#include "ServoController.h"
#include <Arduino.h>

ServoController::ServoController(int pin) : pin(pin), currentAngle(0) {}

void ServoController::initialize() {
    ESP32PWM::allocateTimer(0);
    servo.setPeriodHertz(50);
    servo.attach(pin, 500, 2400);
    servo.write(currentAngle);
    Serial.println("Servo initialized at 0°");
}

void ServoController::moveTo(int angle) {
    angle = constrain(angle, 0, 180);
    if (angle != currentAngle) {
        currentAngle = angle;
        servo.write(currentAngle);
        Serial.print("Servo moved to: ");
        Serial.print(currentAngle);
        Serial.println("°");
    }
}

int ServoController::getAngle() const {
    return currentAngle;
}