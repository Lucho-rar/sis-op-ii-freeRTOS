#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <stdbool.h>
#include "utils.h"
/* ################ defines del demo #################### */
/* Delay between cycles of the 'check' task. */
#define mainCHECK_DELAY            ( ( TickType_t ) 5000 / portTICK_PERIOD_MS )
#define mainBAUD_RATE              ( 19200 )
#define mainFIFO_SET               ( 0x10 )
/* Demo task priorities. */
#define mainQUEUE_POLL_PRIORITY    ( tskIDLE_PRIORITY + 2 )
#define mainCHECK_TASK_PRIORITY    ( tskIDLE_PRIORITY + 3 )
#define mainSEM_TEST_PRIORITY      ( tskIDLE_PRIORITY + 1 )
#define mainBLOCK_Q_PRIORITY       ( tskIDLE_PRIORITY + 2 )

/* Misc. */
#define mainQUEUE_SIZE             ( 3 )
#define mainDEBOUNCE_DELAY         ( ( TickType_t ) 150 / portTICK_PERIOD_MS )
#define mainNO_DELAY               ( ( TickType_t ) 0 )

void boardInit(void);
void prvSetupHardware( void );


#endif /* BOARD_H */