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



//  ===== БЫСТРЫ ТЕСТ РАБОТОСПОСОБНОСТИ ЧИПА =====
// Подключаем библиотеку для управления адресными светодиодами
// #include <Adafruit_NeoPixel.h>

// // Указываем пин, к которому подключен светодиод
// #define LED_PIN    48
// // Указываем, что у нас 1 светодиод
// #define LED_COUNT  1

// // Создаем объект для управления светодиодом
// Adafruit_NeoPixel pixels(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// void setup() {
//   Serial.begin(115200);
//   pixels.begin(); // Инициализируем светодиод
//   Serial.println("Setup готов. Начинаем мигать!");
// }

// void loop() {
//   // Включаем зеленый свет
//   pixels.setPixelColor(0, pixels.Color(0, 150, 0)); 
//   pixels.show();
//   Serial.println("LED ON");
//   delay(500);

//   // Выключаем свет
//   pixels.clear(); 
//   pixels.show();
//   Serial.println("LED OFF");
//   Serial.println("Running...");
//   delay(500);
// }