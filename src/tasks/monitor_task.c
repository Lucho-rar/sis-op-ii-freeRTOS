#include "monitor_task.h"
#define MAX_TASKS 10
void vTaskMonitor(void* pvParameters)
{
    (void)pvParameters;

    char buffer_string[DEFAULT_SIZE_BUFFERS];
    TaskStatus_t* pxTaskStatusArray;
    UBaseType_t uxArraySize, uxTasksReturned;
    uint32_t ulTotalRunTime;

    // get number tasks
    uxArraySize = uxTaskGetNumberOfTasks();

    // reserve memory for array
    pxTaskStatusArray = pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));
    if (pxTaskStatusArray == NULL)
        vTaskDelete(NULL);
    while (1)
    {
        if (pxTaskStatusArray != NULL)
        {
            // Get the task states
            uxTasksReturned = uxTaskGetSystemState(pxTaskStatusArray, MAX_TASKS, &ulTotalRunTime);

            sendUART("\n\n[INFO] | vTaskMonitor | System Task Statistics:\n");
            sendUART("==============================================\n");

            // Iterate through all tasks
            for (UBaseType_t i = 0; i < uxTasksReturned; i++)
            {
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

                uint32_t percentCPU = 0;
                if (ulTotalRunTime > 0)
                    percentCPU = (pxTaskStatusArray[i].ulRunTimeCounter * 100UL) / ulTotalRunTime;

                sendUART("\n  CPU Usage: ");
                vIntToString(percentCPU, buffer_string);
                sendUART(buffer_string);
                sendUART(" %");

                sendUART("\n  ----------------------------------------");
            }

            sendUART("\n\nTotal Tasks: ");
            vIntToString(uxTasksReturned, buffer_string);
            sendUART(buffer_string);
            sendUART("\n");

            // Free the memory after use
            vPortFree(pxTaskStatusArray);
        }
        else
        {
            sendUART("\n[ERROR] | vTaskMonitor | Memory allocation failed\n");
        }

        // wait
        vTaskDelay(10000);
    }
}