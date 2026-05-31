#ifndef RAILWAYSIGNAL_H
#define RAILWAYSIGNAL_H

#include <Arduino.h>

class RailwaySignal {
private:
    uint8_t led1Pin;
    uint8_t led2Pin;
    uint8_t buzzerPin;

    bool active = false;

    unsigned long prevLed = 0;
    unsigned long prevBuzz = 0;

    bool ledState = false;
    bool buzzState = false;

public:
    void attach(uint8_t led1, uint8_t led2, uint8_t buzzer)
    {
        led1Pin = led1;
        led2Pin = led2;
        buzzerPin = buzzer;

        pinMode(led1Pin, OUTPUT);
        pinMode(led2Pin, OUTPUT);
        pinMode(buzzerPin, OUTPUT);

        digitalWrite(led1Pin, LOW);
        digitalWrite(led2Pin, LOW);
        digitalWrite(buzzerPin, LOW);
    }

    void startWarning()
    {
        active = true;
    }

    void stopWarning()
    {
    active = false;

    digitalWrite(led1Pin, LOW);
    digitalWrite(led2Pin, LOW);
    digitalWrite(buzzerPin, LOW);

    ledState = false;
    buzzState = false;
    }

    void update()
    {
        if (!active) return;

        // LED berkedip bergantian setiap 500 ms
        if (millis() - prevLed >= 500) {
            prevLed = millis();

            ledState = !ledState;

            digitalWrite(led1Pin, ledState);
            digitalWrite(led2Pin, !ledState);
        }

        if (millis() - prevBuzz >= 125) {
            prevBuzz = millis();
            buzzState = !buzzState;
            digitalWrite(buzzerPin, buzzState);
        }
    }
};

#endif
