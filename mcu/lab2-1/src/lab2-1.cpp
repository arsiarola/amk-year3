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
#include "Fmutex.h"
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

static Fmutex guard;
static void guarded_write(const char* str) {
    guard.lock();
    Board_UARTPutSTR(str);
    guard.unlock();
}

/* LED1 toggle thread */
static void vSW1Task(void *pvParameters) {
    DigitalIoPin SW1(0, 17, DigitalIoPin::pullup, true);
	while (1) {
        if (SW1.read()) {
            guarded_write("SW1 pressed\n\r");
            while (SW1.read());
        }
	}
}

/* LED2 toggle thread */
static void vSW2Task(void *pvParameters) {
   DigitalIoPin SW2(1, 11, DigitalIoPin::pullup, true);
	while (1) {
        if (SW2.read()) {
            guarded_write("SW2 pressed\n\r");
            while (SW2.read());
        }
	}
}


static void vSW3Task(void *pvParameters) {
    DigitalIoPin SW3(1, 9, DigitalIoPin::pullup, true);
	while (1) {
        if (SW3.read()) {
            guarded_write("SW3 pressed\n\r");
            while (SW3.read());
        }
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
	xTaskCreate(vSW1Task, "vTaskLed1",
				configMINIMAL_STACK_SIZE + 128, NULL, (tskIDLE_PRIORITY + 1UL),
				(TaskHandle_t *) NULL);

	/* LED2 toggle thread */
	xTaskCreate(vSW2Task, "vTaskLed2",
				configMINIMAL_STACK_SIZE + 128, NULL, (tskIDLE_PRIORITY + 1UL),
				(TaskHandle_t *) NULL);

	/* UART output thread, simply counts seconds */
	xTaskCreate(vSW3Task, "vTaskLed3",
				configMINIMAL_STACK_SIZE + 128, NULL, (tskIDLE_PRIORITY + 1UL),
				(TaskHandle_t *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}

