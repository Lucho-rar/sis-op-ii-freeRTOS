#include "updater_n_task.h"

void vTaskUpdateN(void* pvParameters)
{
    (void)pvParameters;
    char buffer[50];
    int N;
    for (;;)
    {
        if (xQueueReceive(xUpdateNQueue, &N, portMAX_DELAY) == pdTRUE)
        {
            valor_ventana = N;
            vIntToString(N, buffer);
            sendUART("\n[INFO] | TaskUpdateN | N: ");
            sendUART(buffer);
        }
    }
}
