// ====================================================================
// 01_task_blink.cpp - Задача управления RGB LED
// Автор: ralevech
// ====================================================================

#include <Arduino.h>
#include <esp_task_wdt.h>
#include "config.h"
#include "common.h"
#include "blink_priv.h"

void taskBlink(void *pvParameters) {
    (void)pvParameters;
    
    esp_task_wdt_add(NULL);     // Регистрация в Watchdog
    initBlink();                // Инициализация RGB LED
    
    while (true) {
        processBlink();         // Обновление цвета (эффект радуги)
        vTaskDelay(pdMS_TO_TICKS(10));  // Задержка 10 мс для плавности
        feedWatchdog();         // Сброс Watchdog
    }
}