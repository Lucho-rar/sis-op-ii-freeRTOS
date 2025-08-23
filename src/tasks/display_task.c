#include "display_task.h"
#include "DriverLib.h"

void vTaskDisplay(void *pvParameters) {
    (void) pvParameters;

    int value_filter;
    static int buffer_onda[96] = {0};
    int i;
    char buffer[DEFAULT_SIZE_BUFFERS];

    OSRAMStringDraw( "FreeRTOS - SO2", 0, 0 );
    vTaskDelay(500);
    OSRAMClear();
    vTaskDelay(500);

    OSRAMStringDraw( "FreeRTOS - SO2", 0, 0 );
    vTaskDelay(500);
    OSRAMClear();
    vTaskDelay(500);
    
    OSRAMStringDraw( "FreeRTOS - SO2", 0, 0 );
    vTaskDelay(500);
    OSRAMClear();
    vTaskDelay(500);

    OSRAMStringDraw( "FreeRTOS - SO2", 0, 0 );
    vTaskDelay(2000);
    OSRAMClear();

    for (;;) {
        xQueueReceive(xDisplayQueue, &value_filter, portMAX_DELAY);

        // Desplazar el buffer y agregar el nuevo valor
        for (i = 95; i > 0; i--) {
            buffer_onda[i] = buffer_onda[i-1];
        }
        buffer_onda[0] = value_filter;

        OSRAMClear();

        // Normalización continua para todo el rango
        for (i = 0; i < 96; i++) {
            int input = buffer_onda[i];

            // Clamp input to range
            if (input < DISPLAY_MIN_INPUT) input = DISPLAY_MIN_INPUT;
            if (input > DISPLAY_MAX_INPUT) input = DISPLAY_MAX_INPUT;

            // Mapea todo el rango de entrada a 2*BITMAP_HALF_SIZE
            int total_steps = 2 * BITMAP_HALF_SIZE;
            int mapped_continuous = (input - DISPLAY_MIN_INPUT) * total_steps / (DISPLAY_MAX_INPUT - DISPLAY_MIN_INPUT);

            int bit_map_half, mapped;
            if (mapped_continuous < BITMAP_HALF_SIZE) {
                bit_map_half = 1; // mitad inferior
                mapped = mapped_continuous; // 0-6
            } else {
                bit_map_half = 0; // mitad superior
                mapped = mapped_continuous - BITMAP_HALF_SIZE; // 0-6
            }

            OSRAMImageDraw((const unsigned char *) bitMapping(mapped), i, bit_map_half, 1, 1);
        }

#if !SENSOR_10HZ 
        vIntToString(value_filter, buffer);
        sendUART("\n[INFO] | TaskDisplay | Filtered value: ");
        sendUART(buffer);
#endif
    }
}

char * bitMapping(int valor) {
    switch(valor) {
        case 0: return "\100"; // 01000000
        case 1: return "\020"; // 00010000
        case 2: return "\010"; // 00001000
        case 3: return "\004"; // 00000100
        case 4: return "\002"; // 00000010
        case 5: return "\001"; // 00000001
        case 6: return "\001"; // última fila también encendida
        default: return " ";
    }
}

int mapToBitMapping(int value) {
    if (value < DISPLAY_MIN_INPUT) value = DISPLAY_MIN_INPUT;
    if (value > DISPLAY_MAX_INPUT) value = DISPLAY_MAX_INPUT;
    return (value * BITMAP_HALF_SIZE) / DISPLAY_MAX_INPUT; // 0-6
}
