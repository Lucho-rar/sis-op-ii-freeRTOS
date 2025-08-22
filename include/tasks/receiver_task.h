#ifndef RECEIVER_TASK_H
#define RECEIVER_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "utils.h"

void vTaskReceiverDataSensor(void *pvParameters);
int get_N_value();

#endif /* RECEIVER_TASK_H */