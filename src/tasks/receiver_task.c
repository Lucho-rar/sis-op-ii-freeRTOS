#include "receiver_task.h"

void vTaskReceiverDataSensor(void *pvParameters){
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


 int get_N_value(){

    return valor_ventana;

}