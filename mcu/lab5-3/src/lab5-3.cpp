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
#include "ITM_write.h"
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
	DigitalIoPin button1(0, 8, DigitalIoPin::pullup, true);
	DigitalIoPin button3(1, 8, DigitalIoPin::pullup, true);

	DigitalIoPin dir(1, 0, DigitalIoPin::output, true);
	DigitalIoPin step(0, 24, DigitalIoPin::output, true);

	while (lim1.read() || lim2.read()) {
		Board_LED_Toggle(2);
		vTaskDelay(100);
	}
    Board_LED_Set(2, false);
	xSemaphoreGive(binary);
    int lim1Delay = 0;
    int lim2Delay = 0;
	while (1) {
        if (lim1Delay > 0) --lim1Delay;
        else if (lim1.read()) {
            Board_LED_Set(0, true);
            lim1Delay = 1000;
        }
        else if (!lim1.read()) {
            Board_LED_Set(0, false);
        }

        if (lim2Delay > 0) --lim2Delay;
        else if (lim2.read()) {
            Board_LED_Set(1, true);
            lim2Delay = 1000;
        }
        else if (!lim2.read()) {
            Board_LED_Set(1, false);
        }

        if (lim1Delay > 0 || lim2Delay > 0) vTaskDelay(1);
	}
}

// STEP reduction to jump between the limits and not on them
#define STEP_REDUCTION 2
static void task2(void *pvParameters) {
	DigitalIoPin lim1(0, 9, DigitalIoPin::pullup, true);
	DigitalIoPin lim2(0, 29, DigitalIoPin::pullup, true);
	DigitalIoPin dir(1, 0, DigitalIoPin::output, true);
	DigitalIoPin step(0, 24, DigitalIoPin::output, true);
    int total = 0;
    int average = 0;
    int steps = 0;
    int runs = 0;
    bool limitHit = false;
	step.write(false);
	xSemaphoreTake(binary, portMAX_DELAY);
	while (runs < 4) {
		while (lim1.read() && lim2.read()) {
			vTaskDelay(5000);
		}
		ITM_print("steps = %d\n", steps);
        if (dir.read() && lim1.read()) {
            dir.write(false);
            limitHit = true;
        }
        else if (!dir.read() && lim2.read()) {
            dir.write(true);
            limitHit = true;
        }
        step.write(true);
        vTaskDelay(1);
        step.write(false);

        if (limitHit) {
            limitHit = false;
            if (runs++ > 0) { // dont add first run since it is propably not full run
                total += steps;
            }
            steps = 0;
        }
        ++steps;
	}
	// After the loop we will not be on the limit but moved one away from it
    --runs;
    Board_LED_Set(2, true);
    vTaskDelay(2000);
    Board_LED_Set(2, false);
    average = total / runs;
#ifdef STEP_REDUCTION
    average -= STEP_REDUCTION;
#endif /*STEP_REDUCTION*/
    int count = 0;
    while (1) {
    	ITM_print("avg = %d\tcount = %d\n", average, count);
        if (count >= average) {
            dir.write((bool) !dir.read());
            count = 0;
        }
        step.write(true);
        vTaskDelay(1);
        step.write(false);
        ++count;
    }
}

int main(void) {
	prvSetupHardware();
	ITM_init();
	binary = xSemaphoreCreateBinary();
	xTaskCreate(task1, "task1", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),(TaskHandle_t *) NULL);

	xTaskCreate(task2,"task2", configMINIMAL_STACK_SIZE + 256, NULL, (tskIDLE_PRIORITY + 1UL),(TaskHandle_t *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}

