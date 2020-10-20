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
#include "DigitalIoPin.h"
#include "semphr.h"

extern "C" {
void vConfigureTimerForRunTimeStats(void) {
	Chip_SCT_Init(LPC_SCTSMALL1);
	LPC_SCTSMALL1->CONFIG = SCT_CONFIG_32BIT_COUNTER;
	LPC_SCTSMALL1->CTRL_U = SCT_CTRL_PRE_L(255) | SCT_CTRL_CLRCTR_L; // set prescaler to 256 (255 + 1), and start timer
}
}

static void prvSetupHardware(void) {
	SystemCoreClockUpdate();
	Board_Init();

	/* Initial LED0 state is off */
	Board_LED_Set(0, false);
}

static SemaphoreHandle_t binary;

static void task1(void *pvParameters) {
	DigitalIoPin lim1(0, 9, DigitalIoPin::pullup, true);
	DigitalIoPin lim2(0, 29, DigitalIoPin::pullup, true);

	while (lim1.read() && lim2.read()) {
		Board_LED_Toggle(2);
		vTaskDelay(100);
	}
    Board_LED_Set(2, false);
	xSemaphoreGive(binary);
	while (1) {
		Board_LED_Set(0, lim1.read());
		Board_LED_Set(1, lim2.read());
	}
}

static void task2(void *pvParameters) {
	DigitalIoPin lim1(0, 9, DigitalIoPin::pullup, true);
	DigitalIoPin lim2(0, 29, DigitalIoPin::pullup, true);
	DigitalIoPin button1(0, 8, DigitalIoPin::pullup, true);
	DigitalIoPin button3(1, 8, DigitalIoPin::pullup, true);

	DigitalIoPin dir(1, 0, DigitalIoPin::output, true);
	DigitalIoPin step(0, 24, DigitalIoPin::output, true);

	step.write(false);
	xSemaphoreTake(binary, portMAX_DELAY);
	while (1) {
		while (lim1.read() && lim2.read()) {
			vTaskDelay(5000);
		}
        if (dir.read() && lim1.read()) {
            dir.write(false);
        }
        else if (!dir.read() && lim2.read()) {
            dir.write(true);
        }
        step.write(true);
        vTaskDelay(1);
        step.write(false);
	}
}

int main(void) {
	prvSetupHardware();
	binary = xSemaphoreCreateBinary();
	xTaskCreate(task1, "task1", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),(TaskHandle_t *) NULL);

	xTaskCreate(task2,"task2", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),(TaskHandle_t *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}

