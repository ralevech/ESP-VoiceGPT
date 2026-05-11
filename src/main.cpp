// ====================================================================
// main.cpp - Главный файл проекта ESP32-Skeleton
// 
// Описание: Точка входа в программу. Инициализация системы,
//           настройка Watchdog и запуск всех FreeRTOS задач.
// Автор: ralevech
// ====================================================================

#include <Arduino.h>
#include <esp_task_wdt.h>
#include "config.h"
#include "common.h"
#include "tasks.h"

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    
    initFileSystem();
    
#if ENABLE_WATCHDOG
    LOG_INFO("Инициализация Watchdog (" + String(WATCHDOG_TIMEOUT_MS) + " мс)");
    esp_task_wdt_init(WATCHDOG_TIMEOUT_MS, true);
    esp_task_wdt_add(NULL);
    LOG_INFO("Watchdog активирован");
#endif
    
    // ===== ЗАПУСК FreeRTOS ЗАДАЧ =====
    
    // WiFi задача (ядро 0, высший приоритет) — управление подключением и AP режимом
    xTaskCreatePinnedToCore(taskWiFi, "WiFi Task", STACK_WIFI, NULL, PRIO_WIFI, NULL, CORE_WIFI);
    
    // RGB LED задача (ядро 1, высокий приоритет) — эффект радуги на WS2812
    xTaskCreatePinnedToCore(taskBlink, "Blink Task", STACK_BLINK, NULL, PRIO_BLINK, NULL, CORE_BLINK);
    
    // Serial отладка (ядро 0, средний приоритет) — вывод статуса системы
    xTaskCreatePinnedToCore(taskSerial, "Serial Task", STACK_SERIAL, NULL, PRIO_SERIAL, NULL, CORE_SERIAL);
 
    // Веб-сервер (ядро 0, средний приоритет) — HTTP API и WebSocket
#if ENABLE_WEBSERVER
    xTaskCreatePinnedToCore(taskWebServer, "WebServer Task", STACK_SERVER, NULL, PRIO_SERVER, NULL, CORE_SERVER);
#endif

    // Датчики (ядро 1, низкий приоритет) — заглушка для будущих датчиков
#if ENABLE_SENSOR
    xTaskCreatePinnedToCore(taskSensor, "Sensor Task", STACK_SENSOR, NULL, PRIO_SENSOR, NULL, CORE_SENSOR);
#endif

    // Аудио задача (ядро 1, низкий приоритет) — MP3, TTS, радио
#if ENABLE_AUDIO
    xTaskCreatePinnedToCore(taskAudio, "Audio Task", STACK_AUDIO, NULL, PRIO_AUDIO, NULL, CORE_AUDIO);
#endif

    // PID регулятор (ядро 1, низкий приоритет) — регулирование температуры, скорости, освещения
    // Коэффициенты Kp, Ki, Kd настраиваются в config.h
#if ENABLE_PID
    xTaskCreatePinnedToCore(taskPID, "PID Task", STACK_PID, NULL, PRIO_PID, NULL, CORE_PID);
#endif
}

void loop() {
    // Все задачи работают через FreeRTOS, loop() отключаем.
    vTaskDelete(NULL);
}