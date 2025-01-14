/* Environment includes. */
#include "DriverLib.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Demo app includes. */
#include "integer.h"
#include "PollQ.h"
#include "semtest.h"
#include "BlockQ.h"
#include <stdio.h>
#include <stdlib.h>
/* Delay between cycles of the 'check' task. */
#define mainCHECK_DELAY            ( ( TickType_t ) 5000 / portTICK_PERIOD_MS )

/* UART configuration - note this does not use the FIFO so is not very
 * efficient. */
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

/*
 * Configure the processor and peripherals for this demo.
 */
static void prvSetupHardware( void );

/*
 * The 'check' task, as described at the top of this file.
 */
static void vCheckTask( void * pvParameters );

/*
 * The task that is woken by the ISR that processes GPIO interrupts originating
 * from the push button.
 */
static void vButtonHandlerTask( void * pvParameters );

/*
 * The task that controls access to the LCD.
 */
static void vPrintTask( void * pvParameter );

static void vTaskSensor ( );

static void vIntToString(int value, char *str) ;
/* String that is transmitted on the UART. */
static char * cMessage = "Task woken by button interrupt! --- ";
static volatile char * pcNextChar;
static int temperature = 22;
static char buffer[50];

static int signal = 0;
/* The semaphore used to wake the button handler task from within the GPIO
 * interrupt handler. */
SemaphoreHandle_t xButtonSemaphore;

/* The queue used to send strings to the print task for display on the LCD. */
QueueHandle_t xPrintQueue;

QueueHandle_t xSensorQueue;

/*-----------------------------------------------------------*/

int main( void )
{
    /* Configure the clocks, UART and GPIO. */
    prvSetupHardware();

    // /* Create the semaphore used to wake the button handler task from the GPIO
    //  * ISR. */
    // vSemaphoreCreateBinary( xButtonSemaphore );
    // xSemaphoreTake( xButtonSemaphore, 0 );

    // /* Create the queue used to pass message to vPrintTask. */
    // xPrintQueue = xQueueCreate( mainQUEUE_SIZE, sizeof( char * ) );

    // /* Start the standard demo tasks. */
    // vStartIntegerMathTasks( tskIDLE_PRIORITY );
    // vStartPolledQueueTasks( mainQUEUE_POLL_PRIORITY );
    // vStartSemaphoreTasks( mainSEM_TEST_PRIORITY );
    // vStartBlockingQueueTasks( mainBLOCK_Q_PRIORITY );

    /* Start the tasks defined within the file. */
    // xTaskCreate( vCheckTask, "Check", configMINIMAL_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY, NULL );
    // xTaskCreate( vButtonHandlerTask, "Status", configMINIMAL_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY + 1, NULL );
    // xTaskCreate( vPrintTask, "Print", configMINIMAL_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY - 1, NULL );


    xTaskCreate(vTaskSensor, "Sensor", configMINIMAL_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY + 1, NULL);
    xSensorQueue = xQueueCreate(50, sizeof(unsigned long));

    /* Start the scheduler. */
    vTaskStartScheduler();

    /* Will only get here if there was insufficient heap to start the
     * scheduler. */

    return 0;
}


static void vTaskSensor ( ){
    // simulo una funcion que llegue a 20 y vuelva a cero, para que sea variable la "medicion" del sensor
    int temperature_v = 0;         
    for ( ; ; ){
        if (temperature_v < 20){
            temperature_v ++ ; 
        }else{
            temperature_v = 0;
        }

        // clean buffer y de momento se imprime solamente
        //memset(buffer, 0 , sizeof(buffer));
        vTaskDelay(1000);
        xQueueSend(xSensorQueue, temperature_v, portMAX_DELAY);
        //vIntToString(temperature_v, buffer);
        //OSRAMClear();
        //OSRAMStringDraw( buffer, 0, 0 );
    }

}

static void vTaskReceiverDataSensor(){
    int value;

    for ( ; ; ){
        xQueueReceive(xSensorQueue, &value, portMAX_DELAY);
        vTaskDelay(1000);
        vIntToString(value);
        OSRAMClear();
        OSRAMStringDraw(buffer, 0 ,0 );

    }



}




