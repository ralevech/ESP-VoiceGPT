// ====================================================================
// 01_task_server.cpp - Задача веб-сервера
// Автор: ralevech
// ====================================================================

#include <Arduino.h>
#include <esp_task_wdt.h>
#include "config.h"
#include "common.h"
#include "globals.h"
#include "tasks.h" 
#include "server_priv.h"

#if ENABLE_WEBSERVER

void taskWebServer(void *pvParameters) {
    (void)pvParameters;
    
    esp_task_wdt_add(NULL);
    
    while (!isFileSystemReady()) {
        vTaskDelay(pdMS_TO_TICKS(100));
        feedWatchdog();
    }
    
    while (!isWiFiConnected() && !isAPMode()) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        feedWatchdog();
    }
    
    initServer();
    
    while (true) {
        ws.cleanupClients();
        vTaskDelay(pdMS_TO_TICKS(1000));
        feedWatchdog();
    }
}

#endif