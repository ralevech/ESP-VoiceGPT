// ====================================================================
// task_audio.cpp - Задача управления аудио (FreeRTOS)
// Автор: ralevech
// ====================================================================

#include <Arduino.h>
#include <esp_task_wdt.h>
#include "Audio.h"
#include "config.h"
#include "common.h"
#include "tasks.h"
#include "audio_priv.h"

#if ENABLE_AUDIO

void taskAudio(void *pvParameters) {
    (void)pvParameters;
    
    esp_task_wdt_add(NULL);
    
    while (!isFileSystemReady()) {
        vTaskDelay(pdMS_TO_TICKS(100));
        feedWatchdog();
    }
    
    initAudio();
    
    // ===== ТЕСТ: Воспроизведение без браузера =====
    vTaskDelay(pdMS_TO_TICKS(2000));  // Ждём 2 секунды
    playMP3("ksyusha.mp3");           // Запускаем MP3
    // ============================================
    
    
    while (true) {
        feedWatchdog();
        processAudioLoop();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

#endif