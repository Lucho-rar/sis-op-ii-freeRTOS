
#include "tasks/monitor_task.h"
#include "common/utils.h"
#include "tasks/display_task.h"
#include "tasks/sensor_task.h"
#include "tasks/receiver_task.h"
#include "tasks/updater_n_task.h"
#include "DriverLib.h"
#include "common/board.h"



int main( void )
{
    boardInit();

    // creacion de tareas
    xTaskCreate(vTaskSensor, "Sensor", configMINIMAL_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY + 1, NULL);
    xTaskCreate(vTaskReceiverDataSensor, "Receiver", configMINIMAL_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY + 1, NULL);
    xTaskCreate(vTaskDisplay, "Display", configMINIMAL_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY + 1, NULL);
    xTaskCreate(vTaskUpdateN, "UpdateN", configMINIMAL_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY + 1, NULL);
    xTaskCreate(vTaskMonitor, "Monitor", configMINIMAL_STACK_SIZE * 2, NULL, mainCHECK_TASK_PRIORITY + 1, NULL);

    // colas 
    xSensorQueue = xQueueCreate(50, sizeof(unsigned long));
    xDisplayQueue = xQueueCreate(50 , sizeof(unsigned long ));
    xUpdateNQueue = xQueueCreate(50, sizeof(unsigned long));


    /* Start the scheduler. */
    vTaskStartScheduler();

    return 0;
}


