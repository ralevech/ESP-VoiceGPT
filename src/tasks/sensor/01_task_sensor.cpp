// ====================================================================
// 01_task_sensor.cpp - Задача опроса датчиков (FreeRTOS)
// Автор: ralevech
// ====================================================================

#include <Arduino.h>
#include <esp_task_wdt.h>
#include "config.h"
#include "common.h"
#include "globals.h"
#include "tasks.h"
#include "sensor_priv.h"

#if ENABLE_SENSOR

void taskSensor(void *pvParameters) {
    (void)pvParameters;
    
    // Регистрация в Watchdog
    esp_task_wdt_add(NULL);
    
    // Ожидание готовности файловой системы
    while (!isFileSystemReady()) {
        vTaskDelay(pdMS_TO_TICKS(100));
        feedWatchdog();
    }
    
    // Инициализация датчика
    initSensor();
    
    // Основной цикл
    while (true) {
        processSensor();                              // Опрос температуры
        vTaskDelay(pdMS_TO_TICKS(DELAY_SENSOR_CHECK)); // Задержка из config.h
        feedWatchdog();                               // Сброс Watchdog
    }
}

#endif