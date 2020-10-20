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

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

#include "FreeRTOS.h"
#include "task.h"
#include "DigitalIoPin.h"
#include <mutex>
#include "Bsemaphore.h"
#include "semphr.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

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

SemaphoreHandle_t xBinarySemaphore;
static BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
static void vReadTask(void *pvParameters) {
	int c;
	while (1) {
		c = Board_UARTGetChar();
		if (c != EOF) {
			if (c == '\n')Board_UARTPutChar('\r'); // precede linefeed with carriage return
			Board_UARTPutChar(c);
			if (c == '\r')Board_UARTPutChar('\n'); // send line feed after carriage return
			xSemaphoreGive(xBinarySemaphore);
		}
	}
}

static void vBlinkTask(void *pvParameters) {
	while (1) {
		if(xSemaphoreTake(xBinarySemaphore, portMAX_DELAY) == pdTRUE) {
			Board_LED_Set(0, true);
			vTaskDelay(configTICK_RATE_HZ / 10);
			Board_LED_Set(0, false);
			vTaskDelay(configTICK_RATE_HZ / 10);
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

	xBinarySemaphore = xSemaphoreCreateBinary();

	/* LED1 toggle thread */
	xTaskCreate(vReadTask, "vReadSerial",
	configMINIMAL_STACK_SIZE + 128, NULL, (tskIDLE_PRIORITY + 1UL),
			(TaskHandle_t *) NULL);

	/* LED2 toggle thread */
	xTaskCreate(vBlinkTask, "vBlinker",
	configMINIMAL_STACK_SIZE + 128, NULL, (tskIDLE_PRIORITY + 1UL),
			(TaskHandle_t *) NULL);

	xSemaphoreGive(xBinarySemaphore);

	/* Start the scheduler */
	vTaskStartScheduler();


	/* Should never arrive here */
	return 1;
}

