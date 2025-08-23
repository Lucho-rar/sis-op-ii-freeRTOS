#include "receiver_task.h"

void vTaskReceiverDataSensor(void *pvParameters){
    (void) pvParameters;

    int values[MAX_VALUES_AVERAGE] = {0};
    int value;
    int filter ;
    int N ;
    char buffer[DEFAULT_SIZE_BUFFERS];
    N = get_N_value();
    vIntToString(N, buffer);
    sendUART("\n[INFO] | TaskReceiverDataSensor | N initial: ");
    sendUART(buffer);

    for ( ; ; ){
        xQueueReceive(xSensorQueue, &value, portMAX_DELAY);
        // Desplazo los valores hacia atrás en el buffer
        for (int i = MAX_VALUES_AVERAGE - 1; i > 0; i--) {
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
#if SENSOR_10HZ
    xQueueSend(xDisplayQueue, &filter, portMAX_DELAY ); 
#else
    vIntToString(N, buffer);
    sendUART("\n[INFO] | TaskReceiverDataSensor | Value filtered: ");
    sendUART(buffer);
    xQueueSend(xDisplayQueue, &filter, portMAX_DELAY ); 
#endif
    }



}

