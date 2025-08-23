#ifndef SENSOR_TASK_H
#define SENSOR_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "utils.h"

#define MAX_TEMPERATURE 20
#define MIN_TEMPERATURE 0
// #define SENSOR_10HZ 0           // 1 = 10Hz, 0 = 3s
#define DEFAULT_SIZE_BUFFERS 20
void vTaskSensor(void *pvParameters);

#endif /* SENSOR_TASK_H */