// ====================================================================
// common.h - Макросы логирования и системные функции
// Автор: ralevech
// ====================================================================

#ifndef COMMON_H
#define COMMON_H

#include <Arduino.h>

// ====================================================================
// УРОВНИ ЛОГИРОВАНИЯ
// ====================================================================
#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_ERROR 3
#define LOG_LEVEL_NONE  4

#define LOG_LEVEL 3

// ====================================================================
// МАКРОСЫ ЛОГИРОВАНИЯ
// ====================================================================
#define LOG_DEBUG(msg) if(LOG_LEVEL <= LOG_LEVEL_DEBUG) { Serial.println("[DEBUG] " msg); }
#define LOG_INFO(msg)  if(LOG_LEVEL <= LOG_LEVEL_INFO)  { Serial.println("[INFO] " msg); }
#define LOG_WARN(msg)  if(LOG_LEVEL <= LOG_LEVEL_WARN)  { Serial.println("[WARN] " msg); }
#define LOG_ERROR(msg) if(LOG_LEVEL <= LOG_LEVEL_ERROR) { Serial.println("[ERROR] " msg); }

// ====================================================================
// СИСТЕМНЫЕ ФУНКЦИИ (прототипы)
// ====================================================================
void toggleLED();           // Переключение встроенного LED
void feedWatchdog();        // Сброс сторожевого таймера

// ====================================================================
// ФАЙЛОВАЯ СИСТЕМА (прототипы)
// ====================================================================
void initFileSystem();
bool isFileSystemReady();
bool checkFileExists(const char* path);
size_t getFileSize(const char* path);
void listDirectory(const char* path, int depth);

#endif