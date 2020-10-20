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
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <string.h>

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

#include "FreeRTOS.h"
#include "task.h"
#include "DigitalIoPin.h"
#include "Fmutex.h"
#include "semphr.h"


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

static void vTask1(void *pvParameters) {
	int num;
	while (1) {
		num = rand() % 400 + 100;
		if (xQueueSendToBack(
							 queue,
							 &num,
							 portMAX_DELAY) != pdPASS) {
			guard.print("error in sending the random number from read task\r\n");
		}
		vTaskDelay(num);

	}
}

static DigitalIoPin SW1(0, 17, DigitalIoPin::pullup, true);
static const int error = 112;
static void vButtonTask(void *pvParameters) {
	while (1) {
		if(SW1.read()) {
			if (xQueueSendToFront(
								queue,
								& ( error ),
								portMAX_DELAY)
								!= pdPASS) {
				guard.print("error in sending the button press (%d)\n\r", error);
			}
			while (SW1.read());
		}
	}
}



static void vTask3(void *pvParameters) {
	int total = 0;
	int num;
	while (1) {
		if( xQueueReceive(
						  queue,
						  &( num ),
						  portMAX_DELAY ) == pdPASS ) {

			guard.print("%d ", num);
			if (num == error) {
				// call the send
				guard.print("Help me");
				vTaskDelay(configTICK_RATE_HZ * 0.8);
			}
			guard.print("\n\r");
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
	srand (time(NULL));
	prvSetupHardware();

	semaphore = xSemaphoreCreateCounting(TASK_SIZE, 0);
	queue = xQueueCreate(10, sizeof(int));

	xTaskCreate(vTask1, "vTask1",
	configMINIMAL_STACK_SIZE + 256, NULL, (tskIDLE_PRIORITY + 1UL),
			(TaskHandle_t *) NULL);

	xTaskCreate(vButtonTask, "vTask2",
	configMINIMAL_STACK_SIZE + 128, NULL, (tskIDLE_PRIORITY + 1UL),
			(TaskHandle_t *) NULL);

	xTaskCreate(vTask3, "vTask3",
	configMINIMAL_STACK_SIZE + 256, NULL, (tskIDLE_PRIORITY + 1UL),
			(TaskHandle_t *) NULL);

	xSemaphoreGive(semaphore);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}

