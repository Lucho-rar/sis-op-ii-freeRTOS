#include "monitor_task.h"

void vTaskMonitor(void *pvParameters)
{
    (void) pvParameters;

    char buffer_string[DEFAULT_SIZE_BUFFERS];
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
            
            sendUART("\n\n[INFO] | vTaskMonitor | System Task Statistics:\n");
            sendUART("==============================================\n");
            
            // Iterar a través de todas las tareas
            for(UBaseType_t i = 0; i < uxTasksReturned; i++) {
                sendUART("\nTask: ");
                sendUART(pxTaskStatusArray[i].pcTaskName);
                
                sendUART("\n  Priority: ");
                vIntToString(pxTaskStatusArray[i].uxCurrentPriority, buffer_string);
                sendUART(buffer_string);
                
                sendUART("\n  State: ");
                sendUART(getStateName(pxTaskStatusArray[i].eCurrentState));
                
                sendUART("\n  Stack Free: ");
                vIntToString(pxTaskStatusArray[i].usStackHighWaterMark, buffer_string);
                sendUART(buffer_string);
                sendUART(" words");
                
                sendUART("\n  Task Number: ");
                vIntToString(pxTaskStatusArray[i].xTaskNumber, buffer_string);
                sendUART(buffer_string);
                
                sendUART("\n  ----------------------------------------");
            }
            
            sendUART("\n\nTotal Tasks: ");
            vIntToString(uxTasksReturned, buffer_string);
            sendUART(buffer_string);
            sendUART("\n");
            
            // Liberar la memoria después de usar los datos
            vPortFree( pxTaskStatusArray );
        }
        else {
            sendUART("\n[ERROR] | vTaskMonitor | Memory allocation failed\n");
        }
        
        // Esperar 5 segundos antes del próximo reporte
        vTaskDelay(100000);
    }
}