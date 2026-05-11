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

// ====================================================================
// initFileSystem() - Инициализация файловой системы LittleFS
// ====================================================================

static bool fileSystemReady = false;

void initFileSystem() {
    Serial.println("[FS] Монтирование LittleFS...");
    
    if (!LittleFS.begin(true)) {
        fileSystemReady = false;
        Serial.println("[FS] ОШИБКА! Не удалось смонтировать LittleFS");
        return;
    }

    fileSystemReady = true;

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
// checkFileExists() - Проверка существования файла
// ====================================================================
bool checkFileExists(const char* path) {
    return LittleFS.exists(path);
}

// ====================================================================
// getFileSize() - Получение размера файла
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
// ====================================================================
void listDirectory(const char* path, int depth) {
    File root = LittleFS.open(path);
    if (!root) {
        Serial.printf("[FS] Не удалось открыть директорию: %s\n", path);
        return;
    }
    if (!root.isDirectory()) {
        Serial.printf("[FS] Не директория: %s\n", path);
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

// ====================================================================
// isFileSystemReady() - Возвращает состояние монтирования FS
// ====================================================================
bool isFileSystemReady() {
    return fileSystemReady;
}