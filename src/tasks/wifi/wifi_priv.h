// ====================================================================
// wifi_priv.h - Внутренний API WiFi-модуля
// Автор: ralevech
// ====================================================================

#ifndef WIFI_INTERNAL_H
#define WIFI_INTERNAL_H

#include <Arduino.h>
#include <WiFi.h>

// Глобальные переменные
extern bool wifiConnected;      // Флаг подключения к WiFi
extern bool apModeActive;       // Флаг активного AP режима
extern unsigned long lastReconnectAttempt;

// Прототипы функций
void initWiFi();                // Инициализация WiFi
void connectToWiFi();           // Подключение к сети
void startAPMode();             // Запуск точки доступа
void stopAPMode();              // Остановка точки доступа
void processWiFiEvents();       // Обработка событий WiFi

#endif