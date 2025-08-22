#include "utils.h"
SemaphoreHandle_t xButtonSemaphore;
SemaphoreHandle_t xMutexN;
QueueHandle_t xPrintQueue;
QueueHandle_t xSensorQueue;
QueueHandle_t xUpdateNQueue;
QueueHandle_t xDisplayQueue;

int valor_ventana = 1;

void sendUART0(const char *string) {
	while (*string != '\0') {
		UARTCharPut(UART0_BASE, *string);
		string++;
	}
}

void vIntToString(int num, char *str) {
    int i = 0;
    int is_negative = 0;

    // Manejar el caso de número negativo
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    // Convertir cada dígito del número en un carácter
    do {
        str[i++] = (num % 10) + '0'; // Convertir el dígito a su carácter ASCII correspondiente
        num /= 10;
    } while (num > 0);

    // Agregar el signo negativo si es necesario
    if (is_negative) {
        str[i++] = '-';
    }

    // Terminar la cadena
    str[i] = '\0';

    // Invertir la cadena porque los dígitos fueron agregados en orden inverso
    int start = 0;
    int end = i - 1;
    char temp;
    while (start < end) {
        temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}


const char* getStateName(eTaskState state) {
    switch (state) {
        case eRunning: return "Running";
        case eReady: return "Ready";
        case eBlocked: return "Blocked";
        case eSuspended: return "Suspended";
        case eDeleted: return "Deleted";
        default: return "Unknown";
    }
}