#ifndef MONITOR_TASK_H
#define MONITOR_TASK_H

#include "FreeRTOS.h"
#include "task.h"

void vTaskMonitor(void *pvParameters);

#endif // MONITOR_TASK_H