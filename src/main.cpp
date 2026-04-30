#include <Arduino.h>
#include "config.h"
#include "common.h"
#include "tasks.h"

void setup() {
    Serial.begin(115200);
    delay(3000);
    
    Serial.println("\n=== ESP32-S3 СТАРТ ===");
    Serial.print("Чип: ");
    Serial.println(ESP.getChipModel());
    
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    
    initFileSystem();
    
    // Запуск задач
    xTaskCreatePinnedToCore(taskWiFi, "WiFiCheck", STACK_WIFI, NULL, PRIO_WIFI, NULL, CORE_WIFI);
    xTaskCreatePinnedToCore(taskBlink, "Blink", STACK_BLINK, NULL, PRIO_BLINK, NULL, CORE_BLINK);
    xTaskCreatePinnedToCore(taskSerial, "SerialPrint", STACK_SERIAL, NULL, PRIO_SERIAL, NULL, CORE_SERIAL);
    xTaskCreatePinnedToCore(taskWebServer, "WebServer", STACK_SERVER, NULL, PRIO_SERVER, NULL, CORE_SERVER);
    xTaskCreatePinnedToCore(taskSensor, "Sensor", STACK_SENSOR, NULL, PRIO_SENSOR, NULL, CORE_SENSOR);
}

void loop() {
    delay(10000);
    vTaskDelete(NULL);
}