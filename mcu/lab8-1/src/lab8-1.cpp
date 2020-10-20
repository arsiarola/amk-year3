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

#include "FreeRTOS.h"
#include "semphr.h"
#include "ITM_write.h"
#include "DigitalIoPin.h"
#include <stdlib.h>     /* abs */

#define RIGHT_DIR false
#define LEFT_DIR true


static QueueHandle_t queue;

static void prvSetupHardware(void) {
    SystemCoreClockUpdate();
    Board_Init();
    /* Initial LED0 state is off */
    Board_LED_Set(0, false);
}

static void handleButton(int isrNum, int buttonNum) {
    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(isrNum));
    portBASE_TYPE xHigherPriorityWoken = pdFALSE;
    xQueueSendToBack(queue, &buttonNum, 0);
    portEND_SWITCHING_ISR(xHigherPriorityWoken);
}

extern "C" {
    void PIN_INT0_IRQHandler(void) {
        ITM_print("sw1\n");
        handleButton(0, 1);
    }
    void PIN_INT1_IRQHandler(void) {
        ITM_print("sw2\n");
        handleButton(1, 2);
    }
    void PIN_INT2_IRQHandler(void) {
        ITM_print("sw3\n");
        handleButton(2, 3);
    }
}
static void task1(void *pvParameters) {
	int num;
    int lastNum;
	int count = -1;
	while (1) {
		if (xQueueReceive(
					queue,
					&( num ),
					portMAX_DELAY) == pdPASS) {
            if (count < 0) count = 0;
            else if (num != lastNum) {
                ITM_print("Button %d pressed %d times.\n", lastNum, count);
                count = 0;
            }
            lastNum = num;
            ++count;
		}
	}
}


int main(void) {
    prvSetupHardware();
    ITM_init();
    queue = xQueueCreate(10, sizeof(int));

    Chip_PININT_Init(LPC_GPIO_PIN_INT);
    Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_PININT);
    Chip_SYSCTL_PeriphReset(RESET_PININT);

    // sw1
        Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 17,
            	IOCON_DIGMODE_EN | IOCON_MODE_INACT | IOCON_MODE_PULLUP);
    	Chip_INMUX_PinIntSel(0, 0, 17);
    	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(0));
    	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(0));
    	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(0));
    	NVIC_ClearPendingIRQ(PIN_INT0_IRQn);
    	NVIC_EnableIRQ(PIN_INT0_IRQn);

    	// sw2
        Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 11,
        	IOCON_DIGMODE_EN | IOCON_MODE_INACT | IOCON_MODE_PULLUP);
    	Chip_INMUX_PinIntSel(1, 1, 11);
    	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(1));
    	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(1));
    	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(1));
    	NVIC_ClearPendingIRQ(PIN_INT1_IRQn);
    	NVIC_EnableIRQ(PIN_INT1_IRQn);

    	// sw3
    	Chip_INMUX_PinIntSel(2, 1, 9);
        Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 9,
        	IOCON_DIGMODE_EN | IOCON_MODE_INACT | IOCON_MODE_PULLUP);
    	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(2));
    	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(2));
    	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(2));
    	NVIC_ClearPendingIRQ(PIN_INT2_IRQn);
    	NVIC_EnableIRQ(PIN_INT2_IRQn);


    xTaskCreate(task1, "task1", configMINIMAL_STACK_SIZE + 1000, NULL, (tskIDLE_PRIORITY + 2UL),(TaskHandle_t *) NULL);
    /* Start the scheduler */
    vTaskStartScheduler();
    return 1;
}
