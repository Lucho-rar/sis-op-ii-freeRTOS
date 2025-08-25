#ifndef DISPLAY_TASK_H
#define DISPLAY_TASK_H

#include "FreeRTOS.h"
#include "sensor_task.h"
#include "task.h"
#include "utils.h"

#define DISPLAY_MIN_INPUT 0
#define DISPLAY_MAX_INPUT 20
#define BITMAP_HALF_SIZE 7 // Mitad superior/inferior: 0-6

/**
 * @brief Task function for displaying filtered sensor values on an OSRAM display.
 *
 * This FreeRTOS task receives filtered values from a queue, maintains a buffer of the last 96 values,
 * and displays them as a waveform on the OSRAM display. The display is cleared and updated with each new value.
 * The input values are normalized and mapped to fit the display's bitmap range.
 * Optionally, the filtered value is sent over UART for debugging purposes.
 *
 * @param pvParameters Pointer to task parameters (unused).
 *
 * Behavior:
 * - Shows a startup message multiple times with delays.
 * - Continuously receives filtered values from xDisplayQueue.
 * - Shifts the buffer to accommodate the new value.
 * - Normalizes and maps each value for display.
 * - Draws the waveform on the OSRAM display.
 * - Optionally sends the filtered value over UART if SENSOR_10HZ is not defined.
 */
void vTaskDisplay(void* pvParameters);

/**
 * @brief Maps an integer value to a specific bit pattern represented as a string.
 *
 * This function takes an integer input and returns a string containing an escape sequence
 * that represents a specific bit pattern. The mapping is typically used for display or
 * hardware control purposes, where each value corresponds to a particular row or segment.
 *
 * @param valor The integer value to be mapped (expected range: 0-6).
 *              - 0: Returns "\100" (binary 01000000)
 *              - 1: Returns "\020" (binary 00010000)
 *              - 2: Returns "\010" (binary 00001000)
 *              - 3: Returns "\004" (binary 00000100)
 *              - 4: Returns "\002" (binary 00000010)
 *              - 5: Returns "\001" (binary 00000001)
 *              - 6: Returns "\001" (last row also lit)
 *              - Any other value: Returns a single space " "
 *
 * @return A string representing the corresponding bit pattern or a space if the value is out of range.
 */
char* bitMapping(int valor);

/**
 * @brief Maps an input value to a corresponding bit mapping index.
 *
 * This function clamps the input value to the range defined by DISPLAY_MIN_INPUT and DISPLAY_MAX_INPUT.
 * It then scales the value to fit within the range of 0 to BITMAP_HALF_SIZE, which is typically used
 * for indexing into a bitmap or display array.
 *
 * @param value The input value to be mapped.
 * @return The mapped index in the range 0 to BITMAP_HALF_SIZE.
 */
int mapToBitMapping(int value);

#endif /* DISPLAY_TASK_H */
