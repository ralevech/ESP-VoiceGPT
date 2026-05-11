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

// --------------------------------------------------------------------
// Вывод приветствия
// --------------------------------------------------------------------
static void printHello() {
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
    
    // Обработка потери соединения
    if (wasConnected && currentStatus != WL_CONNECTED) {
        LOG_WARN("WiFi: RECONNECTING...");
        wasConnected = false;
    }
    
    // Обработка нового статуса
    switch (currentStatus) {
        case WL_CONNECTED: {
            int rssi = WiFi.RSSI();
            Serial.printf(
                "\n[WiFi] CONNECTED\n"
                "[WiFi] IP:     http://%s\n"
                "[WiFi] MAC:    %s\n"
                "[WiFi] Signal: %s\n"
                "[WiFi] RSSI:   %d dBm\n\n",
                WiFi.localIP().toString().c_str(),
                WiFi.macAddress().c_str(),
                (rssi > -50) ? "Отличный" : (rssi > -70) ? "Нормальный" : "Плохой",
                rssi
            );
            wasConnected = true;
            break;
        }
        case WL_NO_SSID_AVAIL:
            LOG_ERROR("WiFi: SSID NOT FOUND: " + String(WIFI_SSID));
            break;
        case WL_CONNECT_FAILED:
            LOG_ERROR("WiFi: CONNECTION FAILED (wrong password?)");
            break;
        case WL_CONNECTION_LOST:
            LOG_WARN("WiFi: CONNECTION LOST");
            break;
        case WL_DISCONNECTED:
            LOG_WARN("WiFi: DISCONNECTED");
            break;
    }
    
    lastStatus = currentStatus;
}

// --------------------------------------------------------------------
// Вывод статуса веб-сервера
// --------------------------------------------------------------------
static void printServerStatus() {
    if (serverRunning) {
        LOG_INFO("Веб-сервер запущен: http://" + WiFi.localIP().toString());
    } else {
        LOG_WARN("Веб-сервер не запущен");
    }
}

// --------------------------------------------------------------------
// Вывод статуса файловой системы
// --------------------------------------------------------------------
static void printFileSystemStatus() {
    if (isFileSystemReady()) {
        LOG_INFO("LittleFS готово. Всего: " + String(LittleFS.totalBytes() / 1024) + 
                 " KB, свободно: " + String((LittleFS.totalBytes() - LittleFS.usedBytes()) / 1024) + " KB");
    } else {
        LOG_ERROR("LittleFS НЕ ДОСТУПНА");
    }
}

// --------------------------------------------------------------------
// Вывод статуса LED
// --------------------------------------------------------------------
static void printLedStatus() {
    LOG_DEBUG("LED состояние: " + String(ledState ? "ON" : "OFF"));
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
    LOG_INFO("Аудио статус: " + String(audioReady ? "Инициализирован" : "Ошибка/Не готов"));
}

// ====================================================================
// taskSerial() - Задача вывода статуса
// ====================================================================
void taskSerial(void *pvParameters) {
    (void)pvParameters;
    
    // Регистрация в Watchdog
    esp_task_wdt_add(NULL);
    
    // Вывод приветствия
    printHello();
    
    // Основной цикл вывода статуса
    while (true) {
        printSystemStatus();
        printWiFiStatus();
        printServerStatus();
        printFileSystemStatus();
        printLedStatus();
        printAudioStatus();
        Serial.println();
        
        // Сброс watchdog
        feedWatchdog();
        
        // Задержка до следующего вывода
        vTaskDelay(pdMS_TO_TICKS(DELAY_SERIAL_PRINT));
    }
}