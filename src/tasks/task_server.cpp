// ====================================================================
// task_server.cpp - Задача веб-сервера
// 
// Описание: Асинхронный веб-сервер для обслуживания HTML страниц
//           и обработки API запросов
// Автор: ralevech
// ====================================================================

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include "config.h"
#include "common.h"

// Глобальный объект сервера
static AsyncWebServer server(80);

// --------------------------------------------------------------------
// Обработчики API запросов
// --------------------------------------------------------------------

// Обработчик: включение светодиода
static void handleLedOn(AsyncWebServerRequest *request) {
    ledState = true;
    Serial.println("[WEB] LED ON запрос получен");
    
    // Здесь будет код управления светодиодом
    // digitalWrite(LED_PIN, HIGH);
    
    request->send(200, "text/plain; charset=utf-8", "LED включен");
}

// Обработчик: выключение светодиода
static void handleLedOff(AsyncWebServerRequest *request) {
    ledState = false;
    Serial.println("[WEB] LED OFF запрос получен");
    
    // Здесь будет код управления светодиодом
    // digitalWrite(LED_PIN, LOW);
    
    request->send(200, "text/plain; charset=utf-8", "LED выключен");
}

// Обработчик: получение статуса
static void handleStatus(AsyncWebServerRequest *request) {
    Serial.println("[WEB] STATUS запрос получен");
    
    String status = "{"
        "\"status\": \"ok\","
        "\"uptime\": " + String(millis() / 1000) + ","
        "\"heap\": " + String(ESP.getFreeHeap()) + ","
        "\"wifi\": \"" + String(WiFi.isConnected() ? "connected" : "disconnected") + "\","
        "\"led_state\": " + String(ledState ? "true" : "false") +
    "}";
    
    request->send(200, "application/json; charset=utf-8", status);
}

// --------------------------------------------------------------------
// Настройка маршрутов (роутов) веб-сервера
// --------------------------------------------------------------------
static void setupRoutes() {
    // ===== API ENDPOINTS (JSON) =====
    server.on("/api/led/on", HTTP_GET, handleLedOn);
    server.on("/api/led/off", HTTP_GET, handleLedOff);
    server.on("/api/status", HTTP_GET, handleStatus);
    
    // ===== СТАТИЧЕСКИЕ ФАЙЛЫ =====
    // Главная страница
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/index.html", "text/html; charset=utf-8");
    });
    
    // Обработка запросов к файлам в папке data
    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
    
    // ===== ОБРАБОТКА ОШИБОК =====
    server.onNotFound([](AsyncWebServerRequest *request) {
        Serial.print("[WEB] 404 Not Found: ");
        Serial.println(request->url());
        
        String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>"
                      "<title>404</title></head><body>"
                      "<h1>404 - Страница не найдена</h1>"
                      "<p>Запрошенный URL: " + String(request->url()) + "</p>"
                      "<a href='/'>Вернуться на главную</a>"
                      "</body></html>";
        
        request->send(404, "text/html; charset=utf-8", html);
    });
}

// ====================================================================
// taskWebServer() - Основная задача веб-сервера
// ====================================================================
void taskWebServer(void *pvParameters) {
    (void)pvParameters;
    
    // Ждем монтирование файловой системы
    while (!isFileSystemReady()) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    // Ждем подключения WiFi
    while (!isWiFiConnected()) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // Настройка маршрутов и запуск сервера
    setupRoutes();
    server.begin();
    
    Serial.println("[WEB] Сервер запущен!");
    Serial.print("[WEB] http://");
    Serial.println(WiFi.localIP());
    
    // Основной цикл
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}