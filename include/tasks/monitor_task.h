#ifndef MONITOR_TASK_H
#define MONITOR_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "utils.h"

#define MAX_TASKS 10
/**
 * @brief Monitors and reports the status of all FreeRTOS tasks periodically via UART.
 *
 * This task gathers system state information for all running tasks, including their name,
 * priority, state, stack high water mark, and task number. The information is formatted
 * and sent over UART for monitoring purposes. Memory for the task status array is dynamically
 * allocated and freed on each iteration. If memory allocation fails, an error message is sent.
 * The task repeats every 10,000 ticks (typically 5 seconds, depending on tick rate).
 *
 * @param[in] pvParameters Pointer to task parameters (unused).
 */
void vTaskMonitor(void* pvParameters);

#endif // MONITOR_TASK_H