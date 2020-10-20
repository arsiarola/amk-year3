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
#include "LpcUart.h"
#include "ITM_write.h"
#include "DigitalIoPin.h"


#define STR 64
#define RIGHT false
#define LEFT true

#define SIMULATOR
#ifdef SIMULATOR
#define LIMX1PIN  9
#define LIMX2PIN  29
#else
#define LIMX1PIN  27
#define LIMX2PIN  28
#endif /*SIMULATOR*/

LpcPinMap LIMX1 = { .port = 0, .pin = LIMX1PIN };
LpcPinMap LIMX2 = { .port = 0, .pin = LIMX2PIN };
LpcPinMap STEP = { .port = 0, .pin = 24 };
LpcPinMap DIR = { .port = 1, .pin = 0 };

LpcPinMap none = { .port = -1, .pin = -1}; // unused pin has negative values in it
LpcPinMap txpin1 = { .port = 0, .pin = 18 }; // transmit pin that goes to Arduino D4
LpcPinMap rxpin1 = { .port = 0, .pin = 13 }; // receive pin that goes to Arduino D3
LpcUartConfig cfg1 = {
		.pUART = LPC_USART0,
		.speed = 115200,
		.data = UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1,
		.rs485 = false,
		.tx = txpin1,
		.rx = rxpin1,
		.rts = none,
		.cts = none
};

volatile uint32_t RIT_count = 0;
SemaphoreHandle_t sbRIT;
LpcUart *uart;
DigitalIoPin *xMotor;
DigitalIoPin *lim1;
DigitalIoPin *lim2;

int pps = 400;

static void prvSetupHardware(void) {
	SystemCoreClockUpdate();
	Board_Init();
	/* Initial LED0 state is off */
	Board_LED_Set(0, false);
}

extern "C" {
void RIT_IRQHandler(void) {

	// This used to check if a context switch is required
	portBASE_TYPE xHigherPriorityWoken = pdFALSE;
	// Tell timer that we have processed the interrupt.
	// Timer then removes the IRQ until next match occurs
	Chip_RIT_ClearIntStatus(LPC_RITIMER); // clear IRQ flag
	if (RIT_count <= 0 || lim1->read() || lim2->read()) {
		RIT_count = 0;
		Chip_RIT_Disable(LPC_RITIMER); // disable timer
		// Give semaphore and set context switch flag if a higher priority task was woken up
		xSemaphoreGiveFromISR(sbRIT, &xHigherPriorityWoken);
	} else {
		RIT_count--;
		xMotor->write(true);
		xMotor->write(false);
	}
	// End the ISR and (possibly) do a context switch
	portEND_SWITCHING_ISR(xHigherPriorityWoken);
}
}

void RIT_start(int count, int pps) {
	uint64_t cmp_value;
	// Determine approximate compare value based on clock rate and passed interval
	cmp_value = (uint64_t) Chip_Clock_GetSystemClockRate() / pps;
	// disable timer during configuration
	Chip_RIT_Disable(LPC_RITIMER);
	RIT_count = count;
	// enable automatic clear on when compare value==timer value
	// this makes interrupts trigger periodically
	Chip_RIT_EnableCompClear(LPC_RITIMER);
	// reset the counter
	Chip_RIT_SetCounter(LPC_RITIMER, 0);
	Chip_RIT_SetCompareValue(LPC_RITIMER, cmp_value);
	// start counting
	Chip_RIT_Enable(LPC_RITIMER);
	// Enable the interrupt signal in NVIC (the interrupt controller)
	NVIC_EnableIRQ(RITIMER_IRQn);
	// wait for ISR to tell that we're done
	if (xSemaphoreTake(sbRIT, portMAX_DELAY) == pdTRUE) {
		// Disable the interrupt signal in NVIC (the interrupt controller)
		NVIC_DisableIRQ(RITIMER_IRQn);
	} else {
		// unexpected error
	}
}

static bool subStrValue(const char *buf, const char *sub, int &value) {
		char *found = strstr(buf, sub);
		if(found != NULL) {
			char format[32];
			sprintf(format, sub);
			strcat(format, "%d");
			if (sscanf(found, format, &value) == 1) {
				return true;
			}
		}
		return false;
}
static bool handleValue(const char *buf) {
	int value;
	DigitalIoPin dir(1, 0, DigitalIoPin::output, true);
	if (subStrValue(buf, "left ", value)) {
		ITM_print("left %d\n\r", value);
		dir.write(LEFT);
		RIT_start(value, pps);
	}
	else if (subStrValue(buf, "right ", value)) {
		ITM_print("right %d\n\r", value);
		dir.write(RIGHT);
		RIT_start(value, pps);
	}
	else if (subStrValue(buf, "pps ", value)) {
		ITM_print("pps %d\n\r", value);
		pps = value;
	}
	else 	{
		ITM_print("Error\n\r");
		return false;
	}
	return true;
}
static void task1(void *pvParameters) {
	char buf[STR] = "";
	char str[STR] = "";
	int received = 0;
	int len = 0;

	while (1) {
		received = uart->read(str, STR, portTICK_PERIOD_MS * 100);
		if(received > 0) {
			uart->write(str, received);
			ITM_print("str = %s\n", str);
			strncat(buf, str, STR - len);
			len = received + len >= STR-1 ? STR-1 : received + len;
			memset(str, 0, sizeof(str));
		}
		else continue;
		if (strchr(buf, '\n') == NULL && strchr(buf, '\r') == NULL && len < STR-1) {
			continue;
		}
		uart->write("\n\r", 2);
		buf[len-1] = '\0';
		handleValue(buf);
		buf[0] = '\0';
		len = 0;
	}
}

int main(void) {
	prvSetupHardware();
	ITM_init();
	sbRIT = xSemaphoreCreateBinary();
	xMotor = new DigitalIoPin(STEP.port, STEP.pin, DigitalIoPin::output, true);
	uart = new LpcUart(cfg1);
	lim1 = new DigitalIoPin (LIMX1.port, LIMX1.pin, DigitalIoPin::pullup, true);
	lim2 = new DigitalIoPin (LIMX2.port, LIMX2.pin, DigitalIoPin::pullup, true);

	Chip_RIT_Init(LPC_RITIMER);
	Chip_RIT_Disable(LPC_RITIMER);
	NVIC_SetPriority(RITIMER_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
	xTaskCreate(task1, "task1", configMINIMAL_STACK_SIZE + 256, NULL, (tskIDLE_PRIORITY + 1UL),(TaskHandle_t *) NULL);

	//xTaskCreate(task2,"task2", configMINIMAL_STACK_SIZE + 256, NULL, (tskIDLE_PRIORITY + 1UL),(TaskHandle_t *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();
	return 1;
}
