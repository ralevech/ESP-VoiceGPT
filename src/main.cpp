// ====================================================================
// main.cpp - Главный файл проекта ESP32-Skeleton
// 
// Описание: Точка входа в программу. Инициализация системы и запуск
//           всех FreeRTOS задач.
// Автор: ralevech
// ====================================================================

#include <Arduino.h>
#include <esp_task_wdt.h>
#include "config.h"
#include "common.h"
#include "tasks.h"

// ====================================================================
// setup() - Инициализация системы
// ====================================================================
void setup() {
    // Инициализация Serial порта
    Serial.begin(115200);
    delay(1000);
    
    // Настройка встроенного светодиода (обычный, не RGB)
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    
    // Инициализация файловой системы
    initFileSystem();
    
    // ===== ИНИЦИАЛИЗАЦИЯ WATCHDOG (старый API для совместимости) =====
    #if ENABLE_WATCHDOG
        Serial.print("[WDT] Инициализация сторожевого таймера (");
        Serial.print(WATCHDOG_TIMEOUT_MS);
        Serial.println(" мс)");
        
        // Инициализация watchdog с таймаутом
        esp_task_wdt_init(WATCHDOG_TIMEOUT_MS, true);
        
        // Добавляем текущую задачу в watchdog
        esp_task_wdt_add(NULL);
        
        Serial.println("[WDT] Сторожевой таймер активирован");
    #endif
    
    // ===== ЗАПУСК FreeRTOS ЗАДАЧ =====
    
    // Задача управления WiFi (подключение к сети или AP режим)
    xTaskCreatePinnedToCore(
        taskWiFi,           // функция задачи
        "WiFi Task",        // имя задачи
        STACK_WIFI,         // размер стека
        NULL,               // параметры
        PRIO_WIFI,          // приоритет
        NULL,               // хэндл
        CORE_WIFI           // ядро
    );

    // Задача управления RGB светодиодом
    xTaskCreatePinnedToCore(
        taskBlink,
        "Blink Task",
        STACK_BLINK,
        NULL,
        PRIO_BLINK,
        NULL,
        CORE_BLINK
    );

    // Задача вывода отладочной информации
    xTaskCreatePinnedToCore(
        taskSerial,
        "Serial Task",
        STACK_SERIAL,
        NULL,
        PRIO_SERIAL,
        NULL,
        CORE_SERIAL
    );

    // Задача веб-сервера
    xTaskCreatePinnedToCore(
        taskWebServer,
        "WebServer Task",
        STACK_SERVER,
        NULL,
        PRIO_SERVER,
        NULL,
        CORE_SERVER
    );

    // Задача опроса датчиков
    xTaskCreatePinnedToCore(
        taskSensor,
        "Sensor Task",
        STACK_SENSOR,
        NULL,
        PRIO_SENSOR,
        NULL,
        CORE_SENSOR
    );
}

// ====================================================================
// loop() - Пустой цикл Arduino
// ====================================================================
void loop() {
    #if ENABLE_WATCHDOG
        feedWatchdog();
    #endif

    vTaskDelete(NULL);
}