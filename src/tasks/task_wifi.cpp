// ====================================================================
// task_wifi.cpp - Задача управления WiFi подключением
// 
// Описание: Обеспечивает стабильное подключение к WiFi сети с
//           автоматическим переподключением при обрыве связи.
//           Если подключение не удалось - запускает режим точки доступа (AP)
// Автор: ralevech
// ====================================================================

#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "common.h"

// --------------------------------------------------------------------
// Глобальные переменные для отслеживания состояния WiFi
// --------------------------------------------------------------------
static unsigned long lastReconnectAttempt = 0;
static bool wifiConnected = false;
static bool apModeActive = false;  // Флаг активного AP режима

// --------------------------------------------------------------------
// Прототипы внутренних функций
// --------------------------------------------------------------------
static bool connectToWiFi();
static void checkWiFiConnection();
static void startAPmode();
static void stopAPmode();

// ====================================================================
// taskWiFi() - Основная задача управления WiFi
// 
// Алгоритм:
//   1. При старте - подключается к WiFi
//   2. Если не получилось - запускает AP mode
//   3. В цикле - проверяет состояние подключения
//   4. При обрыве связи - пытается переподключиться
//   5. Если переподключение не удалось - включает AP mode
//   6. Периодический сброс watchdog
// ====================================================================
void taskWiFi(void *pvParameters) {
    (void)pvParameters;
    
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
    if (apModeActive) {
        stopAPmode();
    }
    
    // Отключаемся от предыдущей сети (если была)
    WiFi.disconnect(true);
    vTaskDelay(pdMS_TO_TICKS(100));
    
    // Настраиваем WiFi в режим станции (клиента)
    WiFi.mode(WIFI_STA);
    
    Serial.print("[WiFi] Подключение к ");
    Serial.println(WIFI_SSID);
    
    // Начинаем подключение
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    
    // Ждем подключения с таймаутом
    int attempts = 0;
    while (attempts < MAX_ATTEMPTS && WiFi.status() != WL_CONNECTED) {
        vTaskDelay(pdMS_TO_TICKS(ATTEMPT_TIMEOUT_MS / MAX_ATTEMPTS));
        attempts++;
        Serial.print(".");
    }
    Serial.println();
    
    // Проверяем результат
    if (WiFi.status() == WL_CONNECTED) {
        wifiConnected = true;
        apModeActive = false;
        apMode = false;  // глобальный флаг из common.h
        Serial.println("[WiFi] Подключено успешно!");
        Serial.print("[WiFi] IP адрес: ");
        Serial.println(WiFi.localIP());
        return true;
    } else {
        wifiConnected = false;
        Serial.println("[WiFi] Не удалось подключиться!");
        return false;
    }
}

// ====================================================================
// startAPmode() - Запуск режима точки доступа
// 
// Алгоритм:
//   1. Настраивает WiFi в режим точки доступа
//   2. Устанавливает SSID и пароль из config.h
//   3. Выводит информацию об AP в Serial
// ====================================================================
static void startAPmode() {
    Serial.println("[WiFi] Запуск режима точки доступа (AP)...");
    
    // Отключаем STA режим
    WiFi.mode(WIFI_AP);
    vTaskDelay(pdMS_TO_TICKS(100));
    
    // Запускаем точку доступа
    bool result = WiFi.softAP(AP_SSID, AP_PASS);
    
    if (result) {
        apModeActive = true;
        apMode = true;  // глобальный флаг из common.h
        Serial.println("[WiFi] Точка доступа запущена!");
        Serial.print("[WiFi] AP SSID: ");
        Serial.println(AP_SSID);
        Serial.print("[WiFi] AP пароль: ");
        Serial.println(AP_PASS);
        Serial.print("[WiFi] AP IP адрес: ");
        Serial.println(WiFi.softAPIP());
    } else {
        Serial.println("[WiFi] ОШИБКА: Не удалось запустить точку доступа!");
    }
}

// ====================================================================
// stopAPmode() - Остановка режима точки доступа
// ====================================================================
static void stopAPmode() {
    Serial.println("[WiFi] Остановка режима точки доступа...");
    WiFi.softAPdisconnect(true);
    apModeActive = false;
    apMode = false;  // глобальный флаг из common.h
    vTaskDelay(pdMS_TO_TICKS(500));
}

// ====================================================================
// checkWiFiConnection() - Проверка состояния WiFi и переподключение
// 
// Если соединение потеряно - пытается восстановить.
// Если восстановление не удалось - включает AP режим.
// Если AP режим активен - пробует периодически подключиться к WiFi
// ====================================================================
static void checkWiFiConnection() {
    // Если AP режим активен - пробуем периодически подключиться к WiFi
    if (apModeActive) {
        unsigned long currentTime = millis();
        if (currentTime - lastReconnectAttempt > 30000) {  // Каждые 30 секунд
            lastReconnectAttempt = currentTime;
            Serial.println("[WiFi] AP режим: пробую подключиться к WiFi...");
            
            if (connectToWiFi()) {
                // Успешно подключились к WiFi, выключаем AP режим
                Serial.println("[WiFi] Подключено к WiFi, выключаю AP режим");
            }
        }
        return;
    }
    
    // Если WiFi отключился - пробуем переподключиться
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[WiFi] Соединение потеряно! Пробую переподключиться...");
        
        unsigned long currentTime = millis();
        if (currentTime - lastReconnectAttempt > 5000) {  // Не чаще раза в 5 секунд
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

// ====================================================================
// Публичные функции для внешнего использования
// ====================================================================

// Возвращает состояние WiFi подключения
bool isWiFiConnected() {
    return (WiFi.status() == WL_CONNECTED);
}

// Возвращает активен ли AP режим
bool isAPMode() {
    return apModeActive;
}