// ====================================================================
// fs_manager.cpp - Управление файловой системой LittleFS
// 
// Описание: Монтирование и базовые операции с файловой системой
// Автор: ralevech
// ====================================================================

#include <LittleFS.h>
#include "config.h"
#include "common.h"

// ====================================================================
// initFileSystem() - Инициализация файловой системы LittleFS
// 
// Алгоритм:
//   1. Монтирование файловой системы
//   2. При ошибке - вывод сообщения об ошибке
//   3. При успехе - вывод информации о размере
// ====================================================================
void initFileSystem() {
    Serial.println("[FS] Монтирование LittleFS...");
    
    // Монтируем файловую систему (true = форматировать при ошибке)
    if (!LittleFS.begin(true)) {
        Serial.println("[FS] ОШИБКА! Не удалось смонтировать LittleFS");
        return;
    }
    
    Serial.println("[FS] LittleFS успешно смонтирована");
    
    // Получение информации о файловой системе (альтернативный способ)
    size_t totalBytes = LittleFS.totalBytes();
    size_t usedBytes = LittleFS.usedBytes();
    
    Serial.print("[FS] Общий размер: ");
    Serial.print(totalBytes / 1024);
    Serial.println(" KB");
    Serial.print("[FS] Использовано: ");
    Serial.print(usedBytes / 1024);
    Serial.println(" KB");
    Serial.print("[FS] Свободно: ");
    Serial.print((totalBytes - usedBytes) / 1024);
    Serial.println(" KB");
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
        Serial.println("[FS] Не удалось открыть директорию: " + String(path));
        return;
    }
    if (!root.isDirectory()) {
        Serial.println("[FS] Не директория: " + String(path));
        return;
    }
    
    File file = root.openNextFile();
    while (file) {
        for (int i = 0; i < depth; i++) {
            Serial.print("  ");
        }
        if (file.isDirectory()) {
            Serial.print("[DIR] ");
            Serial.println(file.name());
            listDirectory(file.name(), depth + 1);
        } else {
            Serial.print("[FILE] ");
            Serial.print(file.name());
            Serial.print(" (");
            Serial.print(file.size());
            Serial.println(" bytes)");
        }
        file = root.openNextFile();
    }
    file.close();
}