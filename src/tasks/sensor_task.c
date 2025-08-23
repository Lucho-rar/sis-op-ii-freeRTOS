#include "sensor_task.h"

void vTaskSensor(void *pvParameters){
    (void) pvParameters;
    // simulo una funcion que llegue a 20 y vuelva a cero, para que sea variable la "medicion" del sensor
    int temperature_v = MIN_TEMPERATURE;         
    char buffer[DEFAULT_SIZE_BUFFERS];

    //Habilito 2 modos para poder verlo a 10 hertz o mas lento con los prints
    TickType_t xLastWakeTime = xTaskGetTickCount();
#if SENSOR_10HZ
    const TickType_t xFrequency = pdMS_TO_TICKS(100);
#else
    const TickType_t xFrequency = pdMS_TO_TICKS(3000); 
#endif

    for ( ; ; ){
        if (temperature_v < MAX_TEMPERATURE){
            temperature_v ++ ; 
        }else{
            temperature_v = MIN_TEMPERATURE;
        }
        xQueueSend(xSensorQueue, &temperature_v, portMAX_DELAY); // Se lo envio al filtro

#if SENSOR_10HZ
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
#else
    vIntToString(temperature_v, buffer); // Convertir el entero a cadena
    sendUART("\n[INFO] | TaskSensor | Temperature: ");
    sendUART(buffer);
    sendUART("ºC");
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
#endif
        
    }
}