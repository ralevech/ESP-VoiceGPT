// ====================================================================
// task_sensor.cpp - Задача опроса датчиков
// 
// Описание: Заглушка для будущих датчиков. При необходимости
//           раскомментировать код для конкретных датчиков.
// Автор: ralevech
// ====================================================================

#include <Arduino.h>
#include <esp_task_wdt.h>
#include "config.h"
#include "common.h"

#if ENABLE_SENSOR

// ====================================================================
// taskSensor() - Задача опроса датчиков
// 
// Алгоритм:
//   1. Регистрация в Watchdog
//   2. Вывод сообщения о запуске
//   3. Бесконечный цикл с периодическим опросом
//   4. Сброс watchdog при каждой итерации
// ====================================================================
void taskSensor(void *pvParameters) {
    (void)pvParameters;
    
    // Регистрация в Watchdog
    esp_task_wdt_add(NULL);
    
    Serial.println("[SENSOR] Задача запущена (заглушка для будущих датчиков)");
    
    while(true) {
        // TODO: Здесь будет опрос датчиков
        // Примеры:
        // float temperature = readTemperature();
        // float humidity = readHumidity();
        // int distance = readUltrasonic();
        
        // Сброс watchdog (задача жива)
        feedWatchdog();

        // Ожидание перед следующим опросом
        vTaskDelay(pdMS_TO_TICKS(DELAY_SENSOR_CHECK));
    }
}

#endif // ENABLE_SENSOR