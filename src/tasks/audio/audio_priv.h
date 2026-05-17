// ====================================================================
// audio_priv.h - Внутренний API аудио-модуля
// Автор: ralevech
// ====================================================================

#ifndef AUDIO_H
#define AUDIO_H

#include <Arduino.h>
#include "Audio.h"              // Библиотека ESP32-audioI2S

// Глобальные переменные
extern Audio audio;             // Объект аудио (I2S, MP3, радио)
extern bool isPlaying;          // Флаг: идёт воспроизведение?
extern String currentFile;      // Имя текущего файла / "speech" / "radio"

// Прототипы функций
void initAudio();               // Инициализация I2S пинов и громкости
void processAudioLoop();        // Вызов audio.loop() (обслуживание аудио)

#endif