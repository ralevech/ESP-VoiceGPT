// ====================================================================
// common.cpp - Реализация общих функций
// Автор: ralevech
// ====================================================================

#include "common.h"
#include "config.h"
#include <esp_task_wdt.h>

// ===== ОПРЕДЕЛЕНИЯ ПЕРЕМЕННЫХ =====
bool ledState = false;
bool apMode = false;
bool serverRunning = false;
bool audioReady = false;

void toggleLED() {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
}

void feedWatchdog() {
#if ENABLE_WATCHDOG
    esp_task_wdt_reset();
#endif
}