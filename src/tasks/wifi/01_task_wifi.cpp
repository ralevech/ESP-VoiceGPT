// ====================================================================
// 01_task_wifi.cpp - Задача управления WiFi
// Автор: ralevech
// ====================================================================

#include <Arduino.h>
#include <esp_task_wdt.h>
#include "config.h"
#include "common.h"
#include "wifi_priv.h"

void taskWiFi(void *pvParameters) {
    (void)pvParameters;
    
    esp_task_wdt_add(NULL);                             // Регистрация в Watchdog
    vTaskDelay(pdMS_TO_TICKS(500));                     // Задержка при старте
    
    initWiFi();                                         // Инициализация WiFi
    
    while (true) {
        processWiFiEvents();                            // Проверка статуса и переподключение
        vTaskDelay(pdMS_TO_TICKS(DELAY_WIFI_CHECK));    // Пауза между проверками
        feedWatchdog();                                 // Сброс Watchdog
    }
}