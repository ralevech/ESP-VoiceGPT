// ====================================================================
// 02_pid_core.cpp - Логика PID регулятора
// Автор: ralevech
// ====================================================================

#include <Arduino.h>
#include "config.h"
#include "common.h"
#include "pid_priv.h"

#if ENABLE_PID

static PIDController pid(PID_DT);
static float fakeValue = 25.0;

void initPID() {
    pid.setGains(PID_Kp, PID_Ki, PID_Kd);
    pid.setIntegralLimits(PID_INTEGRAL_MIN, PID_INTEGRAL_MAX);
    pid.setOutputLimits(PID_OUTPUT_MIN, PID_OUTPUT_MAX);
    pid.reset();
    
    LOG_INFO("PID задача запущена");
    LOG_INFO("Kp=" + String(PID_Kp) + ", Ki=" + String(PID_Ki) + ", Kd=" + String(PID_Kd));
    LOG_INFO("Target=" + String(PID_TARGET));
}

float readSensor() {
    // TODO: Заменить на реальный датчик
    fakeValue += (random(-10, 10) / 100.0);
    fakeValue = constrain(fakeValue, 20, 80);
    return fakeValue;
}

void controlActuator(float output) {
    // TODO: Заменить на реальный актуатор
    LOG_DEBUG("PID Output: " + String(output, 2));
}

void processPID() {
    float current = readSensor();
    float output = pid.compute(PID_TARGET, current);
    controlActuator(output);
    
    LOG_DEBUG("Target=" + String(PID_TARGET) + 
              ", Current=" + String(current) + 
              ", Error=" + String(pid.getError(), 2) + 
              ", Output=" + String(output, 2));
}

#endif