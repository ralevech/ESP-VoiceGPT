// общие переменные и функции
#ifndef COMMON_H
#define COMMON_H

#include <Arduino.h>

// ===== ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ =====
extern bool ledState;       // состояние светодиода (вкл/выкл)
extern bool apMode;         // режим точки доступа (true=AP, false=клиент)

// ===== ОБЩИЕ ФУНКЦИИ =====
void toggleLED();           // Переключает светодиод и обновляет ledState

// ===== ЗАПУСК ФАЙЛОВОЙ СИСТЕМЫ =====
void initFileSystem();      // Инициализация и монтирование LittleFS
bool isFileSystemReady();   // Возвращает true, если FS смонтирована

// ===== СТАТУС ПОДКЛЮЧЕНИЯ WIFI =====
bool isWiFiConnected();     // Возвращает true, если WiFi подключен

#endif