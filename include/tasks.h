// ====================================================================
// tasks.h - Объявления всех задач FreeRTOS и публичных функций
// 
// Описание: Здесь объявляются все задачи, которые работают в проекте,
//           а также публичные функции для межзадачного взаимодействия.
// Автор: ralevech
// ====================================================================

#ifndef TASKS_H
#define TASKS_H

#include <Arduino.h>

// ====================================================================
// 1. ОСНОВНЫЕ ЗАДАЧИ (всегда активны)
// ====================================================================

void taskWiFi(void *pvParameters);      // Управление WiFi (подключение, AP режим)
void taskBlink(void *pvParameters);     // RGB LED (эффект радуги)
void taskSerial(void *pvParameters);    // Вывод статуса системы в Serial

// ====================================================================
// 2. ОПЦИОНАЛЬНЫЕ ЗАДАЧИ (включаются через флаги в config.h)
// ====================================================================

#if ENABLE_WEBSERVER
    void taskWebServer(void *pvParameters); // Веб-сервер (HTTP API + WebSocket)
#endif

#if ENABLE_SENSOR
    void taskSensor(void *pvParameters);    // Заглушка для будущих датчиков
#endif

#if ENABLE_AUDIO
    void taskAudio(void *pvParameters);     // Воспроизведение звука через MAX98357A
#endif

#if ENABLE_PID
    void taskPID(void *pvParameters);       // PID регулятор (температура, скорость, освещение)
#endif

// ====================================================================
// 3. ПУБЛИЧНЫЕ ФУНКЦИИ ДЛЯ ВЫЗОВА ИЗ ДРУГИХ ЗАДАЧ
// ====================================================================

bool isWiFiConnected();     // Статус WiFi подключения
bool isAPMode();            // Статус режима точки доступа

#if ENABLE_AUDIO
    bool playMP3(String filename);      // Воспроизвести MP3 из LittleFS
    void speak(String text);            // Синтезировать речь (Text-to-Speech)
    void playRadio(String url);         // Включить интернет-радио
    void stopAudio();                   // Остановить воспроизведение
    void setAudioVolume(int volume);    // Установить громкость (0-21)
#endif

// ====================================================================
// 4. БУДУЩИЕ ЗАДАЧИ (закомментированы до добавления функционала)
// ====================================================================

// #if ENABLE_MQTT
//     void taskMQTT(void *pvParameters);   // MQTT клиент (управление через брокер)
// #endif

// #if ENABLE_MODBUS
//     void taskModbus(void *pvParameters); // Modbus RTU (промышленная автоматизация)
// #endif

#endif // TASKS_H