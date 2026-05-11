// ====================================================================
// task_pid.cpp - Задача PID регулятора
// 
// Описание: Реализует PID регулирование для различных процессов:
//           - Температура (инерционный процесс)
//           - Скорость двигателя (быстрый процесс)
//           - Освещение (мгновенный процесс)
// 
// Использование:
//   1. В config.h настроить коэффициенты (Kp, Ki, Kd)
//   2. В этой задаче добавить чтение датчика и управление актуатором
//   3. Коэффициенты можно менять через веб-интерфейс
// Автор: ralevech
// ====================================================================

#include <Arduino.h>
#include <esp_task_wdt.h>
#include "config.h"
#include "common.h"
#include "pid_controller.h"          // Подключаем библиотеку из lib/

#if ENABLE_PID

// --------------------------------------------------------------------
// Глобальные объекты
// --------------------------------------------------------------------
static PIDController pid(PID_DT);

// --------------------------------------------------------------------
// Здесь нужно подключить реальный датчик
// --------------------------------------------------------------------
static float readSensor() {
    // TODO: Заменить на реальный датчик
    // Пример для термистора или датчика температуры:
    // return analogRead(TEMP_SENSOR_PIN);
    // 
    // Пример для скорости двигателя (энкодер):
    // return readEncoder();
    // 
    // Пример для освещения (фоторезистор):
    // return analogRead(LIGHT_SENSOR_PIN);
    
    // Временно имитируем чтение датчика (20-80 градусов)
    static float fakeValue = 25.0;
    fakeValue += (random(-10, 10) / 100.0);  // Медленное изменение
    fakeValue = constrain(fakeValue, 20, 80);
    return fakeValue;
}

// --------------------------------------------------------------------
// Здесь нужно управлять исполнительным устройством
// --------------------------------------------------------------------
static void controlActuator(float output) {
    // TODO: Заменить на реальный актуатор
    // Пример для ШИМ (вентилятор, нагреватель, LED):
    // int pwmValue = constrain(map(output, 0, 255, 0, 255), 0, 255);
    // ledcWrite(PWM_CHANNEL, pwmValue);
    //
    // Пример для реле (включить если output > threshold):
    // digitalWrite(RELAY_PIN, output > 50 ? HIGH : LOW);
    
    // Выводим в Serial для отладки
    LOG_DEBUG("PID Output: " + String(output, 2));
}

// ====================================================================
// taskPID() - Основная задача PID регулятора
// ====================================================================
void taskPID(void *pvParameters) {
    (void)pvParameters;
    
    // Регистрация в Watchdog
    esp_task_wdt_add(NULL);
    
    // Инициализация PID
    pid.setGains(PID_Kp, PID_Ki, PID_Kd);
    pid.setIntegralLimits(PID_INTEGRAL_MIN, PID_INTEGRAL_MAX);
    pid.setOutputLimits(PID_OUTPUT_MIN, PID_OUTPUT_MAX);
    pid.reset();
    
    LOG_INFO("PID задача запущена");
    LOG_INFO("Kp=" + String(PID_Kp) + ", Ki=" + String(PID_Ki) + ", Kd=" + String(PID_Kd));
    LOG_INFO("Target=" + String(PID_TARGET));
    
    unsigned long lastTime = millis();
    
    // Основной цикл
    while (true) {
        // Сброс watchdog
        feedWatchdog();
        
        // Проверяем время для дискретизации
        unsigned long now = millis();
        if ((now - lastTime) >= (PID_DT * 1000)) {
            lastTime = now;
            
            // Чтение текущего значения датчика
            float current = readSensor();
            
            // Расчёт PID
            float output = pid.compute(PID_TARGET, current);
            
            // Управление актуатором
            controlActuator(output);
            
            // Отладочный вывод (можно закомментировать)
            LOG_DEBUG("Target=" + String(PID_TARGET) + 
                      ", Current=" + String(current) + 
                      ", Error=" + String(pid.getError(), 2) + 
                      ", Output=" + String(output, 2));
        }
        
        // Короткая задержка (не блокируем систему)
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

#endif // ENABLE_PID