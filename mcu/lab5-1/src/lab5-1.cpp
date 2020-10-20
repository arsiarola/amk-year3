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
/* Sets up system hardware */
static void prvSetupHardware(void) {
	SystemCoreClockUpdate();
	Board_Init();

	/* Initial LED0 state is off */
	Board_LED_Set(0, false);
}

//#define DUC
#ifdef DUC
static void readButtons(void *pvParameters) {
DigitalIoPin b1(0, 8, DigitalIoPin::pullup, true);
	DigitalIoPin b3(1, 8, DigitalIoPin::pullup, true);
	DigitalIoPin lmsw1(0, 9, DigitalIoPin::pullup, true);
	DigitalIoPin lmsw2(0, 29, DigitalIoPin::pullup, true);

	DigitalIoPin direct(1, 0, DigitalIoPin::output, true);
	DigitalIoPin step(0, 24, DigitalIoPin::output, true);
	while(1){
		 if((b1.read() && b3.read()) || (lmsw1.read() && lmsw2.read())){
			 continue;
		 }else if((b1.read() && !lmsw2.read()) || (b3.read() && !lmsw1.read())){
			 direct.write(b3.read());
			 step.write(true);
			 vTaskDelay(1);
			 step.write(false);
			 vTaskDelay(1);
		 }
	}
}
#else

static void readButtons(void *pvParameters) {
	DigitalIoPin lim1(0, 9, DigitalIoPin::pullup, true);
	DigitalIoPin lim2(0, 29, DigitalIoPin::pullup, true);
	DigitalIoPin button1(0, 8, DigitalIoPin::pullup, true);
	DigitalIoPin button3(1, 8, DigitalIoPin::pullup, true);

	DigitalIoPin dir(1, 0, DigitalIoPin::output, true);
	DigitalIoPin step(0, 24, DigitalIoPin::output, true);

	vTaskDelay(100);
	while (1) {
		if ((lim1.read() && lim2.read()) || (button1.read() && button3.read())) continue;
		if ((button1.read() && !lim2.read()) || (button3.read() && !lim1.read())) {
			dir.write(button3.read());
			step.write(true);
			vTaskDelay(1);
			step.write(false);
		}
	}
}
#endif /*DUC*/

int main(void) {
	prvSetupHardware();

	xTaskCreate([](void *pvParameters) {
		DigitalIoPin lim1(0, 9, DigitalIoPin::pullup, true);
		DigitalIoPin lim2(0, 29, DigitalIoPin::pullup, true);
		while (1) {
			Board_LED_Set(0, lim1.read());
			Board_LED_Set(1, lim2.read());
		}
	},
	"limSwLeds", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),(TaskHandle_t *) NULL);

	xTaskCreate(readButtons,
	"read", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),(TaskHandle_t *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}

