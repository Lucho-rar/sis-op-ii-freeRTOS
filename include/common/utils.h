#ifndef UTILS_H
#define UTILS_H


/* Environment includes. */
#include "DriverLib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>


#define DEFAULT_SIZE_BUFFERS 64


// recursos compartidos
extern SemaphoreHandle_t xButtonSemaphore;


// handle de las colas y semáforos
extern QueueHandle_t xPrintQueue;
extern QueueHandle_t xSensorQueue;
extern QueueHandle_t xUpdateNQueue;
extern SemaphoreHandle_t xMutexN;
extern QueueHandle_t xDisplayQueue;

extern int valor_ventana;

void sendUART(const char *string);
void vIntToString(int value, char *str);
int get_N_value();
const char* getStateName(eTaskState state) ;
#endif /* UTILS_H */