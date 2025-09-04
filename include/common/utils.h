#ifndef UTILS_H
#define UTILS_H

/* Environment includes. */
#include "DriverLib.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define DEFAULT_SIZE_BUFFERS 64

/*
 * Global variables used for FreeRTOS synchronization and communication:
 *
 * xButtonSemaphore   - Semaphore for button event signaling.
 * xMutexN            - Mutex for protecting shared resource 'N'.
 * xPrintQueue        - Queue for print task messages.
 * xSensorQueue       - Queue for sensor data communication.
 * xUpdateNQueue      - Queue for updating 'N' related data.
 * xDisplayQueue      - Queue for display task messages.
 *
 * valor_ventana      - Integer variable representing the current window value (default: 1).
 */
extern SemaphoreHandle_t xButtonSemaphore;

extern QueueHandle_t xPrintQueue;
extern QueueHandle_t xSensorQueue;
extern QueueHandle_t xUpdateNQueue;
extern SemaphoreHandle_t xMutexN;
extern QueueHandle_t xDisplayQueue;

extern int valor_ventana;

/**
 * @brief Sends a null-terminated string over UART.
 *
 * This function iterates through each character in the provided string
 * and transmits it via UART using UARTCharPut. Transmission stops when
 * the null terminator is encountered.
 *
 * @param string Pointer to the null-terminated string to send.
 */
void sendUART(const char* string);

/**
 * @brief Converts an integer to its string representation.
 *
 * This function takes an integer value and converts it to a null-terminated string.
 * It handles negative numbers by prepending a '-' sign to the string.
 *
 * @param num The integer to convert.
 * @param str Pointer to a character array where the resulting string will be stored.
 *            The array must be large enough to hold the resulting string including the
 *            null terminator.
 *
 * @note The digits are processed in reverse order and then the string is reversed
 *       to obtain the correct representation.
 */
void vIntToString(int value, char* str);

/**
 * @brief Retrieves the value of the global variable 'valor_ventana'.
 *
 * This function returns the current value stored in the global variable 'valor_ventana'.
 *
 * @return int The value of 'valor_ventana'.
 */
int get_N_value();

/**
 * @brief Returns the string representation of a FreeRTOS task state.
 *
 * This function maps an eTaskState enumeration value to its corresponding
 * human-readable name as a string. If the state does not match any known
 * value, "Unknown" is returned.
 *
 * @param state The eTaskState value representing the task state.
 * @return A constant string with the name of the task state.
 */
const char* getStateName(eTaskState state);

void vConfigureTimerForRunTimeStats(void);
#define PRIORITY_LOW 1
#define PRIORITY_MEDIUM 2
#define PRIORITY_HIGH 3
#define PRIORITY_CRITICAL 4

#define STACK_SIZE_SENSE 80
#define STACK_SIZE_RECEIVER 160
#define STACK_SIZE_DISPLAY 100
#define STACK_SIZE_UPDATE 120
#define STACK_SIZE_MONITOR 80

#endif /* UTILS_H */