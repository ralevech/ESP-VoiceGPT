// ====================================================================
// task_wifi.cpp - Задача управления WiFi подключением
// ====================================================================

#include <Arduino.h>
#include <esp_task_wdt.h>
#include <WiFi.h>
#include "config.h"
#include "common.h"

static unsigned long lastReconnectAttempt = 0;
static bool wifiConnected = false;

static bool connectToWiFi();
static void checkWiFiConnection();
static void startAPmode();
static void stopAPmode();

void taskWiFi(void *pvParameters) {
    (void)pvParameters;
    
    esp_task_wdt_add(NULL);
    vTaskDelay(pdMS_TO_TICKS(500));
    
    if (!connectToWiFi()) {
        startAPmode();
    }
    
    while(true) {
        checkWiFiConnection();
        vTaskDelay(pdMS_TO_TICKS(DELAY_WIFI_CHECK));
        feedWatchdog();
    }
}

static bool connectToWiFi() {
    if (apMode) {
        stopAPmode();
    }
    
    WiFi.disconnect(true);
    vTaskDelay(pdMS_TO_TICKS(100));
    WiFi.mode(WIFI_STA);
    
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    
    int attempts = 0;
    while (attempts < MAX_ATTEMPTS && WiFi.status() != WL_CONNECTED) {
        vTaskDelay(pdMS_TO_TICKS(ATTEMPT_TIMEOUT_MS / MAX_ATTEMPTS));
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        wifiConnected = true;
        apMode = false;
        return true;
    } else {
        wifiConnected = false;
        Serial.println("[WiFi] Не удалось подключиться!");
        return false;
    }
}

static void startAPmode() {
    WiFi.mode(WIFI_AP);
    vTaskDelay(pdMS_TO_TICKS(100));
    
    bool result = WiFi.softAP(AP_SSID, AP_PASS);
    
    if (result) {
        apMode = true;
    } else {
        Serial.println("[WiFi] ОШИБКА: Не удалось запустить точку доступа!");
    }
}

static void stopAPmode() {
    WiFi.softAPdisconnect(true);
    apMode = false;
    vTaskDelay(pdMS_TO_TICKS(500));
}

static void checkWiFiConnection() {
    if (apMode) {
        unsigned long currentTime = millis();
        if (currentTime - lastReconnectAttempt > 30000) {
            lastReconnectAttempt = currentTime;
            if (connectToWiFi()) {
                Serial.println("[WiFi] Подключено к WiFi, выключаю AP режим");
            }
        }
        return;
    }
    
    if (WiFi.status() != WL_CONNECTED) {
        unsigned long currentTime = millis();
        if (currentTime - lastReconnectAttempt > 5000) {
            lastReconnectAttempt = currentTime;
            if (connectToWiFi()) {
                Serial.println("[WiFi] Переподключение успешно!");
            } else {
                Serial.println("[WiFi] Не удалось переподключиться, включаю AP режим");
                startAPmode();
            }
        }
    }
}

bool isWiFiConnected() {
    return (WiFi.status() == WL_CONNECTED);
}

bool isAPMode() {
    return apMode;
}