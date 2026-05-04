// ====================================================================
// task_server.cpp - Задача веб-сервера
// 
// Описание: Асинхронный веб-сервер для обслуживания HTML страниц
//           и обработки API запросов. Работает как в STA режиме, так и в AP.
// Автор: ralevech
// ====================================================================

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include "config.h"
#include "common.h"
#include "tasks.h"

// Глобальный объект сервера
static AsyncWebServer* server = nullptr;
static bool serverRunning = false;

// Внешние функции (определены в task_wifi.cpp)
extern bool isWiFiConnected();
extern bool isAPMode();

// --------------------------------------------------------------------
// Обработчики API запросов
// --------------------------------------------------------------------

// Обработчик: включение светодиода
static void handleLedOn(AsyncWebServerRequest *request) {
    ledState = true;
    Serial.println("[WEB] LED ON запрос получен");
    
    // TODO: Здесь будет код управления RGB светодиодом
    // pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    // pixels.show();
    
    request->send(200, "text/plain; charset=utf-8", "LED включен");
}

// Обработчик: выключение светодиода
static void handleLedOff(AsyncWebServerRequest *request) {
    ledState = false;
    Serial.println("[WEB] LED OFF запрос получен");
    
    // TODO: Здесь будет код управления RGB светодиодом
    // pixels.setPixelColor(0, pixels.Color(0, 0, 0));
    // pixels.show();
    
    request->send(200, "text/plain; charset=utf-8", "LED выключен");
}

// Обработчик: получение статуса
static void handleStatus(AsyncWebServerRequest *request) {
    Serial.println("[WEB] STATUS запрос получен");
    
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
    // ===== API ENDPOINTS (JSON) =====
    srv->on("/api/led/on", HTTP_GET, handleLedOn);
    srv->on("/api/led/off", HTTP_GET, handleLedOff);
    srv->on("/api/status", HTTP_GET, handleStatus);
    
    // ===== СТАТИЧЕСКИЕ ФАЙЛЫ =====
    // Главная страница
    srv->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/index.html", "text/html; charset=utf-8");
    });
    
    // Обработка запросов к файлам в папке data
    srv->serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
    
    // ===== ОБРАБОТКА ОШИБОК =====
    srv->onNotFound([](AsyncWebServerRequest *request) {
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

// --------------------------------------------------------------------
// Запуск веб-сервера
// --------------------------------------------------------------------
static void startServer() {
    if (server != nullptr) {
        // Если сервер уже запущен, останавливаем его
        server->end();
        delete server;
        server = nullptr;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    
    // Создаём новый сервер на порту 80
    server = new AsyncWebServer(80);
    
    // Настраиваем маршруты
    setupRoutes(server);
    
    // Запускаем сервер
    server->begin();
    serverRunning = true;
    
    Serial.println("[WEB] Сервер запущен!");
    
    // Выводим информацию о доступе в зависимости от режима
    if (isWiFiConnected()) {
        Serial.print("[WEB] STA режим: http://");
        Serial.println(WiFi.localIP());
    } else if (isAPMode()) {
        Serial.print("[WEB] AP режим: http://");
        Serial.println(WiFi.softAPIP());
    } else {
        Serial.println("[WEB] ОЖИДАНИЕ: режим сети не определён");
    }
}

// ====================================================================
// taskWebServer() - Основная задача веб-сервера
// 
// Алгоритм:
//   1. Ждём монтирования файловой системы
//   2. Ждём готовности сети (STA или AP)
//   3. Запускаем сервер
//   4. В цикле проверяем смену режима (STA <-> AP)
//   5. При смене режима перезапускаем сервер
//   6. Периодический сброс watchdog
// ====================================================================
void taskWebServer(void *pvParameters) {
    (void)pvParameters;
    
    // Ждём монтирование файловой системы
    while (!isFileSystemReady()) {
        vTaskDelay(pdMS_TO_TICKS(100));
        feedWatchdog();  // Сброс watchdog во время ожидания
    }
    Serial.println("[WEB] Файловая система готова");
    
    bool lastAPMode = false;
    bool lastWiFiConnected = false;
    
    // Основной цикл
    while (true) {
        // Проверяем, готова ли сеть (хотя бы один режим активен)
        bool networkReady = (isWiFiConnected() || isAPMode());
        
        if (networkReady && !serverRunning) {
            // Сеть появилась, а сервер не запущен - запускаем
            Serial.println("[WEB] Сеть готова, запускаем сервер...");
            startServer();
        } 
        else if (serverRunning) {
            // Сервер запущен, проверяем не сменился ли режим
            bool modeChanged = false;
            
            if (lastAPMode != isAPMode()) {
                Serial.println("[WEB] Обнаружена смена AP режима!");
                modeChanged = true;
            }
            
            if (lastWiFiConnected != isWiFiConnected()) {
                Serial.println("[WEB] Обнаружена смена STA режима!");
                modeChanged = true;
            }
            
            if (modeChanged) {
                Serial.println("[WEB] Перезапуск сервера для нового режима...");
                startServer();  // Перезапускаем сервер
            }
        }
        
        // Сохраняем текущее состояние для следующей проверки
        lastAPMode = isAPMode();
        lastWiFiConnected = isWiFiConnected();
        
        // Задержка перед следующей проверкой
        vTaskDelay(pdMS_TO_TICKS(2000));
        
        // Сброс watchdog (задача жива)
        feedWatchdog();
    }
}