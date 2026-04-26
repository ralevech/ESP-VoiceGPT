// вывод в Serial
#include <Arduino.h>
#include <WiFi.h>
#include "config.h"

void taskSerial(void *pvParameters) {
    while(1) {
        Serial.println("taskSerial - OK");
        vTaskDelay(1500 / portTICK_PERIOD_MS);
    }
    // while (true) {
    //     if (WiFi.status() == WL_CONNECTED) {
    //         Serial.print("IP: ");
    //         Serial.println(WiFi.localIP());
    //     } else {
    //         Serial.println("WiFi отключён");
    //     }
    //     vTaskDelay(DELAY_SERIAL_PRINT / portTICK_PERIOD_MS);
    // }
}