// ====================================================================
// server_priv.h - Внутренний API веб-сервера
// Автор: ralevech
// ====================================================================

#ifndef SERVER_INTERNAL_H
#define SERVER_INTERNAL_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

extern AsyncWebServer* server;
extern AsyncWebSocket ws;

// Прототипы функций
void initServer();
void setupRoutes();
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, 
                      AwsEventType type, void *arg, uint8_t *data, size_t len);
void handleLedOn(AsyncWebServerRequest *request);
void handleLedOff(AsyncWebServerRequest *request);
void handleStatus(AsyncWebServerRequest *request);

#endif