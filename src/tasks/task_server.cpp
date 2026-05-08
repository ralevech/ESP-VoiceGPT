// ====================================================================
// task_server.cpp - Задача веб-сервера
// 
// Описание: Асинхронный веб-сервер для обслуживания HTML страниц
//           и обработки API запросов. Работает как в STA режиме, так и в AP.
// Автор: ralevech
// ====================================================================

#include <Arduino.h>
#include <esp_task_wdt.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include "config.h"
#include "common.h"
#include "tasks.h"

// Глобальный объект сервера
static AsyncWebServer* server = nullptr;

// Внешние функции (определены в task_wifi.cpp)
extern bool isWiFiConnected();
extern bool isAPMode();

// --------------------------------------------------------------------
// Обработчики API запросов
// --------------------------------------------------------------------

// Обработчик: включение светодиода
static void handleLedOn(AsyncWebServerRequest *request) {
    ledState = true;
    request->send(200, "text/plain; charset=utf-8", "LED включен");
}

// Обработчик: выключение светодиода
static void handleLedOff(AsyncWebServerRequest *request) {
    ledState = false;
    request->send(200, "text/plain; charset=utf-8", "LED выключен");
}

// Обработчик: получение статуса
static void handleStatus(AsyncWebServerRequest *request) {
    String status = "{"
        "\"status\": \"ok\","
        "\"uptime\": " + String(millis() / 1000) + ","
        "\"heap\": " + String(ESP.getFreeHeap()) + ","
        "\"wifi\": \"" + String(isWiFiConnected() ? "connected" : "disconnected") + "\","
        "\"ap_mode\": " + String(apMode ? "true" : "false") + ","
        "\"led_state\": " + String(ledState ? "true" : "false") +
    "}";
    request->send(200, "application/json; charset=utf-8", status);
}

// --------------------------------------------------------------------
// Настройка маршрутов (роутов) веб-сервера
// --------------------------------------------------------------------
static void setupRoutes(AsyncWebServer* srv) {
    // API ENDPOINTS (JSON)
    srv->on("/api/led/on", HTTP_GET, handleLedOn);
    srv->on("/api/led/off", HTTP_GET, handleLedOff);
    srv->on("/api/status", HTTP_GET, handleStatus);
    
    // СТАТИЧЕСКИЕ ФАЙЛЫ
    srv->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/index.html", "text/html; charset=utf-8");
    });
    
    srv->serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
    
    // ОБРАБОТКА ОШИБОК 404
    srv->onNotFound([](AsyncWebServerRequest *request) {
        String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>"
                      "<title>404</title></head><body>"
                      "<h1>404 - Страница не найдена</h1>"
                      "<p>Запрошенный URL: " + String(request->url()) + "</p>"
                      "<a href='/'>Вернуться на главную</a>"
                      "</body></html>";
        request->send(404, "text/html; charset=utf-8", html);
    });
}

// --------------------------------------------------------------------
// Запуск веб-сервера
// --------------------------------------------------------------------
static void startServer() {
    if (server != nullptr) {
        server->end();
        delete server;
        server = nullptr;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    
    server = new AsyncWebServer(80);
    setupRoutes(server);
    server->begin();
    serverRunning = true;
}

// ====================================================================
// taskWebServer() - Основная задача веб-сервера
// ====================================================================
void taskWebServer(void *pvParameters) {
    (void)pvParameters;
    
    // Регистрация в Watchdog
    esp_task_wdt_add(NULL);
    
    // Ждём монтирование файловой системы
    while (!isFileSystemReady()) {
        vTaskDelay(pdMS_TO_TICKS(100));
        feedWatchdog();
    }
    
    // Ждём готовности сети (STA или AP)
    while (!isWiFiConnected() && !isAPMode()) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        feedWatchdog();
    }
    
    // Запуск сервера
    startServer();
    
    // Основной цикл
    while (true) {
        feedWatchdog();
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}