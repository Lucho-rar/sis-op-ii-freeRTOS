#ifndef SENSOR_TASK_H
#define SENSOR_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "utils.h"

void vTaskSensor(void *pvParameters);

#endif /* SENSOR_TASK_H */