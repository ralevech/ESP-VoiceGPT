// ====================================================================
// task_wifi.cpp - Задача управления WiFi подключением
// 
// Описание: Обеспечивает стабильное подключение к WiFi сети с
//           автоматическим переподключением при обрыве связи.
//           Если подключение не удалось - запускает режим точки доступа (AP)
// Автор: ralevech
// ====================================================================

#include <Arduino.h>
#include <esp_task_wdt.h>
#include <WiFi.h>
#include "config.h"
#include "common.h"

// --------------------------------------------------------------------
// Глобальные переменные для отслеживания состояния WiFi
// --------------------------------------------------------------------
static unsigned long lastReconnectAttempt = 0;
static bool wifiConnected = false;

// --------------------------------------------------------------------
// Прототипы внутренних функций
// --------------------------------------------------------------------
static bool connectToWiFi();
static void checkWiFiConnection();
static void startAPmode();
static void stopAPmode();

// ====================================================================
// taskWiFi() - Основная задача управления WiFi
// ====================================================================
void taskWiFi(void *pvParameters) {
    (void)pvParameters;
    
    // Регистрация в Watchdog
    esp_task_wdt_add(NULL);
    
    // Задержка при старте (дает время инициализации Serial)
    vTaskDelay(pdMS_TO_TICKS(500));
    
    // Первичное подключение к WiFi
    if (!connectToWiFi()) {
        // Если подключение не удалось, запускаем AP режим
        startAPmode();
    }
    
    // Основной цикл задачи
    while(true) {
        // Проверяем состояние WiFi подключения
        checkWiFiConnection();
        // Задержка перед следующей проверкой
        vTaskDelay(pdMS_TO_TICKS(DELAY_WIFI_CHECK));
        
        // Сброс watchdog (задача жива)
        feedWatchdog();
    }
}

// ====================================================================
// connectToWiFi() - Подключение к WiFi сети
// 
// Возвращает: true - подключение успешно, false - ошибка
// ====================================================================
static bool connectToWiFi() {
    // Если AP режим активен - сначала его выключаем
    if (apMode) {
        stopAPmode();
    }
    
    // Отключаемся от предыдущей сети (если была)
    WiFi.disconnect(true);
    vTaskDelay(pdMS_TO_TICKS(100));
    
    // Настраиваем WiFi в режим станции (клиента)
    WiFi.mode(WIFI_STA);
    
    LOG_INFO("Подключение к " + String(WIFI_SSID));
    
    // Начинаем подключение
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    
    // Ждем подключения с таймаутом
    int attempts = 0;
    while (attempts < MAX_ATTEMPTS && WiFi.status() != WL_CONNECTED) {
        vTaskDelay(pdMS_TO_TICKS(ATTEMPT_TIMEOUT_MS / MAX_ATTEMPTS));
        attempts++;
    }
    
    // Проверяем результат
    if (WiFi.status() == WL_CONNECTED) {
        wifiConnected = true;
        apMode = false;
        LOG_INFO("Подключено успешно! IP: " + WiFi.localIP().toString());
        return true;
    } else {
        wifiConnected = false;
        LOG_ERROR("Не удалось подключиться!");
        return false;
    }
}

// ====================================================================
// startAPmode() - Запуск режима точки доступа
// ====================================================================
static void startAPmode() {
    LOG_WARN("Запуск режима точки доступа (AP)...");
    
    // Отключаем STA режим
    WiFi.mode(WIFI_AP);
    vTaskDelay(pdMS_TO_TICKS(100));
    
    // Запускаем точку доступа
    bool result = WiFi.softAP(AP_SSID, AP_PASS);
    
    if (result) {
        apMode = true;
        LOG_INFO("Точка доступа запущена. SSID: " + String(AP_SSID) + ", IP: " + WiFi.softAPIP().toString());
    } else {
        LOG_ERROR("Не удалось запустить точку доступа!");
    }
}

// ====================================================================
// stopAPmode() - Остановка режима точки доступа
// ====================================================================
static void stopAPmode() {
    LOG_DEBUG("Остановка режима точки доступа...");
    WiFi.softAPdisconnect(true);
    apMode = false;
    vTaskDelay(pdMS_TO_TICKS(500));
}

// ====================================================================
// checkWiFiConnection() - Проверка состояния WiFi и переподключение
// ====================================================================
static void checkWiFiConnection() {
    // Если AP режим активен - пробуем периодически подключиться к WiFi
    if (apMode) {
        unsigned long currentTime = millis();
        if (currentTime - lastReconnectAttempt > 30000) {  // Каждые 30 секунд
            lastReconnectAttempt = currentTime;
            LOG_DEBUG("AP режим: пробую подключиться к WiFi...");
            
            if (connectToWiFi()) {
                LOG_INFO("Подключено к WiFi, выключаю AP режим");
            }
        }
        return;
    }
    
    // Если WiFi отключился - пробуем переподключиться
    if (WiFi.status() != WL_CONNECTED) {
        LOG_WARN("Соединение потеряно! Пробую переподключиться...");
        
        unsigned long currentTime = millis();
        if (currentTime - lastReconnectAttempt > 5000) {  // Не чаще раза в 5 секунд
            lastReconnectAttempt = currentTime;
            
            if (connectToWiFi()) {
                LOG_INFO("Переподключение успешно!");
            } else {
                LOG_WARN("Не удалось переподключиться, включаю AP режим");
                startAPmode();
            }
        }
    }
}

// ====================================================================
// Публичные функции для внешнего использования
// ====================================================================

// Возвращает состояние WiFi подключения
bool isWiFiConnected() {
    return (WiFi.status() == WL_CONNECTED);
}

// Возвращает активен ли AP режим
bool isAPMode() {
    return apMode;
}