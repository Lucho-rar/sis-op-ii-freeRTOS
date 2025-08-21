#include "display_task.h"
#include "DriverLib.h"


static char buffer[50];
void vTaskDisplay(void *pvParameters) {
    int value_filter;
    static int buffer_onda[96] = {0};
    int i;


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

            // Mapea todo el rango de entrada a 2*BITMAP_HALF_SIZE (mitad inferior y superior)
            int total_steps = 2 * BITMAP_HALF_SIZE;
            int mapped_continuous = (input - DISPLAY_MIN_INPUT) * total_steps / (DISPLAY_MAX_INPUT - DISPLAY_MIN_INPUT);

            int bit_map_half, mapped;
            if (mapped_continuous < BITMAP_HALF_SIZE) {
                bit_map_half = 1; // mitad inferior
                mapped = BITMAP_MIN + mapped_continuous;
            } else {
                bit_map_half = 0; // mitad superior
                mapped = BITMAP_MIN + (mapped_continuous - BITMAP_HALF_SIZE);
            }

            OSRAMImageDraw(bitMapping(mapped), i, bit_map_half, 1, 1);
        }

        vIntToString(value_filter, buffer);
        sendUART0("\n[INFO] | TaskDisplay | Filtered value: ");
        sendUART0(buffer);
    }
}

char * bitMapping(int valor) {
    if ((valor <= 13) || (valor == 20)) {
        return "\100"; // 01000000
    }
    if ((valor == 14) || (valor == 21)) {
        return " ";
    }
    if ((valor == 15) || (valor == 22)) {
        return "\020"; // 00010000
    }
    if ((valor == 16) || (valor == 23)){
        return "\010"; // 00001000
    }
    if ((valor == 17) || (valor == 24)){
        return "\004"; // 00000100
    }
    if ((valor == 18) || (valor == 25)) {
        return "\002"; // 00000010
    }
    if ((valor == 19) || (valor == 26)) {
        return "\001"; // 00000001
    }
    return " "; // Valor fuera de rango
}

int mapToBitMapping(int value) {
    if (value < 0) value = 0;
    if (value > 20) value = 20;
    // Mapea 0 -> 13, 20 -> 19 
    return 13 + ((value * 6) / 20); // 6 = (19-13)
}
