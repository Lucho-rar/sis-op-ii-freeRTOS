
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

    // Task create
    xTaskCreate(vTaskSensor, "Sensor", 80, NULL, PRIORITY_HIGH, NULL);
    xTaskCreate(vTaskReceiverDataSensor, "Receiver", 160, NULL, PRIORITY_HIGH + 1, NULL);
    xTaskCreate(vTaskDisplay, "Display", 100, NULL, PRIORITY_MEDIUM, NULL);
    xTaskCreate(vTaskUpdateN, "UpdateN", 120, NULL, PRIORITY_MEDIUM + 1, NULL);
    xTaskCreate(vTaskMonitor, "Monitor", 80, NULL, PRIORITY_LOW, NULL);

    // Queue
    xSensorQueue = xQueueCreate(50, sizeof(unsigned long));
    xDisplayQueue = xQueueCreate(50, sizeof(unsigned long));
    xUpdateNQueue = xQueueCreate(50, sizeof(unsigned long));

    // Start scheduler
    vTaskStartScheduler();

    return 0;
}
