#ifndef DISPLAY_TASK_H
#define DISPLAY_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "utils.h"
#include "sensor_task.h"

#define DISPLAY_MIN_INPUT 0
#define DISPLAY_MAX_INPUT 20  
#define BITMAP_HALF_SIZE 7 // Mitad superior/inferior: 0-6

void vTaskDisplay(void *pvParameters);
char * bitMapping(int valor);
int mapToBitMapping(int value);

#endif /* DISPLAY_TASK_H */
