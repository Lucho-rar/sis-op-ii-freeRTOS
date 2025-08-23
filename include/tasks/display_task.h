#ifndef DISPLAY_TASK_H
#define DISPLAY_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "utils.h"
#include "sensor_task.h"
#define DISPLAY_MIN_INPUT 0
#define DISPLAY_MAX_INPUT 20  

#define BITMAP_MIN 13
#define BITMAP_MAX 19
#define BITMAP_RANGE (BITMAP_MAX - BITMAP_MIN + 1)
#define BITMAP_HALF_SIZE BITMAP_RANGE



void vTaskDisplay(void *pvParameters);
char * bitMapping(int valor);
int mapToBitMapping(int value);

#endif /* DISPLAY_TASK_H */