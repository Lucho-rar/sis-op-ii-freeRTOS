#ifndef BOARD_H
#define BOARD_H

#include "utils.h"
#include <stdbool.h>
#include <stdint.h>
/* ################ defines del demo #################### */
/* Delay between cycles of the 'check' task. */
#define mainCHECK_DELAY ((TickType_t)5000 / portTICK_PERIOD_MS)
#define mainBAUD_RATE (19200)
#define mainFIFO_SET (0x10)
/* Demo task priorities. */
#define mainQUEUE_POLL_PRIORITY (tskIDLE_PRIORITY + 2)
#define mainCHECK_TASK_PRIORITY (tskIDLE_PRIORITY + 3)
#define mainSEM_TEST_PRIORITY (tskIDLE_PRIORITY + 1)
#define mainBLOCK_Q_PRIORITY (tskIDLE_PRIORITY + 2)

/* Misc. */
#define mainQUEUE_SIZE (3)
#define mainDEBOUNCE_DELAY ((TickType_t)150 / portTICK_PERIOD_MS)
#define mainNO_DELAY ((TickType_t)0)

/**
 * @brief Initializes the board hardware.
 *
 * This function sets up the hardware required for the board to operate.
 * It calls prvSetupHardware() to perform the necessary initialization.
 */
void boardInit(void);

/**
 * @brief Initializes and configures the hardware peripherals required for the board.
 *
 * This function performs the following initializations:
 * - Sets up the system clock using the PLL with a 6MHz crystal.
 * - Enables the UART0 and GPIOA peripherals.
 * - Configures GPIO pins A0 and A1 for UART peripheral function.
 * - Sets up UART0 for 8 data bits, no parity, and 1 stop bit (8-N-1).
 * - Disables UART FIFO to generate more interrupts for testing purposes.
 * - Enables and sets priority for UART0 transmit and receive interrupts.
 * - Initializes the LCD display.
 *
 * @note The UART FIFO is disabled to maximize interrupt generation for test purposes.
 */
void prvSetupHardware(void);

/**
 * @brief UART interrupt service routine.
 *
 * This ISR handles UART interrupts for UART0. It checks the interrupt status,
 * clears the interrupt flags, and processes received characters.
 * If the received character is 'U', it increments the value of N and sends an update
 * to the xUpdateNQueue. If the character is 'D', it decrements N and sends the update.
 * Informational messages are sent via UART for both increment and decrement actions.
 *
 * @note Assumes existence of get_N_value(), sendUART(), and xUpdateNQueue.
 */
// void vUART_ISR(void);

#endif /* BOARD_H */