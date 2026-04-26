// чтение датчиков
#include <Arduino.h>
#include "config.h"

void taskSensor(void *pvParameters) {
    while(1) {
        Serial.println("taskSensor - OK");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}