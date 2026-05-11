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
// ====================================================================

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
    
    LOG_INFO("MAX98357A инициализирован");
    audioReady = true;
}

// --------------------------------------------------------------------
// ВОСПРОИЗВЕДЕНИЕ MP3 ФАЙЛА ИЗ LITTLEFS
// --------------------------------------------------------------------
bool playMP3(String filename) {
    String fullPath = "/" + filename;
    
    // Проверка существования файла
    if (!LittleFS.exists(fullPath)) {
        LOG_ERROR("MP3 файл не найден: " + fullPath);
        return false;
    }
    
    // Остановка текущего воспроизведения
    if (isPlaying) {
        audio.stopSong();
        delay(100);
    }
    
    // Запуск воспроизведения
    LOG_INFO("Воспроизведение MP3: " + fullPath);
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
    LOG_INFO("Синтез речи: " + text);
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
    LOG_INFO("Интернет-радио: " + url);
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
        LOG_DEBUG("Воспроизведение остановлено");
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
    LOG_INFO("Громкость: " + String(volume));
}

// ====================================================================
// ОБРАБОТЧИКИ СОБЫТИЙ БИБЛИОТЕКИ AUDIO
// ====================================================================

// Информационные сообщения
void audio_info(const char* info) {
    LOG_DEBUG("Audio info: " + String(info));
}

// Метаданные ID3 (название трека, исполнитель и т.д.)
void audio_id3data(const char* info) {
    LOG_DEBUG("ID3: " + String(info));
}

// Событие окончания воспроизведения
void audio_eof_stream(const char* info) {
    LOG_DEBUG("Воспроизведение завершено: " + String(info));
    isPlaying = false;
    currentFile = "";
}

// ====================================================================
// ОСНОВНАЯ ЗАДАЧА АУДИО
// ====================================================================
void taskAudio(void *pvParameters) {
    (void)pvParameters;
    
    LOG_DEBUG("Аудио задача запущена");
    
    // Регистрация в Watchdog
    esp_task_wdt_add(NULL);
    
    // Ожидание монтирования файловой системы
    while (!isFileSystemReady()) {
        vTaskDelay(pdMS_TO_TICKS(100));
        feedWatchdog();
    }
    
    // Инициализация аудио
    initAudio();
    
    // ТЕСТОВОЕ ВОСПРОИЗВЕДЕНИЕ (раскомментируй нужное)
    vTaskDelay(pdMS_TO_TICKS(2000));
    playMP3("ksyusha.mp3");
    // speak("Привет, мир!");
    
    LOG_DEBUG("Аудио задача готова к работе");
    
    // Основной цикл задачи
    while (true) {
        feedWatchdog();                                 // Сброс watchdog
        audio.loop();                                   // Обслуживание аудио (обязательно!)
        vTaskDelay(pdMS_TO_TICKS(10));                  // Задержка для стабильности
    }
}

#endif // ENABLE_AUDIO