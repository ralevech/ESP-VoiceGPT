// ====================================================================
// fs_manager.cpp - Управление файловой системой LittleFS
// 
// Описание: Монтирование и базовые операции с файловой системой
// Автор: ralevech
// ====================================================================

#include <Arduino.h>
#include <LittleFS.h>
#include "config.h"
#include "common.h"

// --------------------------------------------------------------------
// Статическая переменная состояния файловой системы
// --------------------------------------------------------------------
static bool fileSystemReady = false;

// ====================================================================
// initFileSystem() - Инициализация файловой системы LittleFS
// 
// Алгоритм:
//   1. Монтирование файловой системы
//   2. При ошибке - вывод сообщения об ошибке
//   3. При успехе - вывод информации о размере
// ====================================================================
void initFileSystem() {
    LOG_INFO("Монтирование LittleFS...");
    
    // Монтируем файловую систему (true = форматировать при ошибке)
    if (!LittleFS.begin(true)) {
        fileSystemReady = false;
        LOG_ERROR("ОШИБКА! Не удалось смонтировать LittleFS");
        return;
    }

    fileSystemReady = true;

    // Получение информации о файловой системе
    size_t totalBytes = LittleFS.totalBytes();
    size_t usedBytes = LittleFS.usedBytes();
    
    Serial.printf(
        "[FS] LittleFS успешно смонтирована\n"
        "[FS] Общий размер: %u KB\n"
        "[FS] Использовано: %u KB\n"
        "[FS] Свободно: %u KB\n",
        totalBytes / 1024,
        usedBytes / 1024,
        (totalBytes - usedBytes) / 1024
    );
}

// ====================================================================
// isFileSystemReady() - Возвращает состояние файловой системы
// 
// Возвращает: true - файловая система смонтирована и готова к работе
//             false - ошибка монтирования
// ====================================================================
bool isFileSystemReady() {
    return fileSystemReady;
}

// ====================================================================
// checkFileExists() - Проверка существования файла
// 
// Параметры: path - путь к файлу
// Возвращает: true - файл существует, false - нет
// ====================================================================
bool checkFileExists(const char* path) {
    return LittleFS.exists(path);
}

// ====================================================================
// getFileSize() - Получение размера файла
// 
// Параметры: path - путь к файлу
// Возвращает: размер файла в байтах, 0 если файл не найден
// ====================================================================
size_t getFileSize(const char* path) {
    File file = LittleFS.open(path, "r");
    if (!file) {
        return 0;
    }
    size_t size = file.size();
    file.close();
    return size;
}

// ====================================================================
// listDirectory() - Вывод содержимого директории (для отладки)
// 
// Параметры: path - путь к директории, depth - глубина рекурсии
// ====================================================================
void listDirectory(const char* path, int depth) {
    File root = LittleFS.open(path);
    if (!root) {
        LOG_ERROR("Не удалось открыть директорию: " + String(path));
        return;
    }
    if (!root.isDirectory()) {
        LOG_ERROR("Не директория: " + String(path));
        return;
    }
    
    File file = root.openNextFile();
    while (file) {
        for (int i = 0; i < depth; i++) {
            Serial.print("  ");
        }
        if (file.isDirectory()) {
            Serial.printf("[DIR] %s\n", file.name());
            listDirectory(file.name(), depth + 1);
        } else {
            Serial.printf("[FILE] %s (%u bytes)\n", file.name(), file.size());
        }
        file = root.openNextFile();
    }
    file.close();
}