
#include "DriverLib.h"
#include "common/board.h"
#include "common/utils.h"
#include "tasks/display_task.h"
#include "tasks/monitor_task.h"
#include "tasks/receiver_task.h"
#include "tasks/sensor_task.h"
#include "tasks/updater_n_task.h"

int main(void)
{
    // Init board and variables
    boardInit();
    vConfigureTimerForRunTimeStats();
    // Task create
    xTaskCreate(vTaskSensor, "Sensor", STACK_SIZE_SENSE, NULL, PRIORITY_HIGH, NULL);
    xTaskCreate(vTaskReceiverDataSensor, "Receiver", STACK_SIZE_RECEIVER, NULL, PRIORITY_HIGH + 1, NULL);
    xTaskCreate(vTaskDisplay, "Display", STACK_SIZE_DISPLAY, NULL, PRIORITY_MEDIUM, NULL);
    xTaskCreate(vTaskUpdateN, "UpdateN", STACK_SIZE_UPDATE, NULL, PRIORITY_MEDIUM + 1, NULL);
    xTaskCreate(vTaskMonitor, "Monitor", STACK_SIZE_MONITOR, NULL, PRIORITY_LOW, NULL);

    // Queue
    xSensorQueue = xQueueCreate(50, sizeof(unsigned long));
    xDisplayQueue = xQueueCreate(50, sizeof(unsigned long));
    xUpdateNQueue = xQueueCreate(50, sizeof(unsigned long));

    // Start scheduler
    vTaskStartScheduler();

    return 0;
}
