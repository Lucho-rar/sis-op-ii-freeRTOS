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
#include <string.h>

#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>

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
static void vTaskReceiverDataSensor();
static void vTaskDisplay();
static void vTaskUpdateN();
//void vAFunction( void );
static void vTaskMonitor();
static int get_N_value();
const char* getStateName(eTaskState state);
int putchar(int c);
int _write(int file, char *ptr, int len);
void imprimir(const char *fmt, ...);
void sendUART0(const char *string);

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
QueueHandle_t xDisplayQueue;
QueueHandle_t xUpdateNQueue;
SemaphoreHandle_t xMutexN;

int valor_ventana = 1;


/******************************* */

#include <sys/stat.h>
#include <unistd.h>



/*-----------------------------------------------------------*/

int main( void )
{
    prvSetupHardware();


    xTaskCreate(vTaskSensor, "Sensor", configMINIMAL_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY + 1, NULL);
    xTaskCreate(vTaskReceiverDataSensor, "Receiver", configMINIMAL_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY + 1, NULL);
    xTaskCreate(vTaskDisplay, "Display", configMINIMAL_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY + 1, NULL);
    xTaskCreate(vTaskUpdateN, "UpdateN", configMINIMAL_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY + 1, NULL);
     xTaskCreate(vTaskMonitor, "Monitor", configMINIMAL_STACK_SIZE * 2, NULL, mainCHECK_TASK_PRIORITY + 1, NULL);
    xMutexN = xSemaphoreCreateBinary();

    xSensorQueue = xQueueCreate(50, sizeof(unsigned long));
    xDisplayQueue = xQueueCreate(50 , sizeof(unsigned long ));
    xUpdateNQueue = xQueueCreate(50, sizeof(unsigned long));
    /* Start the scheduler. */
    vTaskStartScheduler();
    /* Will only get here if there was insufficient heap to start the
     * scheduler. */

    return 0;
}

static void vTaskMonitor( void )
{
    char buffercin[64];
    TaskHandle_t xHandle;
    TaskStatus_t xTaskDetails;
    while(1){
        xHandle = xTaskGetHandle( "Sensor" );
        configASSERT( xHandle );
        vTaskGetInfo(xHandle, &xTaskDetails, pdTRUE, eInvalid);
        
        sendUART0("\n[INFO] | vTaskMonitor | Task Statistics:\n");
        sendUART0(xTaskDetails.pcTaskName);


        // Si sprintf te rompe, prueba esto:
        memset(buffercin, 0, sizeof(buffercin));
       sendUART0("\nPriority: ");
       // snprintf(buffercin, sizeof(buffercin), "Prioridad: %u\r\n", xTaskDetails.uxCurrentPriority);
           // sendUART0(buffercin);
       // sendUART0(
        vIntToString(xTaskDetails.uxCurrentPriority, buffercin);
        sendUART0(buffercin);
           


        memset(buffercin, 0, sizeof(buffercin));
        vIntToString(xTaskDetails.usStackHighWaterMark, buffercin);
        sendUART0("\nStack free: ");
        sendUART0(buffercin);

        vTaskDelay(5000);
    }
}
const char* getStateName(eTaskState state) {
    switch (state) {
        case eRunning: return "Running";
        case eReady: return "Ready";
        case eBlocked: return "Blocked";
        case eSuspended: return "Suspended";
        case eDeleted: return "Deleted";
        default: return "Unknown";
    }
}
/*
    @brief Task sensor
*/
static void vTaskSensor ( ){
    // simulo una funcion que llegue a 20 y vuelva a cero, para que sea variable la "medicion" del sensor
    int temperature_v = 0;         
    char buffercito[64];
    for ( ; ; ){
        if (temperature_v < 20){
            temperature_v ++ ; 
        }else{
            temperature_v = 0;
        }

        vTaskDelay(3000);
        xQueueSend(xSensorQueue, &temperature_v, portMAX_DELAY); // Se lo envio al filtro
        // sendUART0("hola wachin\n");
        vIntToString(temperature_v, buffercito); // Convertir el entero a cadena
        sendUART0("\n[INFO] | TaskSensor | Temperature: ");
        sendUART0(buffercito);
        sendUART0("ºC");
        //vAFunction(); // Llamo a la funcion que imprime las estadisticas de la tarea
     //   getTaskStatistics();
        // Enviar el valor de temperature_v
        //itoa(temperature_v, buffercito, 10); // Convertir el entero a cadena
        // // Enviar la cadena a través de UART0
        // sendUART0(buffercito);
        //snprintf(buffercito, sizeof(buffercito), "Raw temperature: %d\n", temperature_v);
        // sendUART0("\nRaw temperature: ");
        // sendUART0(buffercito);

        //imprimir("Temperatura: %s\n", buffercito);
        // vIntToString(temperature_v, buffercito);
        // OSRAMClear();
        // OSRAMStringDraw( buffercito, 0, 0 );
    }

}

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
        
        for (int i = 9; i > 0; i--) {  // Desplazo los valores hacia atrás en el buffer
            values[i] = values[i-1];
        }
        values[0] = value;  // Guardo el nuevo valor en la primera posición


        // Aplico el filtro de la ventana (sumar los N valores y promiedos)
        filter = 0;
        N = get_N_value();
        for (int i = 0 ; i < N ; i ++){
            filter += values[i];
        }
        //TODO debug
        filter = filter / N ;
        vIntToString(N, buffer);
        sendUART0("\n[INFO] | TaskReceiverDataSensor | Value filtered: ");
        sendUART0(buffer);
        // vIntToString(values[9], buffer);
        // sendUART0("\nUltimo valor: ");
        //vIntToString(filter, buffer);
        //sendUART0(buffer);
        // vIntToString(N, buffer);
        // imprimir("Filtro: %s\n", buffer);
        // OSRAMClear();
        // OSRAMStringDraw(buffer, 0, 0);
        // vTaskDelay(1000);
        xQueueSend(xDisplayQueue, &filter, portMAX_DELAY ); // se lo mando al display
    }



}

static void vTaskDisplay() {
    int value_filter; 
    int n_filter = 0;

    for ( ; ; ) {
        xQueueReceive(xDisplayQueue , &value_filter , portMAX_DELAY );
        vIntToString(value_filter, buffer);
        sendUART0("\n[INFO] | TaskDisplay | Filtered value: ");
        sendUART0(buffer);
        OSRAMClear() ;
        OSRAMStringDraw(buffer, 0, 0);
    }
}



// TODO: mover esto a un archivo de cabecera
int putchar(int c) {
    UARTCharPut(UART0_BASE, c);  // Envía el carácter por la UART0
    return c;
}

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

void imprimir(const char *fmt, ...) {
    char buffer[64];  // Buffer para almacenar la cadena formateada
    va_list args;
    va_start(args, fmt);

    // Usar vsprintf para formatear la cadena
    vsprintf(buffer, fmt, args);

    // Enviar el mensaje a través de UART carácter por carácter
    for (int i = 0; buffer[i] != '\0'; i++) {
        putchar(buffer[i]);
    }

    va_end(args);
}



/**
 * @brief Enviar una cadena de caracteres a través de UART0.
 * 
 * @param string Cadena de caracteres a enviar.
 */
void sendUART0(const char *string) {
	while (*string != '\0') {
		UARTCharPut(UART0_BASE, *string);
		string++;
	}
}


void UARTCharPutSafe(uint32_t base, char c) {
    while (HWREG(base + UART_O_FR) & UART_FR_TXFF); // Espera si el FIFO está lleno
    HWREG(base + UART_O_DR) = c;
}

