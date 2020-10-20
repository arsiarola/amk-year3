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
#include "Syslog.h"

#define CMD_SIZE 10
#define QUEUE_LENGTH 2


static TimerHandle_t oneShotTimer;
static TimerHandle_t autoReloadTimer;

static TaskHandle_t vTask1Handle;
static TaskHandle_t vTask2Handle;

static SemaphoreHandle_t binary;
static QueueHandle_t queue;

Syslog uart = Syslog();

static void prvSetupHardware(void) {
    SystemCoreClockUpdate();
    Board_Init();
    /* Initial LED0 state is off */
    Board_LED_Set(0, false);
}

extern "C"
{

	void vConfigureTimerForRunTimeStats(void)
	{
		Chip_SCT_Init(LPC_SCTSMALL1);
		LPC_SCTSMALL1->CONFIG = SCT_CONFIG_32BIT_COUNTER;
		LPC_SCTSMALL1->CTRL_U = SCT_CTRL_PRE_L(255) | SCT_CTRL_CLRCTR_L; // set prescaler to 256 (255 + 1), and start timer
	}

}

int main(void) {
    prvSetupHardware();
    ITM_init();
    ITM_print("starting\n");
    queue = xQueueCreate(QUEUE_LENGTH, sizeof(char[CMD_SIZE]) );
    binary = xSemaphoreCreateBinary();
    oneShotTimer = xTimerCreate(
    				"one-shot",
                      20'000,
                      pdFALSE,
                      (void *) 0,
					  [] (TimerHandle_t xTimer) {
        					xSemaphoreGive(binary);
    				  }
    );

    autoReloadTimer = xTimerCreate (
    					  "auto-reload",
                          5'000,
                          pdTRUE,
                          (void *) 0,
						  [](TimerHandle_t xTimer) {
    							xQueueSendToBack(queue, "hello", portMAX_DELAY);
    					  }
    );

    xTimerStart(autoReloadTimer, 0);
    xTimerStart(oneShotTimer, 0);

    xTaskCreate([](void *pvParameters) {
    	char buffer[CMD_SIZE];
    	while (1) {
    			xQueueReceive(queue, &buffer, portMAX_DELAY);
    			uart.print("%s\n\r", buffer);

    	}
    }
    , "task1", configMINIMAL_STACK_SIZE * 4, NULL, (tskIDLE_PRIORITY + 1UL), &vTask1Handle);

    xTaskCreate([](void *pvParameters) {
    	while (1) {
    		if (xSemaphoreTake(binary, portMAX_DELAY) == pdPASS) {
    			xQueueSendToBack(queue, "aargh", portMAX_DELAY);
    		}
    	}
    },
	"task2", configMINIMAL_STACK_SIZE * 4, NULL, (tskIDLE_PRIORITY + 1UL), &vTask2Handle);


    /* Start the scheduler */
    vTaskStartScheduler();
    return 1;
}
