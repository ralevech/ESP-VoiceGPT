// ====================================================================
// server_core.cpp - Публичные функции веб-сервера
// Автор: ralevech
// ====================================================================

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include "config.h"
#include "common.h"
#include "globals.h"
#include "tasks.h" 
#include "server_priv.h"
#include "tasks.h"

#if ENABLE_WEBSERVER

AsyncWebServer* server = nullptr;
AsyncWebSocket ws("/ws");

// --------------------------------------------------------------------
// initServer() - Запуск веб-сервера на порту 80
// --------------------------------------------------------------------
void initServer() {
    if (server != nullptr) {
        server->end();
        delete server;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    
    server = new AsyncWebServer(80);
    setupRoutes();
    server->begin();
    serverRunning = true;
    LOG_INFO("Веб-сервер запущен на порту 80");
}

// --------------------------------------------------------------------
// handleLedOn() - Включить LED
// --------------------------------------------------------------------
void handleLedOn(AsyncWebServerRequest *request) {
    ledState = true;
    digitalWrite(LED_PIN, HIGH);
    request->send(200, "text/plain", "LED ON");
    ws.textAll("LED:ON");
}

// --------------------------------------------------------------------
// handleLedOff() - Выключить LED
// --------------------------------------------------------------------
void handleLedOff(AsyncWebServerRequest *request) {
    ledState = false;
    digitalWrite(LED_PIN, LOW);
    request->send(200, "text/plain", "LED OFF");
    ws.textAll("LED:OFF");
}

// --------------------------------------------------------------------
// handleStatus() - JSON статус системы
// --------------------------------------------------------------------

void handleStatus(AsyncWebServerRequest *request) {
    String json = "{";
    json += "\"led\":" + String(ledState ? "true" : "false") + ",";
    json += "\"wifi\":" + String(isWiFiConnected() ? "true" : "false") + ",";
    json += "\"temp\":" + String(lastTemperature) + ","; // температура
    json += "\"heap\":" + String(ESP.getFreeHeap());
    json += "}";
    request->send(200, "application/json", json);
}

// --------------------------------------------------------------------
// setupRoutes() - Настройка маршрутов HTTP
// --------------------------------------------------------------------
void setupRoutes() {
    server->on("/api/led/on", HTTP_GET, handleLedOn);
    server->on("/api/led/off", HTTP_GET, handleLedOff);
    server->on("/api/status", HTTP_GET, handleStatus);
    
    server->serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
    
    ws.onEvent(onWebSocketEvent);
    server->addHandler(&ws);
    
    server->onNotFound([](AsyncWebServerRequest *request) {
        request->send(404, "text/plain", "Not Found");
    });
}

#endif