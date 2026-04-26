// веб-сервер
#include <Arduino.h>
#include "config.h"
#include "common.h"

void taskWebServer(void *pvParameters) {
    while(1) {
        Serial.println("taskWebServer - OK");
        vTaskDelay(1300 / portTICK_PERIOD_MS);
    }
}