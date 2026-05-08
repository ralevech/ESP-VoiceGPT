// ====================================================================
// task_audio.cpp - Задача управления аудио (MAX98357A)
// 
// Описание: Реализует воспроизведение звука через I2S усилитель.
//           Поддерживает:
//           - MP3 файлы из LittleFS (playMP3)
//           - Синтез речи через VoiceRSS (speak)
//           - Интернет-радио (playRadio)
// Автор: ralevech
// 
// ПОДКЛЮЧЕНИЕ MAX98357A к ESP32-S3:
//   ESP32-S3 -> MAX98357A
//   3.3V     -> VIN
//   GND      -> GND
//   GPIO4    -> DIN   (AUDIO_DOUT_PIN)
//   GPIO5    -> BCLK  (AUDIO_BCLK_PIN)
//   GPIO6    -> LRCK  (AUDIO_LRC_PIN)

#include <Arduino.h>
#include <esp_task_wdt.h>
#include <LittleFS.h>
#include <Audio.h>
#include "config.h"
#include "common.h"

// ====================================================================
// УСЛОВНАЯ КОМПИЛЯЦИЯ: весь код включается только если аудио разрешено
// ====================================================================
#if ENABLE_AUDIO

// --------------------------------------------------------------------
// Глобальные объекты и переменные
// --------------------------------------------------------------------
static Audio audio;                                     // Основной объект аудио
static bool isPlaying = false;                          // Флаг: идёт воспроизведение?
static String currentFile = "";                         // Имя текущего файла (или "speech", "radio")

// --------------------------------------------------------------------
// ИНИЦИАЛИЗАЦИЯ
// --------------------------------------------------------------------
static void initAudio() {
    // Настройка I2S пинов
    audio.setPinout(AUDIO_BCLK_PIN, AUDIO_LRC_PIN, AUDIO_DOUT_PIN);
    
    // Установка громкости (0-21)
    audio.setVolume(AUDIO_VOLUME);
    
    // Принудительный моно режим (для одного динамика)
    audio.forceMono(true);
    
    Serial.println("[AUDIO] MAX98357A инициализирован");

    audioReady = true;
}

// --------------------------------------------------------------------
// ВОСПРОИЗВЕДЕНИЕ MP3 ФАЙЛА ИЗ LITTLEFS
// --------------------------------------------------------------------
bool playMP3(String filename) {
    String fullPath = "/" + filename;
    
    // Проверка существования файла
    if (!LittleFS.exists(fullPath)) {
        Serial.printf("[AUDIO] Файл не найден: %s\n", fullPath.c_str());
        return false;
    }
    
    // Остановка текущего воспроизведения
    if (isPlaying) {
        audio.stopSong();
        delay(100);
    }
    
    // Запуск воспроизведения
    Serial.printf("[AUDIO] Воспроизведение MP3: %s\n", fullPath.c_str());
    audio.connecttoFS(LittleFS, fullPath.c_str());
    isPlaying = true;
    currentFile = fullPath;
    
    return true;
}

// --------------------------------------------------------------------
// СИНТЕЗ РЕЧИ (Text-to-Speech)
// --------------------------------------------------------------------
void speak(String text) {
    // Остановка текущего воспроизведения
    if (isPlaying) {
        audio.stopSong();
        delay(100);
    }
    
    // Запуск синтеза речи (русский язык)
    Serial.printf("[AUDIO] Речь: %s\n", text.c_str());
    audio.connecttospeech(text.c_str(), "ru");          // "ru" - русский язык
    isPlaying = true;
    currentFile = "speech";
}

// --------------------------------------------------------------------
// ИНТЕРНЕТ-РАДИО (потоковое воспроизведение)
// --------------------------------------------------------------------
void playRadio(String url) {
    // Остановка текущего воспроизведения
    if (isPlaying) {
        audio.stopSong();
        delay(100);
    }
    
    // Запуск потока
    Serial.printf("[AUDIO] Радио: %s\n", url.c_str());
    audio.connecttohost(url.c_str());
    isPlaying = true;
    currentFile = "radio";
}

// --------------------------------------------------------------------
// ОСТАНОВКА ВОСПРОИЗВЕДЕНИЯ
// --------------------------------------------------------------------
void stopAudio() {
    if (isPlaying) {
        audio.stopSong();
        isPlaying = false;
        currentFile = "";
        Serial.println("[AUDIO] Воспроизведение остановлено");
    }
}

// --------------------------------------------------------------------
// УСТАНОВКА ГРОМКОСТИ
// --------------------------------------------------------------------
void setAudioVolume(int volume) {
    // Ограничение допустимого диапазона (0-21)
    if (volume < 0) volume = 0;
    if (volume > 21) volume = 21;
    
    audio.setVolume(volume);
    Serial.printf("[AUDIO] Громкость: %d\n", volume);
}

// ====================================================================
// ОБРАБОТЧИКИ СОБЫТИЙ БИБЛИОТЕКИ AUDIO
// ====================================================================

// Информационные сообщения
void audio_info(const char* info) {
    Serial.printf("[AUDIO] info: %s\n", info);
}

// Метаданные ID3 (название трека, исполнитель и т.д.)
void audio_id3data(const char* info) {
    Serial.printf("[AUDIO] ID3: %s\n", info);
}

// Событие окончания воспроизведения
void audio_eof_stream(const char* info) {
    Serial.printf("[AUDIO] Воспроизведение завершено: %s\n", info);
    isPlaying = false;
    currentFile = "";
}

// ====================================================================
// ОСНОВНАЯ ЗАДАЧА АУДИО
// ====================================================================
void taskAudio(void *pvParameters) {
    (void)pvParameters;
    
    Serial.println("[AUDIO] Задача запущена");  // <-- ЭТО САМОЕ ВАЖНОЕ
    
    esp_task_wdt_add(NULL);
    
    while (!isFileSystemReady()) {
        Serial.println("[AUDIO] Жду LittleFS...");
        vTaskDelay(pdMS_TO_TICKS(100));
        feedWatchdog();
    }
    
    Serial.println("[AUDIO] LittleFS готова");
    
    initAudio();
    
    Serial.println("[AUDIO] Готов к работе");
    
    while (true) {
        feedWatchdog();
        audio.loop();
        vTaskDelay(pdMS_TO_TICKS(10));
    }




    // // Регистрация в Watchdog
    // esp_task_wdt_add(NULL);
    
    // // Ожидание монтирования файловой системы
    // while (!isFileSystemReady()) {
    //     vTaskDelay(pdMS_TO_TICKS(100));
    //     feedWatchdog();
    // }
    
    // // Инициализация аудио
    // initAudio();
    
    // Serial.println("[AUDIO] Задача запущена, ожидание команд");
    
    // // Основной цикл
    // while (true) {
    //     feedWatchdog();                                 // Сброс watchdog
    //     audio.loop();                                   // Обслуживание аудио (обязательно!)
    //     vTaskDelay(pdMS_TO_TICKS(10));                  // Небольшая задержка
    // }
}

#endif // ENABLE_AUDIO