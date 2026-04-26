// мигание светодиодом
#include <Arduino.h>
#include "config.h"

void taskBlink(void *pvParameters) {
    
    while(1) {
        Serial.println("taskBlink - OK");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    // bool state = false;
    // pinMode(LED_PIN, OUTPUT);
    // while (true) {
    //     state = !state;
    //     digitalWrite(LED_PIN, state);
    //     vTaskDelay(DELAY_BLINK / portTICK_PERIOD_MS);
    // }
}