// ====================================================================
// 02_audio_core.cpp - Публичные функции аудио-модуля
// Автор: ralevech
// ====================================================================

#include <Arduino.h>
#include <LittleFS.h>
#include "Audio.h"
#include "config.h"
#include "common.h"
#include "globals.h"
#include "audio_priv.h"

#if ENABLE_AUDIO

Audio audio;
bool isPlaying = false;
String currentFile = "";

// --------------------------------------------------------------------
// initAudio() - Настройка I2S пинов, громкости и моно-режима
// --------------------------------------------------------------------
void initAudio() {
    audio.setPinout(AUDIO_BCLK_PIN, AUDIO_LRC_PIN, AUDIO_DOUT_PIN);
   // audio.setVolume(AUDIO_VOLUME);
    audio.setVolume(21);           // ← временно на максимум
    audio.forceMono(true);
    audioReady = true;
}

// --------------------------------------------------------------------
// processAudioLoop() - Обслуживание аудио (декодирование, буферизация, I2S)
// --------------------------------------------------------------------
void processAudioLoop() {
    audio.loop();
}

// --------------------------------------------------------------------
// playMP3() - Воспроизведение MP3 файла из файловой системы LittleFS
// --------------------------------------------------------------------
bool playMP3(String filename) {
    String fullPath = "/" + filename;
    
    if (!LittleFS.exists(fullPath)) {
        LOG_ERROR("Файл не найден: " + fullPath);
        return false;
    }
    
    if (isPlaying) {
        audio.stopSong();
        delay(100);
    }
    
    audio.connecttoFS(LittleFS, fullPath.c_str());
    isPlaying = true;
    currentFile = fullPath;
    
    return true;
}

// --------------------------------------------------------------------
// speak() - Синтез речи из текста (требуется интернет)
// --------------------------------------------------------------------
void speak(String text) {
    if (isPlaying) {
        audio.stopSong();
        delay(100);
    }
    
    audio.connecttospeech(text.c_str(), "ru");
    isPlaying = true;
    currentFile = "speech";
}

// --------------------------------------------------------------------
// playRadio() - Воспроизведение интернет-радио по URL (требуется интернет)
// --------------------------------------------------------------------
void playRadio(String url) {
    if (isPlaying) {
        audio.stopSong();
        delay(100);
    }
    
    audio.connecttohost(url.c_str());
    isPlaying = true;
    currentFile = "radio";
}

// --------------------------------------------------------------------
// stopAudio() - Остановка текущего воспроизведения
// --------------------------------------------------------------------
void stopAudio() {
    if (isPlaying) {
        audio.stopSong();
        isPlaying = false;
        currentFile = "";
    }
}

// --------------------------------------------------------------------
// setAudioVolume() - Установка громкости (диапазон 0-21)
// --------------------------------------------------------------------
void setAudioVolume(int volume) {
    if (volume < 0) volume = 0;
    if (volume > 21) volume = 21;
    audio.setVolume(volume);
}

#endif