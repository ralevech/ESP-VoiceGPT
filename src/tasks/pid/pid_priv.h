// ====================================================================
// pid_priv.h - Внутренний API модуля PID регулятора
// Автор: ralevech
// ====================================================================

#ifndef PID_PRIV_H
#define PID_PRIV_H

#include <Arduino.h>
#include "pid_controller.h"

void initPID();
void processPID();
float readSensor();
void controlActuator(float output);

#endif