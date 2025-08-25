#ifndef SENSOR_TASK_H
#define SENSOR_TASK_H

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "utils.h"

#define MAX_TEMPERATURE 20
#define MIN_TEMPERATURE 0
#define SENSOR_10HZ 1 // 1 = 10Hz, 0 = 3s

/**
 * @brief Sensor task for FreeRTOS.
 *
 * This task simulates a temperature sensor by incrementing a variable from MIN_TEMPERATURE
 * up to MAX_TEMPERATURE, then resetting it to MIN_TEMPERATURE. The temperature value is sent
 * to a queue for further processing (e.g., filtering).
 *
 * The task supports two operating modes:
 * - Fast mode (10 Hz): Sends temperature values to the queue every 100 ms.
 * - Slow mode: Sends temperature values every 3000 ms and prints the value to UART for debugging.
 *
 * @param pvParameters Pointer to task parameters (unused).
 *
 * @note The operating mode is selected via the SENSOR_10HZ macro.
 * @note Uses xQueueSend to send temperature values to xSensorQueue.
 * @note Uses vTaskDelayUntil for periodic execution.
 */
void vTaskSensor(void* pvParameters);

#endif /* SENSOR_TASK_H */