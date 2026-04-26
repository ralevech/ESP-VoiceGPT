#include "common.h"
#include "config.h"

// ===== ОПРЕДЕЛЕНИЯ ПЕРЕМЕННЫХ =====
bool ledState = false;    // начальное состояние: выключен
bool apMode = false;      // начальный режим: клиент (не точка доступа AP)

// ===== ОПРЕДЕЛЕНИЯ ФУНКЦИЙ =====
void toggleLED() {
    ledState = !ledState;                     // меняем состояние
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);  // применяем к пину
}