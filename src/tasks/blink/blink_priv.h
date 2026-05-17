// ====================================================================
// blink_priv.h - Внутренний API модуля RGB LED
// Автор: ralevech
// ====================================================================

#ifndef BLINK_PRIV_H
#define BLINK_PRIV_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

extern Adafruit_NeoPixel pixels;    // Объект для управления RGB лентой
extern uint32_t Wheel(byte wheelPos); // Преобразование позиции в цвет радуги

void initBlink();    // Инициализация RGB LED (пин, яркость)
void processBlink(); // Обновление цвета (вызов в цикле задачи)

#endif