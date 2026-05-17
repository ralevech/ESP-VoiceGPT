// ====================================================================
// 02_sensor_core.cpp - Логика опроса датчика DS18B20
// Автор: ralevech
// ====================================================================

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "config.h"
#include "common.h"
#include "globals.h"
#include "sensor_priv.h"

#if ENABLE_SENSOR

// Подключаем WebSocket для отправки температуры на веб-страницу
#if ENABLE_WEBSERVER
    #include "server_priv.h"
#endif

// Пин для подключения DS18B20 (из config.h)
#define ONE_WIRE_BUS DS18B20_PIN

// Объекты для работы с датчиком
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// --------------------------------------------------------------------
// initSensor() - Инициализация датчика при старте
// --------------------------------------------------------------------
void initSensor() {
    Serial.println("[SENSOR] Инициализация DS18B20...");
    
    sensors.begin();  // Поиск датчика на шине
    
    int deviceCount = sensors.getDeviceCount();
    
    if (deviceCount == 0) {
        LOG_ERROR("DS18B20 не найден! Проверьте резистор 4.7кОм");
    } else {
        LOG_INFO("DS18B20 найден! Датчиков: " + String(deviceCount));
        sensors.setResolution(DS18B20_RESOLUTION);
    }
}

// --------------------------------------------------------------------
// getTemperature() - Опрос температуры, возвращает значение
// --------------------------------------------------------------------
float getTemperature() {
    sensors.requestTemperatures();               // Запуск измерения
    float temp = sensors.getTempCByIndex(0);     // Чтение с первого датчика
    
    if (temp == DEVICE_DISCONNECTED_C) {         // Ошибка (-127°C)
        LOG_WARN("Ошибка чтения температуры");
        return lastTemperature;                  // Возвращаем последнее значение
    }
    
    return temp;
}

// --------------------------------------------------------------------
// processSensor() - Циклический опрос (вызывается из задачи)
// --------------------------------------------------------------------
void processSensor() {
    float temperature = getTemperature();
    
    // Сохраняем в глобальную переменную (для API /api/status)
    lastTemperature = temperature;
    
    // Отправляем на веб-страницу через WebSocket
    #if ENABLE_WEBSERVER
    if (serverRunning) {
        extern AsyncWebSocket ws;
        ws.textAll("TEMP:" + String(temperature, 2));
    }
    #endif
    
    // Вывод в Serial для отладки
    Serial.println("[SENSOR] Температура: " + String(temperature) + " °C");
}

#endif