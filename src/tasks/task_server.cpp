// ====================================================================
// task_server.cpp - Задача веб-сервера
// 
// Описание: Асинхронный веб-сервер + WebSocket для управления в реальном времени
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

static AsyncWebServer* server = nullptr;

// WSS21: Объект WebSocket
static AsyncWebSocket ws("/ws");

extern bool isWiFiConnected();
extern bool isAPMode();

// WSS21: Обработчик входящих WebSocket сообщений
static void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, 
                              AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_DATA) {
        AwsFrameInfo *info = (AwsFrameInfo*)arg;
        if (info->final && info->index == 0 && info->len == len && len > 0) {
            data[len] = '\0';
            String message = String((char*)data);
            
            // Команда: громкость
            if (message.startsWith("VOL:")) {
                int vol = message.substring(4).toInt();
                setAudioVolume(vol);
                // Отправляем подтверждение всем клиентам
                ws.printfAll("VOL:%d", vol);
            }
            // Команда: воспроизвести MP3
            else if (message.startsWith("PLAY:")) {
                String filename = message.substring(5);
                playMP3(filename);
                ws.printfAll("STATUS:Воспроизводится %s", filename.c_str());
            }
            // Команда: стоп
            else if (message == "STOP") {
                stopAudio();
                ws.printfAll("STATUS:Остановлено");
            }
            // Команда: получить статус
            else if (message == "STATUS") {
                client->text("STATUS:Аудио готово");
            }
        }
    }
}

static void handleLedOn(AsyncWebServerRequest *request) {
    ledState = true;
    LOG_DEBUG("LED ON запрос получен");
    request->send(200, "text/plain; charset=utf-8", "LED включен");
    ws.printfAll("LED:ON");   // WSS21: Уведомляем всех
}

static void handleLedOff(AsyncWebServerRequest *request) {
    ledState = false;
    LOG_DEBUG("LED OFF запрос получен");
    request->send(200, "text/plain; charset=utf-8", "LED выключен");
    ws.printfAll("LED:OFF");   // WSS21: Уведомляем всех
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
    // API ENDPOINTS (JSON)
    srv->on("/api/led/on", HTTP_GET, handleLedOn);
    srv->on("/api/led/off", HTTP_GET, handleLedOff);
    srv->on("/api/status", HTTP_GET, handleStatus);
    
    // СТАТИЧЕСКИЕ ФАЙЛЫ
    srv->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/index.html", "text/html; charset=utf-8");
    });
    
    srv->serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
    
    // WSS21: Обработка WebSocket
    ws.onEvent(onWebSocketEvent);
    srv->addHandler(&ws);
    
    // ОБРАБОТКА ОШИБОК 404
    srv->onNotFound([](AsyncWebServerRequest *request) {
        LOG_WARN("404 Not Found: " + String(request->url()));
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
    LOG_INFO("Веб-сервер запущен на порту 80");
    
    // WSS21: Очищаем WebSocket подключения
    ws.cleanupClients();
}

// WSS21: Функция для вещания статуса аудио через WebSocket
void wsAudioStatus(const char* status) {
    ws.printfAll("AUDIO:%s", status);
}

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
    
    // WSS21: Периодическая очистка старых WebSocket клиентов
    while (true) {
        feedWatchdog();
        ws.cleanupClients();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

#endif // ENABLE_WEBSERVER