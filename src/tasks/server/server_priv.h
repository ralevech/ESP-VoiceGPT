// ====================================================================
// server_priv.h - Внутренний API веб-сервера
// Автор: ralevech
// ====================================================================

#ifndef SERVER_PRIV_H
#define SERVER_PRIV_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

// ====================================================================
// ГЛОБАЛЬНЫЕ ОБЪЕКТЫ (доступны всем файлам модуля server)
// ====================================================================

extern AsyncWebServer* server;   // Указатель на HTTP сервер
extern AsyncWebSocket ws;        // WebSocket объект (для отправки данных в реальном времени)

// ====================================================================
// ПРОТОТИПЫ ФУНКЦИЙ
// ====================================================================

void initServer();               // Запуск веб-сервера (создание и настройка)
void setupRoutes();              // Настройка HTTP маршрутов (API и статика)
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, 
                      AwsEventType type, void *arg, uint8_t *data, size_t len);  // Обработчик WebSocket
void handleLedOn(AsyncWebServerRequest *request);     // Включить LED
void handleLedOff(AsyncWebServerRequest *request);    // Выключить LED
void handleStatus(AsyncWebServerRequest *request);    // Вернуть JSON со статусом

#endif