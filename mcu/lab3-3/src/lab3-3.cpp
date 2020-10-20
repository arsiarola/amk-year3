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
#include <ctype.h>

#include "FreeRTOS.h"
#include "task.h"
#include "DigitalIoPin.h"
#include "Fmutex.h"
#include "semphr.h"
#include "ITM_write.h"
#include "heap_lock_monitor.h"


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
QueueHandle_t queue;

struct debugEvent {
 const char *format;
 uint32_t data[3];
};

void debug(const char *format, uint32_t d0, uint32_t d1, uint32_t d2) {
	debugEvent e;
	e.format = format;
	e.data[0] = d0;
	e.data[1] = d1;
	e.data[2] = d2;
	xQueueSendToBack(
					 queue,
					 &e,
					 portMAX_DELAY);
}


static void vPrint(void *pvParameters) {
	debugEvent e;
	char buffer[64];
	while (1) {
		xQueueReceive(queue, &e, portMAX_DELAY);
		snprintf(buffer, 64, e.format, e.data[0], e.data[1], e.data[2]);
		ITM_write(buffer);
	}
}

static void vReadTask(void *pvParameters) {
	int c;
	uint32_t count = 0;
	while(1) {
		c = guard.readChar();
		if (c == EOF) {
			vTaskDelay(3);
			continue;
		}

		if (c == '\n' || c == '\r') guard.print("\r\n");
		else 						guard.print("%c", c);

		if (!isspace(c)) {
			++count;
		}

		if (count > 0 && (isspace(c))) {
			debug("Word length: %u, Timestamp: %d\n\r",
					count,
					xTaskGetTickCount(),
					0);
			count = 0;
		}
	}
}

static DigitalIoPin SW1(0, 17, DigitalIoPin::pullup, true);
static void vButtonTask(void *pvParameters) {
	uint32_t tickCount = 0; // one tick is approx in ms
	while (1) {
		if (SW1.read()) {
			tickCount = xTaskGetTickCount();
			while (SW1.read());
			debug("Milliseconds held: %d\r\n",
					xTaskGetTickCount() - tickCount,
					0,
					0);
		}
		vTaskDelay(3);
	}
}

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
    //ITM_init();
	prvSetupHardware();

	queue = xQueueCreate(10, sizeof(debugEvent));

	xTaskCreate(vPrint, "vTask1",
	configMINIMAL_STACK_SIZE + 256, NULL, (tskIDLE_PRIORITY + 1UL),
			(TaskHandle_t *) NULL);

	xTaskCreate(vReadTask, "vTask2",
	configMINIMAL_STACK_SIZE + 256, NULL, (tskIDLE_PRIORITY + 2UL),
			(TaskHandle_t *) NULL);

	xTaskCreate(vButtonTask, "vTask3",
	configMINIMAL_STACK_SIZE + 256, NULL, (tskIDLE_PRIORITY + 2UL),
			(TaskHandle_t *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}

