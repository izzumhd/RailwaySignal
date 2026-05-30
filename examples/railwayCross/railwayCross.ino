#include <Servo.h>
#include "RailwaySignal.h"

#define BTN_NORMAL PA3
#define BTN_TRAIN  PA2
#define SERVO_PIN  PA1

Servo gate;
RailwaySignal sinyal;

bool trainComing = false;
unsigned long warningStart = 0, prevServo = 0;
int currentAngle = 90, targetAngle  = 90;

void updateServo() {
    if (millis() - prevServo >= 30) {
        prevServo = millis();
        if (currentAngle < targetAngle) {
            currentAngle++; gate.write(currentAngle);
        } else if (currentAngle > targetAngle) {
            currentAngle--; gate.write(currentAngle);
        }
    }
}

void setup() {
    pinMode(BTN_NORMAL, INPUT_PULLUP);
    pinMode(BTN_TRAIN, INPUT_PULLUP);

    gate.attach(SERVO_PIN);
    gate.write(currentAngle);
    sinyal.attach(PA3, PA4, PA5); // (led1, led2, buzzer)
}

void loop() {
    if (!digitalRead(BTN_TRAIN) && !trainComing) {
        sinyal.startWarning();
        trainComing = true;
        warningStart = millis();
    }
    if (trainComing && millis() - warningStart >= 3000) 
        targetAngle = 0;
    if (!digitalRead(BTN_NORMAL)) {
        targetAngle = 90;
        sinyal.stopWarning();
        trainComing = false;
        warningStart = 0;
    }
    sinyal.update(); updateServo();
}