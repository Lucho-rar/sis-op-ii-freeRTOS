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


/* ################ defines del demo #################### */
/* Delay between cycles of the 'check' task. */
#define mainCHECK_DELAY            ( ( TickType_t ) 5000 / portTICK_PERIOD_MS )
#define mainBAUD_RATE              ( 19200 )
#define mainFIFO_SET               ( 0x10 )
/* Demo task priorities. */
#define mainQUEUE_POLL_PRIORITY    ( tskIDLE_PRIORITY + 2 )
#define mainCHECK_TASK_PRIORITY    ( tskIDLE_PRIORITY + 3 )
#define mainSEM_TEST_PRIORITY      ( tskIDLE_PRIORITY + 1 )
#define mainBLOCK_Q_PRIORITY       ( tskIDLE_PRIORITY + 2 )
/* Demo board specifics. */
#define mainPUSH_BUTTON            GPIO_PIN_4
/* Misc. */
#define mainQUEUE_SIZE             ( 3 )
#define mainDEBOUNCE_DELAY         ( ( TickType_t ) 150 / portTICK_PERIOD_MS )
#define mainNO_DELAY               ( ( TickType_t ) 0 )


// recursos compartidos
extern SemaphoreHandle_t xButtonSemaphore;


// handle de las colas y semáforos
extern QueueHandle_t xPrintQueue;
extern QueueHandle_t xSensorQueue;
extern QueueHandle_t xUpdateNQueue;
extern SemaphoreHandle_t xMutexN;
extern QueueHandle_t xDisplayQueue;



void sendUART0(const char *string);
void vIntToString(int value, char *str);

#endif /* UTILS_H */