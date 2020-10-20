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
static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();
	Board_Init();

	/* Initial LED0 state is off */
	Board_LED_Set(0, false);
}

#define DOT 1
#define DASH 3

#define SYMBOL  1
#define LETTER  3
#define WORD    7

bool GreenLed = true;
/* LED1 toggle thread */
static void vReadTask(void *pvParameters) {
    int count = 0;
    int onDelay = DOT; // true = DASH, false = DOT
    int offDelay = SYMBOL;

	while (1) {

        switch (count) {
            case 9:
                onDelay = 0;
                offDelay = WORD;
                count = -1;
                break;
            case 6:
                onDelay = DOT;
                offDelay = LETTER;
                break;
            case 3:
                onDelay = DASH;
                offDelay = LETTER;
                break;
            case 0:
                onDelay = DOT;
                offDelay = 0;
                GreenLed = !GreenLed;
                break;
            default:
                offDelay = SYMBOL;
                break;
        }
        /* DEBUGOUT("OnDelay = %d, OffDelay = %d, count = %d\r\n", onDelay, offDelay, count); */
		Board_LED_Set(0, false);
		vTaskDelay(offDelay * (configTICK_RATE_HZ/3));
		Board_LED_Set(0, (bool) onDelay);
		vTaskDelay(onDelay * (configTICK_RATE_HZ/3));
        ++count;
	}
}

/* LED2 toggle thread */
static void vSW1Task(void *pvParameters) {

	while (1) {
		Board_LED_Set(1, GreenLed);
	}
}


static DigitalIoPin SW1(0, 17, DigitalIoPin::pullup, false);
/* UART (or output) thread */
static void vReceiveTask(void *pvParameters) {
	int tickCnt = 50;

	while (1) {
		DEBUGOUT("Time: %02d:%02d \r\n", tickCnt/60, tickCnt%60);
        if (SW1.read()) {
            vTaskDelay(configTICK_RATE_HZ);
        }
        else {
            vTaskDelay(configTICK_RATE_HZ/10);
        }

        tickCnt++;
	}
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* the following is required if runtime statistics are to be collected */
extern "C" {

void vConfigureTimerForRunTimeStats( void ) {
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
int main(void)
{
	prvSetupHardware();

	/* LED1 toggle thread */
	xTaskCreate(vReadTask, "vRead",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(TaskHandle_t *) NULL);

	/* LED2 toggle thread */
	xTaskCreate(vSW1Task, "vSW1",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(TaskHandle_t *) NULL);

	/* UART output thread, simply counts seconds */
	xTaskCreate(vReceiveTask, "vReceive",
				configMINIMAL_STACK_SIZE + 128, NULL, (tskIDLE_PRIORITY + 1UL),
				(TaskHandle_t *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}

