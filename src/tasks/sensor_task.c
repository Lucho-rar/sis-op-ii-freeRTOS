#include "sensor_task.h"

void vTaskSensor(void *pvParameters){
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