// ====================================================================
// 02_serial_core.cpp - Функции вывода статуса системы
// Автор: ralevech
// ====================================================================

#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include "config.h"
#include "common.h"
#include "globals.h"
#include "serial_priv.h"

void printHello() {
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

void printWiFiStatus() {
    static bool wasConnected = false;
    
    if (WiFi.status() == WL_CONNECTED) {
        if (!wasConnected) {
            Serial.printf(
                "\n[WiFi] CONNECTED\n"
                "[WiFi] IP: http://%s\n"
                "[WiFi] RSSI: %d dBm\n\n",
                WiFi.localIP().toString().c_str(),
                WiFi.RSSI()
            );
            wasConnected = true;
        }
    } else {
        if (wasConnected) {
            LOG_WARN("WiFi: RECONNECTING...");
            wasConnected = false;
        }
    }
}

void printServerStatus() {
    if (serverRunning) {
        LOG_INFO("Веб-сервер запущен: http://" + WiFi.localIP().toString());
    } else {
        LOG_WARN("Веб-сервер не запущен");
    }
}

void printFileSystemStatus() {
    if (isFileSystemReady()) {
        LOG_INFO("LittleFS. Всего: " + String(LittleFS.totalBytes() / 1024) + 
                 " KB, свободно: " + String((LittleFS.totalBytes() - LittleFS.usedBytes()) / 1024) + " KB");
    } else {
        LOG_ERROR("LittleFS НЕ ДОСТУПНА");
    }
}

void printLedStatus() {
    LOG_DEBUG("LED состояние: " + String(ledState ? "ON" : "OFF"));
}

void printSystemStatus() {
    Serial.printf(
        "[ESP] Чип: %s, ядер: %d, MHz: %d\n"
        "[ESP] Flash: %u MB, Heap: %u bytes, Free: %u bytes\n",
        ESP.getChipModel(),
        ESP.getChipCores(),
        ESP.getCpuFreqMHz(),
        ESP.getFlashChipSize() / (1024 * 1024),
        ESP.getHeapSize(),
        ESP.getFreeHeap()
    );
}

void printAudioStatus() {
    LOG_INFO("Аудио: " + String(audioReady ? "Инициализирован" : "Ошибка/Не готов"));
}