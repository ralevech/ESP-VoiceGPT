// объявления всех задач
#ifndef TASKS_H
#define TASKS_H

// Объявления всех задач
void taskWiFi(void *pvParameters);
void taskBlink(void *pvParameters);
void taskSerial(void *pvParameters);
void taskWebServer(void *pvParameters);
void taskSensor(void *pvParameters);

// Будущие задачи
// void taskReadSensor(void *pvParameters);
// void taskMQTT(void *pvParameters);

#endif