// настройки (WiFi, пины, тайминги)
#ifndef CONFIG_H
#define CONFIG_H

// ===== WiFi НАСТРОЙКИ =====
#define WIFI_SSID     "Gorizont_2"
#define WIFI_PASS     "31123522"

// ===== ТОЧКА ДОСТУПА (AP) =====
#define AP_SSID       "ESP32_AP"
#define AP_PASS       "12345678"

// ===== ПОДКЛЮЧЕНИЕ К WIFI =====
#define MAX_ATTEMPTS      3
#define ATTEMPT_TIMEOUT_MS  10000

// ===== ПИНЫ =====
#define LED_PIN       LED_BUILTIN

// ===== ЗАДАЧИ: ПРИОРИТЕТЫ (0-24, 0-низкий) =====
#define PRIO_SERIAL   4
#define PRIO_WIFI     5
#define PRIO_BLINK    3
#define PRIO_SERVER   2
#define PRIO_SENSOR   1

// ===== ЗАДАЧИ: СТЕК (байты) =====
#define STACK_WIFI    4096
#define STACK_BLINK   2048
#define STACK_SERIAL  2048
#define STACK_SERVER  8192
#define STACK_SENSOR  4096

// ===== ЗАДАЧИ: ЯДРА (0 или 1) =====
#define CORE_WIFI     0
#define CORE_BLINK    1
#define CORE_SERIAL   0
#define CORE_SERVER   0
#define CORE_SENSOR   1

// ===== ТАЙМИНГИ (мс) =====
#define DELAY_WIFI_CHECK    10000
#define DELAY_BLINK         500
#define DELAY_SERIAL_PRINT  5000
#define DELAY_SENSOR_CHECK  2000

#endif