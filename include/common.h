// общие переменные и функции
#ifndef COMMON_H
#define COMMON_H

#include <Arduino.h>

// ===== ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ =====
extern bool ledState;      // состояние светодиода (вкл/выкл)
extern bool apMode;        // режим точки доступа (true=AP, false=клиент)

// ===== ОБЩИЕ ФУНКЦИИ =====
void toggleLED();          // Переключает светодиод и обновляет ledState

// ===== ЗАПУСК ФАЙЛОВОЙ СИСТЕМЫ =====
void initFileSystem();     //

// ===== СТАТУС ПОДКЛЮЧЕНИЯ WIFI =====
bool isWiFiConnected();    //

#endif