// ====================================================================
// 02_wifi_core.cpp - Публичные функции WiFi-модуля
// Автор: ralevech
// ====================================================================

#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "common.h"
#include "wifi_priv.h"

bool wifiConnected = false;         // Флаг подключения к WiFi
bool apModeActive = false;          // Флаг активного AP режима
unsigned long lastReconnectAttempt = 0;  // Время последней попытки переподключения

// --------------------------------------------------------------------
// initWiFi() - Инициализация WiFi (режим станции)
// --------------------------------------------------------------------
void initWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true);
    delay(100);
    connectToWiFi();
}

// --------------------------------------------------------------------
// connectToWiFi() - Подключение к WiFi сети (из config.h)
// --------------------------------------------------------------------
void connectToWiFi() {
    if (apModeActive) {
        stopAPMode();
    }
    
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    
    int attempts = 0;
    while (attempts < MAX_ATTEMPTS && WiFi.status() != WL_CONNECTED) {
        vTaskDelay(pdMS_TO_TICKS(ATTEMPT_TIMEOUT_MS / MAX_ATTEMPTS));
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        wifiConnected = true;
        apModeActive = false;
        LOG_INFO("WiFi подключен. IP: " + WiFi.localIP().toString());
    } else {
        wifiConnected = false;
        LOG_ERROR("Не удалось подключиться");
        startAPMode();
    }
}

// --------------------------------------------------------------------
// startAPMode() - Запуск режима точки доступа
// --------------------------------------------------------------------
void startAPMode() {
    WiFi.mode(WIFI_AP);
    delay(100);
    
    if (WiFi.softAP(AP_SSID, AP_PASS)) {
        apModeActive = true;
        wifiConnected = false;
        LOG_INFO("AP режим запущен. SSID: " + String(AP_SSID));
    } else {
        LOG_ERROR("Не удалось запустить AP режим");
    }
}

// --------------------------------------------------------------------
// stopAPMode() - Остановка режима точки доступа
// --------------------------------------------------------------------
void stopAPMode() {
    WiFi.softAPdisconnect(true);
    apModeActive = false;
    delay(500);
}

// --------------------------------------------------------------------
// processWiFiEvents() - Проверка статуса и переподключение
// --------------------------------------------------------------------
void processWiFiEvents() {
    if (apModeActive) {
        if (millis() - lastReconnectAttempt > 30000) {
            lastReconnectAttempt = millis();
            connectToWiFi();
        }
        return;
    }
    
    if (WiFi.status() != WL_CONNECTED && wifiConnected) {
        wifiConnected = false;
        LOG_WARN("Соединение потеряно");
        
        if (millis() - lastReconnectAttempt > 5000) {
            lastReconnectAttempt = millis();
            connectToWiFi();
        }
    }
}

// --------------------------------------------------------------------
// isWiFiConnected() - Возвращает статус WiFi подключения
// --------------------------------------------------------------------
bool isWiFiConnected() {
    return (WiFi.status() == WL_CONNECTED);
}

// --------------------------------------------------------------------
// isAPMode() - Возвращает статус AP режима
// --------------------------------------------------------------------
bool isAPMode() {
    return apModeActive;
}