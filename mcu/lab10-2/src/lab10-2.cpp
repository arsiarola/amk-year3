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

#include <stdio.h>
#include <string.h>
#include <cr_section_macros.h>
#include <cstdlib>
#include <ctime>

#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"
#include "ITM_write.h"
#include "DigitalIoPin.h"

static TimerHandle_t oneShotTimer;

static TaskHandle_t vTask1Handle;

static DigitalIoPin sw1(0, 17, DigitalIoPin::pullup, true);
static DigitalIoPin sw2(1, 11, DigitalIoPin::pullup, true);
static DigitalIoPin sw3(1, 9, DigitalIoPin::pullup, true);

static void prvSetupHardware(void) {
    SystemCoreClockUpdate();
    Board_Init();
    /* Initial LED0 state is off */
    Board_LED_Set(0, false);
}

int main(void) {
    prvSetupHardware();
    ITM_init();
    ITM_print("starting\n");
    oneShotTimer = xTimerCreate(
    				"one-shot",
                      5'000,
                      pdFALSE,
                      (void *) 0,
					  [] (TimerHandle_t xTimer) {
    					Board_LED_Set(1, false);
    					ITM_print("Reset the timer\n");
    				  }
    );

    xTaskCreate([](void *pvParameters) {
    	while (1) {
    		if (sw1.read() || sw2.read() || sw3.read()) {
    			Board_LED_Set(1, true);
    			xTimerReset(oneShotTimer, 100); // not really sure what kind of value we want for the BlockTime
    			while (sw1.read() || sw2.read() || sw3.read());
    		}
    	}
    }
    , "task1", configMINIMAL_STACK_SIZE * 4, NULL, (tskIDLE_PRIORITY + 1UL), &vTask1Handle);


    /* Start the scheduler */
    vTaskStartScheduler();
    return 1;
}