static void vCheckTask( void * pvParameters )
{
    portBASE_TYPE xErrorOccurred = pdFALSE;
    TickType_t xLastExecutionTime;
    const char * pcPassMessage = "PASS";
    const char * pcFailMessage = "FAIL";

    /* Initialise xLastExecutionTime so the first call to vTaskDelayUntil()
     * works correctly. */
    xLastExecutionTime = xTaskGetTickCount();

    for( ; ; )
    {
        /* Perform this check every mainCHECK_DELAY milliseconds. */
        vTaskDelayUntil( &xLastExecutionTime, mainCHECK_DELAY );

        /* Has an error been found in any task? */

        if( xAreIntegerMathsTaskStillRunning() != pdTRUE )
        {
            xErrorOccurred = pdTRUE;
        }

        if( xArePollingQueuesStillRunning() != pdTRUE )
        {
            xErrorOccurred = pdTRUE;
        }

        if( xAreSemaphoreTasksStillRunning() != pdTRUE )
        {
            xErrorOccurred = pdTRUE;
        }

        if( xAreBlockingQueuesStillRunning() != pdTRUE )
        {
            xErrorOccurred = pdTRUE;
        }

        /* Send either a pass or fail message.  If an error is found it is
         * never cleared again.  We do not write directly to the LCD, but instead
         * queue a message for display by the print task. */
        if( xErrorOccurred == pdTRUE )
        {
            xQueueSend( xPrintQueue, &pcFailMessage, portMAX_DELAY );
        }
        else
        {
            xQueueSend( xPrintQueue, &pcPassMessage, portMAX_DELAY );
        }
    }
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
    HWREG( UART0_BASE + UART_O_LCR_H ) &= ~mainFIFO_SET;

    /* Enable Tx interrupts. */
    HWREG( UART0_BASE + UART_O_IM ) |= UART_INT_TX;
    IntPrioritySet( INT_UART0, configKERNEL_INTERRUPT_PRIORITY );
    IntEnable( INT_UART0 );


    // /* Initialise the LCD> */
    OSRAMInit( false );
    // OSRAMStringDraw( "www.FreeRTOS.org", 0, 0 );
    // OSRAMStringDraw( "messi st", 16, 1 );
}
/*-----------------------------------------------------------*/

static void vButtonHandlerTask( void * pvParameters )
{
    const char * pcInterruptMessage = "Int";

    for( ; ; )
    {
        /* Wait for a GPIO interrupt to wake this task. */
        while( xSemaphoreTake( xButtonSemaphore, portMAX_DELAY ) != pdPASS )
        {
        }

        /* Start the Tx of the message on the UART. */
        UARTIntDisable( UART0_BASE, UART_INT_TX );
        {
            pcNextChar = cMessage;

            /* Send the first character. */
            if( !( HWREG( UART0_BASE + UART_O_FR ) & UART_FR_TXFF ) )
            {
                HWREG( UART0_BASE + UART_O_DR ) = *pcNextChar;
            }

            pcNextChar++;
        }
        UARTIntEnable( UART0_BASE, UART_INT_TX );

        /* Queue a message for the print task to display on the LCD. */
        xQueueSend( xPrintQueue, &pcInterruptMessage, portMAX_DELAY );

        /* Make sure we don't process bounces. */
        vTaskDelay( mainDEBOUNCE_DELAY );
        xSemaphoreTake( xButtonSemaphore, mainNO_DELAY );
    }
}

/*-----------------------------------------------------------*/

void vUART_ISR( void )
{
    unsigned long ulStatus;

    /* What caused the interrupt. */
    ulStatus = UARTIntStatus( UART0_BASE, pdTRUE );

    /* Clear the interrupt. */
    UARTIntClear( UART0_BASE, ulStatus );

    /* Was a Tx interrupt pending? */
    if( ulStatus & UART_INT_TX )
    {
        /* Send the next character in the string.  We are not using the FIFO. */
        if( *pcNextChar != 0 )
        {
            if( !( HWREG( UART0_BASE + UART_O_FR ) & UART_FR_TXFF ) )
            {
                HWREG( UART0_BASE + UART_O_DR ) = *pcNextChar;
            }

            pcNextChar++;
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

static void vPrintTask( void * pvParameters )
{
    char * pcMessage;
    unsigned portBASE_TYPE uxLine = 0, uxRow = 0;

    for( ; ; )
    {
        /* Wait for a message to arrive. */
        xQueueReceive( xPrintQueue, &pcMessage, portMAX_DELAY );

        /* Write the message to the LCD. */
        uxRow++;
        uxLine++;
        OSRAMClear();
        OSRAMStringDraw( pcMessage, uxLine & 0x3f, uxRow & 0x01 );
    }
}


void vIntToString(int num, char *str) {
    int i = 0;
    int is_negative = 0;

    // Manejar el caso de número negativo
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    // Convertir cada dígito del número en un carácter
    do {
        str[i++] = (num % 10) + '0'; // Convertir el dígito a su carácter ASCII correspondiente
        num /= 10;
    } while (num > 0);

    // Agregar el signo negativo si es necesario
    if (is_negative) {
        str[i++] = '-';
    }

    // Terminar la cadena
    str[i] = '\0';

    // Invertir la cadena porque los dígitos fueron agregados en orden inverso
    int start = 0;
    int end = i - 1;
    char temp;
    while (start < end) {
        temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}