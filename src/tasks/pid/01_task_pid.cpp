// ====================================================================
// 01_task_pid.cpp - Задача PID регулятора
// Автор: ralevech
// ====================================================================

#include <Arduino.h>
#include <esp_task_wdt.h>
#include "config.h"
#include "common.h"
#include "pid_priv.h"

#if ENABLE_PID

void taskPID(void *pvParameters) {
    (void)pvParameters;
    
    esp_task_wdt_add(NULL);
    initPID();
    
    unsigned long lastTime = millis();
    
    while (true) {
        feedWatchdog();
        
        unsigned long now = millis();
        if ((now - lastTime) >= (PID_DT * 1000)) {
            lastTime = now;
            processPID();
        }
        
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

#endif