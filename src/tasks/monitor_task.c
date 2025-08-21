#include "monitor_task.h"

void vTaskMonitor(void *pvParameters)
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