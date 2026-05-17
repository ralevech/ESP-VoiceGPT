// ====================================================================
// 01_task_serial.cpp - Задача вывода статуса системы
// Автор: ralevech
// ====================================================================

#include <Arduino.h>
#include <esp_task_wdt.h>
#include "config.h"
#include "common.h"
#include "serial_priv.h"

void taskSerial(void *pvParameters) {
    (void)pvParameters;
    
    esp_task_wdt_add(NULL);
    printHello();
    
    while (true) {
        printSystemStatus();
        printWiFiStatus();
        printServerStatus();
        printFileSystemStatus();
        printLedStatus();
        printAudioStatus();
        Serial.println();
        
        feedWatchdog();
        vTaskDelay(pdMS_TO_TICKS(DELAY_SERIAL_PRINT));
    }
}