#ifndef RAILWAYSIGNAL_H
#define RAILWAYSIGNAL_H

#include <Arduino.h>

enum BuzzerType
{
    Ac,
    Pv
};

class RailwaySignal {
private:
    uint8_t led1Pin;
    uint8_t led2Pin;
    uint8_t buzzerPin;

    BuzzerType buzzerType;
    bool active = false;

    unsigned long prevLed = 0;
    unsigned long prevBuzz = 0;

    bool ledState = false;
    bool buzzState = false;

public:
    void attach(
        uint8_t led1,
        uint8_t led2,
        uint8_t buzzer,
        BuzzerType type)
    {
        led1Pin = led1;
        led2Pin = led2;
        buzzerPin = buzzer;
    
        buzzerType = type;
    
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

    void update(
        const int ledTime, 
        const int buzzTime)
    {
        if (!active) return;
        if (millis() - prevLed >= ledTime) {
            prevLed = millis();

            ledState = !ledState;

            digitalWrite(led1Pin, ledState);
            digitalWrite(led2Pin, !ledState);
        }
        
        if (millis() - prevBuzz >= buzzTime)
        {
            prevBuzz = millis();
            buzzState = !buzzState;
            if (buzzerType == Ac) digitalWrite(buzzerPin, buzzState);
            else 
            {
                if (buzzState) tone(buzzerPin, 1200);
                else tone(buzzerPin, 800);
            }
        }
    }
};

class PIDController {
private:
  float kp;
  float ki;
  float kd;
  
  float integral;
  float prevError;
  
  float outputMin;
  float outputMax;
  
  unsigned long lastTime;
  bool firstRun;
  
public:
  PIDController(float p, float i, float d) 
    : kp(p), ki(i), kd(d), integral(0), prevError(0),
      outputMin(-1e6), outputMax(1e6), lastTime(0), firstRun(true) {
  }
  
  void setGains(float p, float i, float d) {
    kp = p; // set kp ki kd dari object(p, i, d)
    ki = i;
    kd = d;
  }
  
  void setOutputLimits(float min, float max) {
    outputMin = min;
    outputMax = max;
  }
  
  float compute(float setpoint, float measurement) {
    unsigned long currentTime = millis();
    
    if (firstRun) {
      lastTime = currentTime;
      prevError = setpoint - measurement;
      firstRun = false;
      return 0;
    }
    
    float dt = (currentTime - lastTime) / 1000.0f; // convert to seconds
    lastTime = currentTime;
    
    if (dt <= 0) dt = 0.01f; // prevent division by zero
    
    // Calculate error
    float error = setpoint - measurement;
    
    // Proportional term
    float pTerm = kp * error;
    
    // Integral term
    integral += error * dt;
    float iTerm = ki * integral;
    
    // Derivative term
    float derivative = (error - prevError) / dt;
    float dTerm = kd * derivative;
    
    prevError = error;
    
    // Calculate output
    float output = pTerm + iTerm + dTerm;
    
    // Apply output limits
    if (output > outputMax) {
      output = outputMax;
      // Anti-windup: stop integrating if saturated
      integral -= error * dt;
    } else if (output < outputMin) {
      output = outputMin;
      // Anti-windup
      integral -= error * dt;
    }
    
    return output;
  }
  
  void reset() {
    integral = 0;
    prevError = 0;
    firstRun = true;
  }
};

#endif
