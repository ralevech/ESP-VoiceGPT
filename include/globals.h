// ====================================================================
// globals.h - Глобальные переменные проекта
// Автор: ralevech
// ====================================================================

#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>

extern bool ledState;       // Состояние встроенного LED
extern bool apMode;         // Режим точки доступа (true=AP, false=клиент)
extern bool serverRunning;  // Статус веб-сервера
extern bool audioReady;     // Статус аудио (инициализировано или нет)

#endif