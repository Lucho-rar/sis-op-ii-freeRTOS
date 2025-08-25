#ifndef UPDATER_N_H
#define UPDATER_N_H

#include "utils.h"
#include <FreeRTOS.h>
#include <task.h>

/**
 * @brief Task function for updating the value of N.
 *
 * This FreeRTOS task waits indefinitely to receive an integer value N from the xUpdateNQueue.
 * Upon receiving a value, it updates the global variable 'valor_ventana' with N, converts N to a string,
 * and sends an informational message via UART.
 *
 * @param pvParameters Pointer to task parameters (unused).
 */
void vTaskUpdateN(void* pvParameters);

#endif /* UPDATER_N_H */