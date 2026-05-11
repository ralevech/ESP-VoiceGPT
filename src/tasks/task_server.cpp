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

#if ENABLE_WEBSERVER

// Глобальный объект сервера
static AsyncWebServer* server = nullptr;

// Внешние функции (определены в task_wifi.cpp)
extern bool isWiFiConnected();
extern bool isAPMode();

// --------------------------------------------------------------------
// Обработчики API запросов
// --------------------------------------------------------------------

static void handleLedOn(AsyncWebServerRequest *request) {
    ledState = true;
    request->send(200, "text/plain; charset=utf-8", "LED включен");
}

static void handleLedOff(AsyncWebServerRequest *request) {
    ledState = false;
    request->send(200, "text/plain; charset=utf-8", "LED выключен");
}

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

static void setupRoutes(AsyncWebServer* srv) {
    srv->on("/api/led/on", HTTP_GET, handleLedOn);
    srv->on("/api/led/off", HTTP_GET, handleLedOff);
    srv->on("/api/status", HTTP_GET, handleStatus);
    
    srv->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/index.html", "text/html; charset=utf-8");
    });
    
    srv->serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
    
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
    
    esp_task_wdt_add(NULL);
    
    while (!isFileSystemReady()) {
        vTaskDelay(pdMS_TO_TICKS(100));
        feedWatchdog();
    }
    
    while (!isWiFiConnected() && !isAPMode()) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        feedWatchdog();
    }
    
    startServer();
    
    while (true) {
        feedWatchdog();
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

#endif // ENABLE_WEBSERVER