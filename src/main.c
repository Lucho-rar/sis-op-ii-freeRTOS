
#include "tasks/monitor_task.h"
#include "common/utils.h"
#include "tasks/display_task.h"
#include "tasks/sensor_task.h"
#include "DriverLib.h"





/*
 * Configure the processor and peripherals for this demo.
 */
static void prvSetupHardware( void );



/*############## declaraciones de funciones propias ###############*/


// tareas
// static void vTaskSensor ( );
static void vTaskReceiverDataSensor();
static void vTaskUpdateN();

// getters y funciones de print
static int get_N_value();
const char* getStateName(eTaskState state);
int putchar(int c);
int _write(int file, char *ptr, int len);

// global
static char buffer[50];
int valor_ventana = 1;






int main( void )
{
    prvSetupHardware();

    // creacion de tareas
    xTaskCreate(vTaskSensor, "Sensor", configMINIMAL_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY + 1, NULL);
    xTaskCreate(vTaskReceiverDataSensor, "Receiver", configMINIMAL_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY + 1, NULL);
    xTaskCreate(vTaskDisplay, "Display", configMINIMAL_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY + 1, NULL);
    xTaskCreate(vTaskUpdateN, "UpdateN", configMINIMAL_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY + 1, NULL);
    xTaskCreate(vTaskMonitor, "Monitor", configMINIMAL_STACK_SIZE * 2, NULL, mainCHECK_TASK_PRIORITY + 1, NULL);


    // mutex
    xMutexN = xSemaphoreCreateBinary();

    // colas 
    xSensorQueue = xQueueCreate(50, sizeof(unsigned long));
    xDisplayQueue = xQueueCreate(50 , sizeof(unsigned long ));
    xUpdateNQueue = xQueueCreate(50, sizeof(unsigned long));


    /* Start the scheduler. */
    vTaskStartScheduler();

    return 0;
}




/*
    @brief Task sensor
*/


static void vTaskReceiverDataSensor(){
    int values[10] = {0};
    int value;
    int filter ;
    int N ;
    N = get_N_value();
    vIntToString(N, buffer);
    sendUART0("\n[INFO] | TaskReceiverDataSensor | N initial: ");
    sendUART0(buffer);

    for ( ; ; ){
        xQueueReceive(xSensorQueue, &value, portMAX_DELAY);
        // Desplazo los valores hacia atrás en el buffer
        for (int i = 9; i > 0; i--) {
            values[i] = values[i-1];
        }
        values[0] = value;  // Guardo el nuevo valor en la primera posición

        // Aplico el filtro de la ventana (sumar los N valores y promedios)
        filter = 0;
        N = get_N_value();
        for (int i = 0 ; i < N ; i ++){
            filter += values[i];
        }

        filter = filter / N ;
        vIntToString(N, buffer);
        sendUART0("\n[INFO] | TaskReceiverDataSensor | Value filtered: ");
        sendUART0(buffer);

        xQueueSend(xDisplayQueue, &filter, portMAX_DELAY ); 
    }



}



// // TODO: mover esto a un archivo de cabecera
// int putchar(int c) {
//     UARTCharPut(UART0_BASE, c);  // Envía el carácter por la UART0
//     return c;
// }

static void vTaskUpdateN(){
    int N = 1;
    for ( ; ; ){
        if (xQueueReceive(xUpdateNQueue, &N, portMAX_DELAY) == pdTRUE){
            valor_ventana = N;
            vIntToString(N, buffer);
            sendUART0("\n[INFO] | TaskUpdateN | N: ");
            sendUART0(buffer);
        }
    }
}

static int get_N_value(){

    return valor_ventana;

}

/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
    /* Setup the PLL. */
    SysCtlClockSet( SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_6MHZ );

    /* Setup the push button. */
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOC );
    GPIODirModeSet( GPIO_PORTC_BASE, mainPUSH_BUTTON, GPIO_DIR_MODE_IN );
    GPIOIntTypeSet( GPIO_PORTC_BASE, mainPUSH_BUTTON, GPIO_FALLING_EDGE );
    IntPrioritySet( INT_GPIOC, configKERNEL_INTERRUPT_PRIORITY );
    GPIOPinIntEnable( GPIO_PORTC_BASE, mainPUSH_BUTTON );
    IntEnable( INT_GPIOC );



    /* Enable the UART.  */
    SysCtlPeripheralEnable( SYSCTL_PERIPH_UART0 );
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOA );

    /* Set GPIO A0 and A1 as peripheral function.  They are used to output the
     * UART signals. */
    GPIODirModeSet( GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_DIR_MODE_HW );

    /* Configure the UART for 8-N-1 operation. */
    UARTConfigSet( UART0_BASE, mainBAUD_RATE, UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE );

    /* We don't want to use the fifo.  This is for test purposes to generate
     * as many interrupts as possible. */
    HWREG(UART0_BASE + UART_O_LCR_H) |= mainFIFO_SET;


    /* Enable Tx interrupts. */
   // HWREG( UART0_BASE + UART_O_IM ) |= UART_INT_TX;
    IntPrioritySet( INT_UART0, configKERNEL_INTERRUPT_PRIORITY );
    IntEnable( INT_UART0 );

    /* Enable Rx interrupts. */
    HWREG( UART0_BASE + UART_O_IM ) |= UART_INT_RX;
    IntPrioritySet( INT_UART0, configKERNEL_INTERRUPT_PRIORITY );
    IntEnable( INT_UART0 );


    // /* Initialise the LCD> */
    OSRAMInit( false );
    // OSRAMStringDraw( "www.FreeRTOS.org", 0, 0 );
    // OSRAMStringDraw( "messi st", 16, 1 );
}


/*-----------------------------------------------------------*/

void vUART_ISR( void )
{

    unsigned long ulStatus;
    int nuevo_N = 0;

    // me fijo la flag de interrupcion
    ulStatus = UARTIntStatus( UART0_BASE, pdTRUE );

   //clean
    UARTIntClear( UART0_BASE, ulStatus );

    /* si es RX leo el caracter para ver si quiere aumentar o disminuir */
    if( ulStatus & UART_INT_RX )
    {
        char c = HWREG( UART0_BASE + UART_O_DR ); /*    Byte received   */
        if (c == 'U'){
            nuevo_N = get_N_value() + 1;
            sendUART0("\n[INFO] | UART_ISR | N incremented");
            xQueueSend(xUpdateNQueue, &nuevo_N, portMAX_DELAY);

        }else if (c == 'D'){
            sendUART0("\n[INFO] | UART_ISR | N decremented");
            nuevo_N = get_N_value() - 1;
            xQueueSend(xUpdateNQueue, &nuevo_N, portMAX_DELAY);
        }
    }

}
/*-----------------------------------------------------------*/

void vGPIO_ISR( void )
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    /* Clear the interrupt. */
    GPIOPinIntClear( GPIO_PORTC_BASE, mainPUSH_BUTTON );

    /* Wake the button handler task. */
    xSemaphoreGiveFromISR( xButtonSemaphore, &xHigherPriorityTaskWoken );

    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}
/*-----------------------------------------------------------*/


