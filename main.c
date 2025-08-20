/* Environment includes. */
#include "DriverLib.h"




/* ################# includes ############################ */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>


/* ################ defines del demo #################### */
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


#define DISPLAY_MIN_INPUT 0
#define DISPLAY_MAX_INPUT 20  

#define BITMAP_MIN 13
#define BITMAP_MAX 19
#define BITMAP_RANGE (BITMAP_MAX - BITMAP_MIN + 1)
#define BITMAP_HALF_SIZE BITMAP_RANGE
/*
 * Configure the processor and peripherals for this demo.
 */
static void prvSetupHardware( void );



/*############## declaraciones de funciones propias ###############*/


// tareas
static void vTaskSensor ( );
static void vTaskReceiverDataSensor();
static void vTaskDisplay();
static void vTaskUpdateN();
static void vTaskMonitor();

// getters y funciones de print
static int get_N_value();
const char* getStateName(eTaskState state);
int putchar(int c);
int _write(int file, char *ptr, int len);
void sendUART0(const char *string);
static void vIntToString(int value, char *str) ;

// global
static char buffer[50];
int valor_ventana = 1;



// recursos compartidos
SemaphoreHandle_t xButtonSemaphore;

// handle de las colas y semáforos
QueueHandle_t xPrintQueue;
QueueHandle_t xSensorQueue;
QueueHandle_t xDisplayQueue;
QueueHandle_t xUpdateNQueue;
SemaphoreHandle_t xMutexN;



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

static void vTaskMonitor( void )
{
    char buffercin[64];
    TaskStatus_t *pxTaskStatusArray;
    UBaseType_t uxArraySize, uxTasksReturned;
    uint32_t ulTotalRunTime;
    
    while(1) {
        // Obtener el numero de tareas
        uxArraySize = uxTaskGetNumberOfTasks();

        // Alocar memoria para el array de estados de tareas
        pxTaskStatusArray = pvPortMalloc( uxArraySize * sizeof( TaskStatus_t ) );
        
        if( pxTaskStatusArray != NULL )
        {
            // Obtener los estados de las tareas
            uxTasksReturned = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, &ulTotalRunTime );
            
            sendUART0("\n\n[INFO] | vTaskMonitor | System Task Statistics:\n");
            sendUART0("==============================================\n");
            
            // Iterar a través de todas las tareas
            for(UBaseType_t i = 0; i < uxTasksReturned; i++) {
                sendUART0("\nTask: ");
                sendUART0(pxTaskStatusArray[i].pcTaskName);
                
                sendUART0("\n  Priority: ");
                vIntToString(pxTaskStatusArray[i].uxCurrentPriority, buffercin);
                sendUART0(buffercin);
                
                sendUART0("\n  State: ");
                sendUART0(getStateName(pxTaskStatusArray[i].eCurrentState));
                
                sendUART0("\n  Stack Free: ");
                vIntToString(pxTaskStatusArray[i].usStackHighWaterMark, buffercin);
                sendUART0(buffercin);
                sendUART0(" words");
                
                sendUART0("\n  Task Number: ");
                vIntToString(pxTaskStatusArray[i].xTaskNumber, buffercin);
                sendUART0(buffercin);
                
                sendUART0("\n  ----------------------------------------");
            }
            
            sendUART0("\n\nTotal Tasks: ");
            vIntToString(uxTasksReturned, buffercin);
            sendUART0(buffercin);
            sendUART0("\n");
            
            // Liberar la memoria después de usar los datos
            vPortFree( pxTaskStatusArray );
        }
        else {
            sendUART0("\n[ERROR] | vTaskMonitor | Memory allocation failed\n");
        }
        
        // Esperar 5 segundos antes del próximo reporte
        vTaskDelay(100000);
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
        vIntToString(temperature_v, buffercito); // Convertir el entero a cadena
        sendUART0("\n[INFO] | TaskSensor | Temperature: ");
        sendUART0(buffercito);
        sendUART0("ºC");
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

char* bitMapping(int valor) {
    if ((valor <= 13) || (valor == 20)) {
        return "\100"; // 01000000
    }
    if ((valor == 14) || (valor == 21)) {
        return " ";
    }
    if ((valor == 15) || (valor == 22)) {
        return "\020"; // 00010000
    }
    if ((valor == 16) || (valor == 23)){
        return "\010"; // 00001000
    }
    if ((valor == 17) || (valor == 24)){
        return "\004"; // 00000100
    }
    if ((valor == 18) || (valor == 25)) {
        return "\002"; // 00000010
    }
    if ((valor == 19) || (valor == 26)) {
        return "\001"; // 00000001
    }
    return " "; // Valor fuera de rango
}

int mapToBitMapping(int value) {
    if (value < 0) value = 0;
    if (value > 20) value = 20;
    // Mapea 0 -> 13, 20 -> 19 
    return 13 + ((value * 6) / 20); // 6 = (19-13)
}


static void vTaskDisplay() {
    int value_filter;
    static int buffer_onda[96] = {0};
    int i;


    OSRAMStringDraw( "FreeRTOS - SO2", 0, 0 );
    vTaskDelay(500);
    OSRAMClear();
    vTaskDelay(500);

    OSRAMStringDraw( "FreeRTOS - SO2", 0, 0 );
    vTaskDelay(500);
    OSRAMClear();
    vTaskDelay(500);
    
    OSRAMStringDraw( "FreeRTOS - SO2", 0, 0 );
    vTaskDelay(500);
    OSRAMClear();
    vTaskDelay(500);

    OSRAMStringDraw( "FreeRTOS - SO2", 0, 0 );
    vTaskDelay(2000);
    OSRAMClear();


    for (;;) {
        xQueueReceive(xDisplayQueue, &value_filter, portMAX_DELAY);

        // Desplazar el buffer y agregar el nuevo valor
        for (i = 95; i > 0; i--) {
            buffer_onda[i] = buffer_onda[i-1];
        }
        buffer_onda[0] = value_filter;

        OSRAMClear();

        // Normalización continua para todo el rango
        for (i = 0; i < 96; i++) {
            int input = buffer_onda[i];

            // Clamp input to range
            if (input < DISPLAY_MIN_INPUT) input = DISPLAY_MIN_INPUT;
            if (input > DISPLAY_MAX_INPUT) input = DISPLAY_MAX_INPUT;

            // Mapea todo el rango de entrada a 2*BITMAP_HALF_SIZE (mitad inferior y superior)
            int total_steps = 2 * BITMAP_HALF_SIZE;
            int mapped_continuous = (input - DISPLAY_MIN_INPUT) * total_steps / (DISPLAY_MAX_INPUT - DISPLAY_MIN_INPUT);

            int bit_map_half, mapped;
            if (mapped_continuous < BITMAP_HALF_SIZE) {
                bit_map_half = 1; // mitad inferior
                mapped = BITMAP_MIN + mapped_continuous;
            } else {
                bit_map_half = 0; // mitad superior
                mapped = BITMAP_MIN + (mapped_continuous - BITMAP_HALF_SIZE);
            }

            OSRAMImageDraw(bitMapping(mapped), i, bit_map_half, 1, 1);
        }

        vIntToString(value_filter, buffer);
        sendUART0("\n[INFO] | TaskDisplay | Filtered value: ");
        sendUART0(buffer);
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
