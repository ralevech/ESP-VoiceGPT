// ====================================================================
// pid_controller.cpp - Реализация PID регулятора
// 
// Описание: Реализация методов класса PIDController
// Автор: ralevech
// ====================================================================

#include "pid_controller.h"

// -----------------------------------------------------------------
// Конструктор
// -----------------------------------------------------------------
PIDController::PIDController(float dt) {
    m_dt = dt;
    m_Kp = 1.0;
    m_Ki = 0.0;
    m_Kd = 0.0;
    
    m_integralMin = -1e6;
    m_integralMax = 1e6;
    m_outputMin = -255.0;
    m_outputMax = 255.0;
    
    m_error = 0;
    m_previousError = 0;
    m_integral = 0;
    m_derivative = 0;
    m_output = 0;
}

// -----------------------------------------------------------------
// Установка коэффициентов PID
// -----------------------------------------------------------------
void PIDController::setGains(float Kp, float Ki, float Kd) {
    m_Kp = Kp;
    m_Ki = Ki;
    m_Kd = Kd;
}

// -----------------------------------------------------------------
// Установка лимитов интегральной составляющей (антивиндовап)
// -----------------------------------------------------------------
void PIDController::setIntegralLimits(float min, float max) {
    m_integralMin = min;
    m_integralMax = max;
}

// -----------------------------------------------------------------
// Установка лимитов выходного сигнала
// -----------------------------------------------------------------
void PIDController::setOutputLimits(float min, float max) {
    m_outputMin = min;
    m_outputMax = max;
}

// -----------------------------------------------------------------
// Сброс интегральной составляющей
// -----------------------------------------------------------------
void PIDController::resetIntegral() {
    m_integral = 0;
}

// -----------------------------------------------------------------
// Сброс всех внутренних состояний
// -----------------------------------------------------------------
void PIDController::reset() {
    m_error = 0;
    m_previousError = 0;
    m_integral = 0;
    m_derivative = 0;
    m_output = 0;
}

// -----------------------------------------------------------------
// Изменение периода дискретизации
// -----------------------------------------------------------------
void PIDController::setDt(float dt) {
    m_dt = dt;
}

// -----------------------------------------------------------------
// Основной расчёт PID
// -----------------------------------------------------------------
float PIDController::compute(float target, float current) {
    // Расчёт ошибки
    m_error = target - current;
    
    // Пропорциональная составляющая
    float pTerm = m_Kp * m_error;
    
    // Интегральная составляющая (с ограничением)
    m_integral += m_error * m_dt;
    m_integral = constrain(m_integral, m_integralMin, m_integralMax);
    float iTerm = m_Ki * m_integral;
    
    // Дифференциальная составляющая
    if (m_dt > 0) {
        m_derivative = (m_error - m_previousError) / m_dt;
    } else {
        m_derivative = 0;
    }
    float dTerm = m_Kd * m_derivative;
    
    // Сохраняем ошибку для следующего шага
    m_previousError = m_error;
    
    // Суммируем
    m_output = pTerm + iTerm + dTerm;
    
    // Ограничиваем выход
    m_output = constrain(m_output, m_outputMin, m_outputMax);
    
    return m_output;
}