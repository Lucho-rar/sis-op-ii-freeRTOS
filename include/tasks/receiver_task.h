#ifndef RECEIVER_TASK_H
#define RECEIVER_TASK_H

#include "FreeRTOS.h"
#include "sensor_task.h"
#include "task.h"
#include "utils.h"
#define MAX_VALUES_AVERAGE 10

/**
 * @brief Task function to receive sensor data, apply a moving average filter, and send the filtered value to a display
 * queue.
 *
 * This FreeRTOS task continuously receives integer sensor values from the xSensorQueue.
 * It maintains a buffer of the last MAX_VALUES_AVERAGE received values, shifting older values back
 * and inserting the newest value at the front. The task applies a moving average filter using the
 * most recent N values, where N is dynamically obtained via get_N_value(). The filtered result is
 * sent to the xDisplayQueue for further processing or display.
 *
 * Additional debug information is sent via UART, including the initial N value and, depending on
 * the SENSOR_10HZ configuration, the filtered value.
 *
 * @param pvParameters Pointer to task parameters (unused).
 */
void vTaskReceiverDataSensor(void* pvParameters);

#endif /* RECEIVER_TASK_H */