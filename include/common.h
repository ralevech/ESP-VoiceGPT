// ====================================================================
// common.h - Общие переменные и функции (ТОЛЬКО ОБЪЯВЛЕНИЯ!)
// Автор: ralevech
// ====================================================================

#ifndef COMMON_H
#define COMMON_H

#include <Arduino.h>

// ====================================================================
// УРОВНИ ЛОГИРОВАНИЯ (чем меньше число, тем подробнее)
// ====================================================================
#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_ERROR 3
#define LOG_LEVEL_NONE  4

// ====================================================================
// ТЕКУЩИЙ УРОВЕНЬ (меняй здесь: 0 - всё, 4 - ничего)
// ====================================================================
#define LOG_LEVEL 0

// ====================================================================
// МАКРОСЫ ЛОГИРОВАНИЯ
// ====================================================================
#define LOG_DEBUG(msg) if(LOG_LEVEL <= LOG_LEVEL_DEBUG) { Serial.println("[DEBUG] " msg); }
#define LOG_INFO(msg)  if(LOG_LEVEL <= LOG_LEVEL_INFO)  { Serial.println("[INFO] " msg); }
#define LOG_WARN(msg)  if(LOG_LEVEL <= LOG_LEVEL_WARN)  { Serial.println("[WARN] " msg); }
#define LOG_ERROR(msg) if(LOG_LEVEL <= LOG_LEVEL_ERROR) { Serial.println("[ERROR] " msg); }

// ===== ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ (объявления, не определения) =====
extern bool ledState;       // состояние светодиода (вкл/выкл)
extern bool apMode;         // режим точки доступа (true=AP, false=клиент)
extern bool serverRunning;  // флаг запуска веб-сервера (из task_server.cpp)
extern bool audioReady;     // флаг готовности аудио (из task_audio.cpp)

// ===== ОБЩИЕ ФУНКЦИИ (только прототипы) =====
void toggleLED();           // Переключает светодиод
void feedWatchdog();        // Сброс watchdog (вызывать в циклах задач)

// ===== ЗАПУСК ФАЙЛОВОЙ СИСТЕМЫ =====
void initFileSystem();      // Инициализация и монтирование LittleFS
bool isFileSystemReady();   // Возвращает true, если FS смонтирована

// ===== СТАТУС ПОДКЛЮЧЕНИЯ =====
bool isWiFiConnected();     // Возвращает true, если WiFi подключен
bool isAPMode();            // Возвращает true, если активен AP режим

#endif