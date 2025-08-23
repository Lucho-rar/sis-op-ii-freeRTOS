# TP4 - SO2

## Descripción general
Firmware para **LM3S811(ARM CORTEX M3)**, utilizando FreeRTOS para su desarrollo y QEMU para su simulación.

![image](https://hackmd.io/_uploads/S1Q8U3DFle.png)

## Objetivo
El firmware debe contemplar las siguientes tareas y requerimientos:
- Simular n sensor de temperatura que tome valores a 10 Hz.
- Receptor de los datos del sensor, que los filtre con un filtro de ventana deslizante. Su valor "N" que define la ventana, debe ser recibido mediante UART.
- Mostrar en el display la señal a la salida del filtro.
- Monitor para observar el estado de todas las tareas. 

## Desarrollo

### Tools
- FreeRTOS: Sistema operativo en tiempo real ampliamente usado en microcontroladores y pequeños microprocesadores.
- QEMU: Emulador que permite virtualizar programas compilados en un entorno diferente a la principal.

### Colas de mensajes

### Tareas 

### vTaskSensor
Esta tarea simula el sensor cada 10 hertz. Para hacer una señal distinguible en el display, usa un contador de una temperatura mínima a máxima definible por parámetros. 

Para el tiempo usamos vTaskDelayUntil() con 100 mS. Además se agregan definiciones que facilitan el debug y la presentación (como delays más bajos y asi observar prints y detalles lentamente). 

Una vez "simulado" el valor del sensor, se envía a la tarea del receptor mediante la xSensorQueue.

```c
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
```