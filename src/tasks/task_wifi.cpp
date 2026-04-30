// ====================================================================
// task_wifi.cpp - Задача управления WiFi подключением
// 
// Описание: Обеспечивает стабильное подключение к WiFi сети с
//           автоматическим переподключением при обрыве связи
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

// --------------------------------------------------------------------
// Прототипы внутренних функций
// --------------------------------------------------------------------
static bool connectToWiFi();
static void checkWiFiConnection();

// ====================================================================
// taskWiFi() - Основная задача управления WiFi
// 
// Алгоритм:
//   1. При старте - подключается к WiFi
//   2. В цикле - проверяет состояние подключения
//   3. При обрыве связи - переподключается автоматически
// ====================================================================
void taskWiFi(void *pvParameters) {
    (void)pvParameters;
    
    // Задержка при старте (дает время инициализации Serial)
    vTaskDelay(pdMS_TO_TICKS(500));
    
    Serial.println();
    Serial.println("[WiFi] Задача запущена");
    
    // Первичное подключение к WiFi
    connectToWiFi();
    
    // Основной цикл задачи
    while(true) {
        // Проверяем состояние WiFi подключения
        checkWiFiConnection();
        
        // Задержка перед следующей проверкой
        vTaskDelay(pdMS_TO_TICKS(DELAY_WIFI_CHECK));
    }
}

// ====================================================================
// connectToWiFi() - Подключение к WiFi сети
// 
// Возвращает: true - подключение успешно, false - ошибка
// ====================================================================
static bool connectToWiFi() {
    Serial.print("[WiFi] Подключение к ");
    Serial.print(WIFI_SSID);
    Serial.print("...");
    
    // Отключаемся от предыдущей сети (если была)
    WiFi.disconnect(true);
    vTaskDelay(pdMS_TO_TICKS(100));
    
    // Настраиваем WiFi в режим станции (клиента)
    WiFi.mode(WIFI_STA);
    
    // Начинаем подключение
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    
    // Ждем подключения с таймаутом
    int attempts = 0;
    while (attempts < MAX_ATTEMPTS && WiFi.status() != WL_CONNECTED) {
        vTaskDelay(pdMS_TO_TICKS(ATTEMPT_TIMEOUT_MS / MAX_ATTEMPTS));
        attempts++;
        Serial.print(".");
    }
    
    // Проверяем результат
    if (WiFi.status() == WL_CONNECTED) {
        wifiConnected = true;
        Serial.println();
        Serial.println("[WiFi] ПОДКЛЮЧЕН!");
        Serial.print("[WiFi] IP адрес: ");
        Serial.println(WiFi.localIP());
        Serial.print("[WiFi] MAC адрес: ");
        Serial.println(WiFi.macAddress());
        Serial.print("[WiFi] Канал: ");
        Serial.println(WiFi.channel());
        Serial.print("[WiFi] Уровень сигнала: ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
        return true;
    } else {
        wifiConnected = false;
        Serial.println();
        Serial.println("[WiFi] ОШИБКА ПОДКЛЮЧЕНИЯ!");
        Serial.println("[WiFi] Проверьте SSID и пароль");
        return false;
    }
}

// ====================================================================
// checkWiFiConnection() - Проверка состояния WiFi и переподключение
// 
// Если соединение потеряно - пытается восстановить
// ====================================================================
static void checkWiFiConnection() {
    // Проверяем текущее состояние подключения
    if (WiFi.status() == WL_CONNECTED) {
        // Если все хорошо, но флаг сброшен - обновляем
        if (!wifiConnected) {
            wifiConnected = true;
            Serial.println("[WiFi] Связь восстановлена!");
            Serial.print("[WiFi] IP адрес: ");
            Serial.println(WiFi.localIP());
        }
        return;
    }
    
    // Соединение потеряно
    if (wifiConnected) {
        // Только что потеряли соединение
        wifiConnected = false;
        Serial.println();
        Serial.println("[WiFi] СВЯЗЬ ПОТЕРЯНА!");
        Serial.println("[WiFi] Попытка переподключения...");
    } else {
        // Уже были в состоянии потери связи
        static unsigned long lastLog = 0;
        if (millis() - lastLog > 30000) {  // Логируем раз в 30 секунд
            lastLog = millis();
            Serial.println("[WiFi] Ожидание восстановления связи...");
        }
    }
    
    // Пытаемся переподключиться
    // Ограничиваем частоту попыток (не чаще раза в 5 секунд)
    unsigned long currentTime = millis();
    if (currentTime - lastReconnectAttempt > 5000) {
        lastReconnectAttempt = currentTime;
        
        Serial.print("[WiFi] Переподключение к ");
        Serial.print(WIFI_SSID);
        Serial.println("...");
        
        // Отключаемся и пробуем заново
        WiFi.disconnect();
        vTaskDelay(pdMS_TO_TICKS(100));
        WiFi.begin(WIFI_SSID, WIFI_PASS);
        
        // Короткая задержка для проверки
        vTaskDelay(pdMS_TO_TICKS(2000));
        
        if (WiFi.status() == WL_CONNECTED) {
            wifiConnected = true;
            Serial.println(" ПОДКЛЮЧЕНО!");
            Serial.print("[WiFi] Новый IP адрес: ");
            Serial.println(WiFi.localIP());
        } else {
            Serial.println("подключение к сети wifi не удалось");
            Serial.println(" ");
        }
    }
}

// ====================================================================
// Дополнительные функции для внешнего использования
// ====================================================================

// Возвращает состояние WiFi подключения
bool isWiFiConnected() {
    return (WiFi.status() == WL_CONNECTED);
}

// Возвращает уровень сигнала RSSI (если подключен)
int getWiFiRSSI() {
    if (WiFi.status() == WL_CONNECTED) {
        return WiFi.RSSI();
    }
    return -999;
}

// Принудительное переподключение
void reconnectWiFi() {
    Serial.println("[WiFi] Принудительное переподключение...");
    WiFi.disconnect(true);
    vTaskDelay(pdMS_TO_TICKS(500));
    connectToWiFi();
}
