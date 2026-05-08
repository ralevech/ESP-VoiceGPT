// ====================================================================
// task_serial.cpp - Задача вывода статуса системы
// 
// Описание: Периодический вывод информации о состоянии всех компонентов
//           системы: WiFi, веб-сервер, файловая система, LED и т.д.
// Автор: ralevech
// ====================================================================

#include <Arduino.h>
#include <esp_task_wdt.h>
#include <WiFi.h>
#include <LittleFS.h>
#include "config.h"
#include "common.h"

// Внешние флаги из других задач
extern bool serverRunning;   // из task_server.cpp

// --------------------------------------------------------------------
// Вывод приветствия
// --------------------------------------------------------------------
static void printHello(){
    Serial.printf(
        "\n"
        "╔════════════════════════════════════════╗\n"
        "║                                        ║\n"
        "║        ESP32-Skeleton by ralevech      ║\n"
        "║                                        ║\n"
        "╚════════════════════════════════════════╝\n"
        "\n"
    );
}

// --------------------------------------------------------------------
// Вывод статуса WiFi
// --------------------------------------------------------------------
static void printWiFiStatus() {
    static wl_status_t lastStatus = WL_DISCONNECTED;
    static bool wasConnected = false;
    
    wl_status_t currentStatus = WiFi.status();
    
    if (wasConnected && currentStatus != WL_CONNECTED) {
        Serial.println("[WiFi] RECONNECTING...");
        wasConnected = false;
    }
    
    switch (currentStatus) {
        case WL_CONNECTED: {
            int rssi = WiFi.RSSI();
            Serial.printf(
                "\n"
                "[WiFi] Status: CONNECTED\n"
                "[WiFi] IP:     http://%s\n"
                "[WiFi] MAC:    %s\n"
                "[WiFi] Signal: %s\n"
                "[WiFi] RSSI:   %d dBm\n\n",
                WiFi.localIP().toString().c_str(),
                WiFi.macAddress().c_str(),
                (rssi > -50) ? "Отличный" : (rssi > -70) ? "Нормальный" : "Плохой, нужно что-то делать!",
                rssi
            );
            wasConnected = true;
            break;
        }
        case WL_NO_SSID_AVAIL:
            Serial.printf("[WiFi] SSID NOT FOUND: %s\n", WIFI_SSID);
            break;
        case WL_CONNECT_FAILED:
            Serial.println("[WiFi] CONNECTION FAILED (wrong password?)");
            break;
        case WL_CONNECTION_LOST:
            Serial.println("[WiFi] CONNECTION LOST");
            break;
        case WL_DISCONNECTED:
            Serial.println("[WiFi] DISCONNECTED");
            break;
    }
    
    lastStatus = currentStatus;
}

// --------------------------------------------------------------------
// Вывод статуса веб-сервера
// --------------------------------------------------------------------
static void printServerStatus() {
    if (serverRunning) {
        Serial.printf("[WEB] Сервер запущен: http://%s\n", WiFi.localIP().toString().c_str());
    } else {
        Serial.println("[WEB] Сервер не запущен");
    }
}

// --------------------------------------------------------------------
// Вывод статуса файловой системы
// --------------------------------------------------------------------
static void printFileSystemStatus() {
    if (isFileSystemReady()) {
        Serial.printf("[FS] LittleFS готово. Всего: %u KB, свободно: %u KB\n\n",
            LittleFS.totalBytes() / 1024,
            (LittleFS.totalBytes() - LittleFS.usedBytes()) / 1024
        );
    } else {
        Serial.println("[FS] LittleFS НЕ ДОСТУПНА");
    }
}

// --------------------------------------------------------------------
// Вывод статуса LED
// --------------------------------------------------------------------
static void printLedStatus() {
    // Раскомментируй при необходимости
    // Serial.printf("[LED] Состояние: %s\n", ledState ? "ON" : "OFF");
}

// --------------------------------------------------------------------
// Вывод статуса системы (память, частота)
// --------------------------------------------------------------------
static void printSystemStatus() {
    Serial.printf(
        "[ESP] Чип: %s\n"
        "[ESP] Кол-во ядер шт.: %d\n"
        "[ESP] Процессор MHz: %d\n"
        "[ESP] Размер Flash чипа байт: %u\n"
        "[ESP] Оперативка байт: %u\n"
        "[ESP] Всего свободно байт: %u\n",
        ESP.getChipModel(),
        ESP.getChipCores(),
        ESP.getCpuFreqMHz(),
        ESP.getFlashChipSize(),
        ESP.getHeapSize(),
        ESP.getFreeHeap()
    );
}

// --------------------------------------------------------------------
// Вывод статуса аудио (MAX98357A)
// --------------------------------------------------------------------
static void printAudioStatus() {
    Serial.printf("[AUDIO] Статус: %s\n", audioReady ? "Инициализирован" : "Ошибка/Не готов");
}

// ====================================================================
// taskSerial() - Задача вывода статуса
// ====================================================================
void taskSerial(void *pvParameters) {
    (void)pvParameters;
    
    esp_task_wdt_add(NULL);

    while (true) {
        printHello();
        printSystemStatus();
        printWiFiStatus();
        printServerStatus();
        printFileSystemStatus();
        printLedStatus();
        printAudioStatus();
    
        feedWatchdog();
        vTaskDelay(pdMS_TO_TICKS(DELAY_SERIAL_PRINT));
    }
}