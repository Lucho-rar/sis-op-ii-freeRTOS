#ifndef RECEIVER_TASK_H
#define RECEIVER_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "utils.h"
#include "sensor_task.h"
#define MAX_VALUES_AVERAGE 10

void vTaskReceiverDataSensor(void *pvParameters);

#endif /* RECEIVER_TASK_H */