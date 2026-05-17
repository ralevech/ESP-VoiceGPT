// ====================================================================
// 02_blink_core.cpp - Логика RGB LED (эффект радуги)
// Автор: ralevech
// ====================================================================

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "config.h"
#include "blink_priv.h"

Adafruit_NeoPixel pixels(NUM_LEDS, RGB_LED_PIN, NEO_GRB + NEO_KHZ800);
static int wheelPos = 0;    // Текущая позиция на цветовом круге (0-255)

// --------------------------------------------------------------------
// initBlink() - Инициализация RGB LED
// --------------------------------------------------------------------
void initBlink() {
    pixels.begin();                 // Инициализация пина
    pixels.setBrightness(50);       // Яркость 50% (0-255)
    pixels.clear();                 // Очистка всех пикселей
    pixels.show();                  // Отправка команды
}

// --------------------------------------------------------------------
// processBlink() - Обновление цвета (вызывается в цикле задачи)
// --------------------------------------------------------------------
void processBlink() {
    pixels.setPixelColor(0, Wheel(wheelPos));   // Установка цвета для первого светодиода
    pixels.show();                              // Отправка команды
    wheelPos++;                                 // Следующая позиция
    if (wheelPos > 255) wheelPos = 0;           // Цикл от 0 до 255
}

// --------------------------------------------------------------------
// Wheel() - Преобразование позиции в цвет радуги
// Параметры: wheelPos - позиция (0-255)
// Возвращает: цвет в формате GRB для библиотеки Adafruit_NeoPixel
// --------------------------------------------------------------------
uint32_t Wheel(byte wheelPos) {
    wheelPos = 255 - wheelPos;
    
    if (wheelPos < 85) {
        // Сектор 1: Красный → Зеленый
        return pixels.Color(255 - wheelPos * 3, 0, wheelPos * 3);
    } 
    else if (wheelPos < 170) {
        // Сектор 2: Зеленый → Синий
        wheelPos -= 85;
        return pixels.Color(0, wheelPos * 3, 255 - wheelPos * 3);
    } 
    else {
        // Сектор 3: Синий → Красный
        wheelPos -= 170;
        return pixels.Color(wheelPos * 3, 255 - wheelPos * 3, 0);
    }
}