#include "board.h"

void boardInit()
{
    // valor_ventana = 1;
    prvSetupHardware();
}

void prvSetupHardware(void)
{
    /* Setup the PLL. */
    SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_6MHZ);

    /* Enable the UART.  */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    /* Set GPIO A0 and A1 as peripheral function.  They are used to output the
     * UART signals. */
    GPIODirModeSet(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_DIR_MODE_HW);

    /* Configure the UART for 8-N-1 operation. */
    UARTConfigSet(UART0_BASE, mainBAUD_RATE, UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE);

    /* We don't want to use the fifo.  This is for test purposes to generate
     * as many interrupts as possible. */
    HWREG(UART0_BASE + UART_O_LCR_H) |= mainFIFO_SET;

    /* Enable Tx interrupts. */
    // HWREG( UART0_BASE + UART_O_IM ) |= UART_INT_TX;
    IntPrioritySet(INT_UART0, configKERNEL_INTERRUPT_PRIORITY);
    IntEnable(INT_UART0);

    /* Enable Rx interrupts. */
    HWREG(UART0_BASE + UART_O_IM) |= UART_INT_RX;
    IntPrioritySet(INT_UART0, configKERNEL_INTERRUPT_PRIORITY);
    IntEnable(INT_UART0);

    // /* Initialise the LCD> */
    OSRAMInit(false);
}

void vUART_ISR(void)
{

    unsigned long ulStatus;
    int nuevo_N = 1;

    // Check flag
    ulStatus = UARTIntStatus(UART0_BASE, pdTRUE);

    // clean
    UARTIntClear(UART0_BASE, ulStatus);

    /* if RX */
    if (ulStatus & UART_INT_RX)
    {
        char c = HWREG(UART0_BASE + UART_O_DR); /*    Byte received   */
        if (c == 'U')                           // inc
        {
            nuevo_N = get_N_value() + 1;
            sendUART("\n[INFO] | UART_ISR | N incremented");
            xQueueSend(xUpdateNQueue, &nuevo_N, portMAX_DELAY);
        }
        else if (c == 'D') // dec
        {
            sendUART("\n[INFO] | UART_ISR | N decremented");
            nuevo_N = get_N_value() - 1;
            xQueueSend(xUpdateNQueue, &nuevo_N, portMAX_DELAY);
        }
    }
}
