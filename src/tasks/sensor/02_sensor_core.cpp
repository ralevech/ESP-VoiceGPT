// ====================================================================
// 02_sensor_core.cpp - Логика опроса датчиков (заглушка)
// Автор: ralevech
// ====================================================================

#include <Arduino.h>
#include "config.h"
#include "common.h"
#include "sensor_priv.h"

#if ENABLE_SENSOR

void initSensor() {
    LOG_INFO("Задача датчиков запущена (заглушка)");
}

void processSensor() {
    // TODO: Добавить опрос реальных датчиков
    // Примеры:
    // float temp = readTemperature();
    // float hum = readHumidity();
    // int distance = readUltrasonic();
}

#endif