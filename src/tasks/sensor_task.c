#include "sensor_task.h"

void vTaskSensor(void* pvParameters)
{
    (void)pvParameters;
    // Simulate a function that reaches 20 and returns to zero, so the "sensor" measurement varies
    int temperature_v = MIN_TEMPERATURE;

    // Enable 2 modes to see it at 10 hertz or slower with prints
    TickType_t xLastWakeTime = xTaskGetTickCount();
#if SENSOR_10HZ
    const TickType_t xFrequency = pdMS_TO_TICKS(100);
#else
    const TickType_t xFrequency = pdMS_TO_TICKS(3000);
#endif

    for (;;)
    {
        if (temperature_v < MAX_TEMPERATURE)
        {
            temperature_v++;
        }
        else
        {
            temperature_v = MIN_TEMPERATURE;
        }
        xQueueSend(xSensorQueue, &temperature_v, portMAX_DELAY); // Send it to the filter

#if SENSOR_10HZ
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
#else
        char buffer[DEFAULT_SIZE_BUFFERS];
        vIntToString(temperature_v, buffer); // Convert integer to string
        sendUART("\n[INFO] | TaskSensor | Temperature: ");
        sendUART(buffer);
        sendUART("ºC");
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
#endif
    }
}