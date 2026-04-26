// Монтирование файловой системы
#include <LittleFS.h>
#include "common.h"

void initFileSystem() {
    if (!LittleFS.begin(true)) {
        Serial.println("[FS] Ошибка монтирования LittleFS!");
        return;
    }
    Serial.println("[FS] LittleFS смонтирована");
}
