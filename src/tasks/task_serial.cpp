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
static void printHello(){
    Serial.println();
    Serial.println("\033[1;36m╔════════════════════════════════════════╗\033[0m");
    Serial.println("\033[1;36m║                                        ║\033[0m");
    Serial.println("\033[1;33m║        ESP32-Skeleton by ralevech     ║\033[0m");
    Serial.println("\033[1;36m║                                        ║\033[0m");
    Serial.println("\033[1;36m╚════════════════════════════════════════╝\033[0m");
    Serial.println();
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
        Serial.println("[WiFi] RECONNECTING...");
        wasConnected = false;
    }
    
    // Обработка нового статуса
    switch (currentStatus) {
        case WL_CONNECTED: {
            int rssi = WiFi.RSSI();
            Serial.println(" ");
            Serial.println("[WiFi] CONNECTED");
            Serial.print("[WiFi] IP:     http://"); Serial.println(WiFi.localIP());
            Serial.print("[WiFi] MAC:    "); Serial.println(WiFi.macAddress());
            
            // Оценка сигнала
            Serial.print("[WiFi] Signal: ");
            if (rssi > -50) {
                Serial.println("Отличный");
            } else if (rssi > -70) {
                Serial.println("Нормальный");
            } else {
                Serial.println("Плохой, нужно что-то делать!");
            }
            
            Serial.print("[WiFi] RSSI:   "); Serial.print(rssi); Serial.println(" dBm");
            Serial.println("");
            wasConnected = true;
            break;
        }
            
        case WL_NO_SSID_AVAIL:
            Serial.print("[WiFi] SSID NOT FOUND: "); Serial.println(WIFI_SSID);
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
    // Сервер запускается в task_server.cpp, флага пока нет
    // Можно добавить флаг serverReady в future
    // Serial.println("[STATUS] Веб-сервер:   ✅ ЗАПУЩЕН (порт 80)");
}

// --------------------------------------------------------------------
// Вывод статуса файловой системы
// --------------------------------------------------------------------
static void printFileSystemStatus() {
    if (isFileSystemReady()) {
        size_t total = LittleFS.totalBytes();
        size_t used = LittleFS.usedBytes();
        // Serial.print("[STATUS] LittleFS:      ✅ СМОНТИРОВАНА");
        // Serial.print(" | Свободно: ");
        // Serial.print((total - used) / 1024);
        // Serial.println(" KB");
    } else {
        // Serial.println("[STATUS] LittleFS:      ❌ НЕ ДОСТУПНА");
    }
}

// --------------------------------------------------------------------
// Вывод статуса LED
// --------------------------------------------------------------------
static void printLedStatus() {
    // Serial.print("[STATUS] LED:          ");
    // Serial.println(ledState ? "🟢 ВКЛЮЧЕН" : "⚫ ВЫКЛЮЧЕН");
}

// --------------------------------------------------------------------
// Вывод статуса системы (память, uptime)
// --------------------------------------------------------------------
static void printSystemStatus() {
    Serial.print("[ESP] Чип: ");
    Serial.println(ESP.getChipModel());
    Serial.print("[ESP] Кол-во ядер шт.: ");
    Serial.println(ESP.getChipCores());
    Serial.print("[ESP] Процессор MHz: ");
    Serial.println(ESP.getCpuFreqMHz());
    Serial.print("[ESP] Размер Flash чипа байт: ");
    Serial.println(ESP.getFlashChipSize());
    Serial.print("[ESP] Оперативка байт: ");
    Serial.println(ESP.getHeapSize());
    Serial.print("[ESP] Свободная байт: ");
    Serial.println(ESP.getFreeHeap());
}

// ====================================================================
// taskSerial() - Задача вывода статуса
// 
// Алгоритм:
//   1. Ожидание запуска системы (3 секунды)
//   2. Вывод приветствия
//   3. Бесконечный цикл вывода информации о состоянии
//   4. Периодический сброс watchdog
// ====================================================================
void taskSerial(void *pvParameters) {
    (void)pvParameters;
    // РЕГИСТРАЦИЯ В WATCHDOG
    // Эта команда добавляет текущую задачу в список наблюдаемых сторожевого таймера.
    // Без неё Watchdog не будет следить за задачей, и если она зависнет - перезагрузки НЕ БУДЕТ.
    // Аргумент NULL означает "текущая задача taskSerial".
    esp_task_wdt_add(NULL);

    vTaskDelay(pdMS_TO_TICKS(3000));
    printHello();

    while (true) {
        // Выводим информацию
        printSystemStatus();
        vTaskDelay(pdMS_TO_TICKS(500));
        printWiFiStatus();
        vTaskDelay(pdMS_TO_TICKS(500));
        printServerStatus();
        vTaskDelay(pdMS_TO_TICKS(500));
        printFileSystemStatus();
        vTaskDelay(pdMS_TO_TICKS(500));
        printLedStatus();
        vTaskDelay(pdMS_TO_TICKS(500));
        Serial.println();
        
        // Сброс watchdog (задача жива)
        feedWatchdog();
        
        // Задержка до следующего вывода
        vTaskDelay(pdMS_TO_TICKS(DELAY_SERIAL_PRINT));
    }
}