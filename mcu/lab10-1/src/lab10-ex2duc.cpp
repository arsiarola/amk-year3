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
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"
#include <cstdio>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "DigitalIoPin.h"
#include "Fmutex.h"


static TimerHandle_t oneShot;
static Fmutex mutex = Fmutex();
static volatile bool ledOn = false;

static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();
	Board_Init();

	/* Initial LED0 state is off */
	Board_LED_Set(0, false);
	Board_LED_Set(1, false);
	Board_LED_Set(2, false);
}

void task1(void *pvParameter)
{
	DigitalIoPin sw1(0, 17, DigitalIoPin::pullup, true);
	DigitalIoPin sw3(1, 9, DigitalIoPin::pullup, true);
	DigitalIoPin sw2(1, 11, DigitalIoPin::pullup, true);

	vTaskDelay(100); // The electrical properties make the pullup of sw2 bit slower than others.

	while (1)
	{
		if (sw1.read() || sw2.read() || sw3.read())
		{
			Board_LED_Set(1, true);
			xTimerReset(oneShot, 0);
			mutex.print("Button pressed, timer resetted\r\n");
			while (sw1.read() || sw2.read() || sw3.read())
				;
		}
	}
}

//static void oneShotCallBack(TimerHandle_t timer)
//{
//	Board_LED_Set(1, false);
//	mutex.print("Time out, turn LED off\r\n");
//}
/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* the following is required if runtime statistics are to be collected */
extern "C"
{

	void vConfigureTimerForRunTimeStats(void)
	{
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
	oneShot = xTimerCreate("one shot", 5000, pdFALSE, nullptr,
			[](TimerHandle_t timer)
			{
				Board_LED_Set(1, false);
				mutex.print("Time out, turn LED off\r\n");
			});

	xTaskCreate(task1, "LED task", configMINIMAL_STACK_SIZE * 4, NULL,
			(tskIDLE_PRIORITY + 1UL), (TaskHandle_t *) nullptr);
	vTaskStartScheduler();
	return 0;
}
