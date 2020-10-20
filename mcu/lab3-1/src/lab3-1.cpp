/*
 ===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
 ===============================================================================
 */

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>
#include <stdio.h>
#include <stdlib.h>     /* srand, rand */
#include <string.h>

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

#include "FreeRTOS.h"
#include "task.h"
#include "DigitalIoPin.h"
#include "Fmutex.h"
#include "semphr.h"
#include <time.h>       /* time */

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#define TASK_SIZE 10
#define LIST_SIZE 139
/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Sets up system hardware */
static void prvSetupHardware(void) {
	SystemCoreClockUpdate();
	Board_Init();

	/* Initial LED0 state is off */
	Board_LED_Set(0, false);
}

Fmutex guard;
SemaphoreHandle_t semaphore;
QueueHandle_t queue;

static void vReadTask(void *pvParameters) {
	int c;
	int count = 0;
	while (1) {
		c = Board_UARTGetChar();
		if (c == '\n' || c == '\r') {
			guard.print("\n\r");
			if (xQueueSendToBack(
					             queue,
					             &count,
					             ( TickType_t ) 10) != pdPASS) {
				guard.print("error in sending the count from read task\r\n");
			}
			count = 0;
		}

		else if (c != EOF) {
			guard.print(c);
			++count;
		}
	}
}

static DigitalIoPin SW1(0, 17, DigitalIoPin::pullup, true);
static void vButtonTask(void *pvParameters) {
	const int error = -1;
	while (1) {
		if(SW1.read()) {
			if (xQueueSendToBack(
								queue,
								& ( error ),
								( TickType_t ) 10)
								!= pdPASS) {
				guard.print("error in sending the button press (%d)\n\r", error);
			}
			while (SW1.read());
		}
	}
}



static void vReplyTask(void *pvParameters) {
	int total = 0;
	int num;
	while (1) {
		if( xQueueReceive(
						  queue,
						  &( num ),
						  ( TickType_t ) 10 ) == pdPASS ) {
			if (num == -1) {
				// call the send
				guard.print("You have typed %d characters\n\r", total);
				total = 0;
			}
			else {
				guard.print("received number %d\r\n", num);
				total += num;
			}
		}
	}
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* the following is required if runtime statistics are to be collected */
extern "C" {

void vConfigureTimerForRunTimeStats(void) {
	Chip_SCT_Init(LPC_SCTSMALL1);
	LPC_SCTSMALL1->CONFIG = SCT_CONFIG_32BIT_COUNTER;
	LPC_SCTSMALL1->CTRL_U = SCT_CTRL_PRE_L(255) | SCT_CTRL_CLRCTR_L; // set prescaler to 256 (255 + 1), and start timer
}

}
/* end runtime statictics collection */

/**
 * @brief	main routine for FreeRTOS blinky example
 * @return	Nothing, function should not exit
 */
int main(void) {
	prvSetupHardware();

	queue = xQueueCreate(10, sizeof(int));

	xTaskCreate(vReadTask, "vReadSerial",
	configMINIMAL_STACK_SIZE + 256, NULL, (tskIDLE_PRIORITY + 1UL),
			(TaskHandle_t *) NULL);

	xTaskCreate(vButtonTask, "vReadButton",
	configMINIMAL_STACK_SIZE + 128, NULL, (tskIDLE_PRIORITY + 1UL),
			(TaskHandle_t *) NULL);

	xTaskCreate(vReplyTask, "vReply",
	configMINIMAL_STACK_SIZE + 256, NULL, (tskIDLE_PRIORITY + 1UL),
			(TaskHandle_t *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}

