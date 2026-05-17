// ====================================================================
// 01_task_sensor.cpp - Задача опроса датчиков
// Автор: ralevech
// ====================================================================

#include <Arduino.h>
#include <esp_task_wdt.h>
#include "config.h"
#include "common.h"
#include "sensor_priv.h"

#if ENABLE_SENSOR

void taskSensor(void *pvParameters) {
    (void)pvParameters;
    
    esp_task_wdt_add(NULL);
    initSensor();
    
    while (true) {
        processSensor();
        vTaskDelay(pdMS_TO_TICKS(DELAY_SENSOR_CHECK));
        feedWatchdog();
    }
}

#endif