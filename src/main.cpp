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

// ====================================================================
// setup() - Инициализация системы (выполняется один раз при старте)
// ====================================================================
void setup() {
    // 1. Инициализация Serial для отладки
    Serial.begin(115200);
    delay(1000);
    
    // 2. Настройка встроенного светодиода
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    
    // 3. Инициализация файловой системы LittleFS
    initFileSystem();
    
    // 4. Настройка Watchdog (если включён в config.h)
#if ENABLE_WATCHDOG
    Serial.print("[WDT] Инициализация сторожевого таймера (");
    Serial.print(WATCHDOG_TIMEOUT_MS);
    Serial.println(" мс)");
    
    esp_task_wdt_init(WATCHDOG_TIMEOUT_MS, true);
    esp_task_wdt_add(NULL);
    Serial.println("[WDT] Сторожевой таймер активирован");
#endif
    
    // 5. Запуск всех FreeRTOS задач
    //    Каждая задача привязана к определённому ядру и имеет свой приоритет
    
    // WiFi задача (ядро 0, высший приоритет)
    xTaskCreatePinnedToCore(taskWiFi, "WiFi Task", STACK_WIFI, NULL, PRIO_WIFI, NULL, CORE_WIFI);
    
    // RGB LED задача (ядро 1, высокий приоритет)
    xTaskCreatePinnedToCore(taskBlink, "Blink Task", STACK_BLINK, NULL, PRIO_BLINK, NULL, CORE_BLINK);
    
    // Serial отладка (ядро 0, средний приоритет)
    xTaskCreatePinnedToCore(taskSerial, "Serial Task", STACK_SERIAL, NULL, PRIO_SERIAL, NULL, CORE_SERIAL);
    
    // Веб-сервер (ядро 0, средний приоритет)
    xTaskCreatePinnedToCore(taskWebServer, "WebServer Task", STACK_SERVER, NULL, PRIO_SERVER, NULL, CORE_SERVER);
    
    // Датчики (ядро 1, низкий приоритет)
    xTaskCreatePinnedToCore(taskSensor, "Sensor Task", STACK_SENSOR, NULL, PRIO_SENSOR, NULL, CORE_SENSOR);
    
    // Аудио задача (ядро 1, низкий приоритет, опционально)
#if ENABLE_AUDIO
    xTaskCreatePinnedToCore(taskAudio, "Audio Task", STACK_AUDIO, NULL, PRIO_AUDIO, NULL, CORE_AUDIO);
#endif
}

// ====================================================================
// loop() - Пустой цикл Arduino
// 
// Примечание: Все задачи работают через FreeRTOS, поэтому loop() не нужен.
//            Удаляем эту задачу сразу после запуска.
// ====================================================================
void loop() {
    // Удаляем задачу loop(), так как она не нужна
    vTaskDelete(NULL);
}