// ====================================================================
// common.cpp - Реализация общих функций
// Автор: ralevech
// ====================================================================

#include "common.h"
#include "config.h"
#include <esp_task_wdt.h>

// ===== ОПРЕДЕЛЕНИЯ ПЕРЕМЕННЫХ (здесь и только здесь!) =====
bool ledState = false;
bool apMode = false;
bool audioReady = false;
bool serverRunning = false;

// ====================================================================
// toggleLED() - Переключение встроенного светодиода
// ====================================================================
void toggleLED() {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
}

// ====================================================================
// feedWatchdog() - Сброс сторожевого таймера
// ====================================================================
void feedWatchdog() {
#if ENABLE_WATCHDOG
    esp_task_wdt_reset();
#endif
}