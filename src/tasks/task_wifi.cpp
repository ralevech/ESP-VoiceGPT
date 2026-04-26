// WiFi контроль
#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "common.h"

void taskWiFi(void *pvParameters) {
    while(1) {
        Serial.println("taskWiFi - OK");
        vTaskDelay(1900 / portTICK_PERIOD_MS);
    }
//     bool connected = false;
//     // Первоначальное подключение
//     Serial.print("По5дключение к ");
//    // Serial.println(WIFI_SSID);
    
//     WiFi.mode(WIFI_STA);
//     WiFi.begin(WIFI_SSID, WIFI_PASS);
    
//     for (int attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
//         Serial.print("Попытка ");
//         Serial.print(attempt);
//         Serial.print(" из ");
//         Serial.print(MAX_ATTEMPTS);
//         Serial.print("... ");
        
//         unsigned long start = millis();
//         while (millis() - start < ATTEMPT_TIMEOUT_MS) {
//             if (WiFi.status() == WL_CONNECTED) {
//                 Serial.println("OK");
//                 Serial.print("IP: ");
//                 Serial.println(WiFi.localIP());
//                 apMode = false;
//                 connected = true;
//                 break;
//             }
//             delay(100);
//         }
//         if (connected) break;
//         Serial.println("таймаут");
//     }
    
//     // Если не удалось подключиться — запускаем точку доступа
//     if (!connected) {
//         Serial.println("Не удалось подключиться к домашнему WiFi");
//         Serial.println("Запуск точки доступа");
        
//         apMode = true;
//         WiFi.mode(WIFI_AP);
//         WiFi.softAP(AP_SSID, AP_PASS);
//         Serial.print("SSID: ");
//         Serial.println(AP_SSID);
//         Serial.print("Пароль: ");
//         Serial.println(AP_PASS);
//         Serial.print("IP: ");
//         Serial.println(WiFi.softAPIP());
//     }
    
//     // Бесконечный цикл контроля подключения
//     while (true) {
//         if (!apMode && WiFi.status() != WL_CONNECTED) {
//             Serial.println("[WiFi] Соединение потеряно. Переподключаюсь...");
//             WiFi.reconnect();
//         }
//         vTaskDelay(DELAY_WIFI_CHECK / portTICK_PERIOD_MS);
//     }
}