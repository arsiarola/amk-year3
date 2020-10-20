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
#include <stdlib.h>     /* abs */

#define RIGHT_DIR false
#define LEFT_DIR true

#define SIMULATOR

#ifdef SIMULATOR
#define LIM1PIN  9
#define LIM2PIN  29
#else
#define LIM1PIN  27
#define LIM2PIN  28
#endif /*SIMULATOR*/

LpcPinMap LIM1 = { .port = 0, .pin = LIM1PIN };
LpcPinMap LIM2 = { .port = 0, .pin = LIM2PIN };
LpcPinMap STEP = { .port = 0, .pin = 24 };
LpcPinMap DIR = { .port = 1, .pin = 0 };


LpcPinMap none = { .port = -1, .pin = -1}; // unused pin has negative values in it
LpcPinMap txpin1 = { .port = 0, .pin = 18 }; // transmit pin
LpcPinMap rxpin1 = { .port = 0, .pin = 13 }; // receive pin
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
QueueHandle_t queue;
LpcUart *uart;
DigitalIoPin *step;
DigitalIoPin *dir;
DigitalIoPin *lim1;
DigitalIoPin *lim2;

static int current_pps = 400;
static uint32_t average = 0;
static uint32_t threshold = 0;

static void prvSetupHardware(void) {
    SystemCoreClockUpdate();
    Board_Init();
    /* Initial LED0 state is off */
    Board_LED_Set(0, false);
}

void start_polling(int pps) {
    Chip_RIT_Disable(LPC_RITIMER);
    uint64_t cmp_value = (uint64_t) Chip_Clock_GetSystemClockRate() / pps;
    Chip_RIT_EnableCompClear(LPC_RITIMER);
    Chip_RIT_SetCounter(LPC_RITIMER, 0);
    Chip_RIT_SetCompareValue(LPC_RITIMER, cmp_value);
    Chip_RIT_Enable(LPC_RITIMER);
    NVIC_EnableIRQ(RITIMER_IRQn);
}

void stop_polling() {
    NVIC_DisableIRQ(RITIMER_IRQn);
    Chip_RIT_Disable(LPC_RITIMER);
}


extern "C" {
    void RIT_IRQHandler(void) {
        Chip_RIT_ClearIntStatus(LPC_RITIMER); // clear IRQ flag
        step->write(true);
        portBASE_TYPE xHigherPriorityWoken = pdFALSE;
        if (RIT_count > 0) --RIT_count;
        if (RIT_count == 0 || (lim1->read() && lim2->read())) {
            stop_polling();
            xSemaphoreGiveFromISR(sbRIT, &xHigherPriorityWoken);
        }
        else {
        	int pps;
            if ((average - RIT_count) < threshold) {
                pps = current_pps * (average - RIT_count) / threshold ;
            }
            else if (RIT_count < threshold) {
                pps = current_pps * RIT_count / threshold;
            }
            else pps = current_pps;
            start_polling(pps);
        }
        step->write(false);
        // End the ISR and (possibly) do a context switch
        portEND_SWITCHING_ISR(xHigherPriorityWoken);
    }
}

void RIT_start(int count, int pps) {
    RIT_count = count;
    // disable timer during configuration
    start_polling(pps);
    xSemaphoreTake(sbRIT, portMAX_DELAY);
}

#define PERCENT 10
static void task1(void *pvParameters) {
    int total = 0;
    int steps = 0;
    int runs = 0;
    dir->write(true);
    while (lim1->read() || lim2->read()); // Wait for limit switches to be open
    ITM_print("lim1 = %d lim2 = %d\n", lim1->read(), lim2->read());
    while (!lim1->read() && !lim2->read()) {// move to edge
        RIT_start(1, 500);
    }
    dir->write(!dir->read());
    while (runs < 2) {
        RIT_start(1, 500);
        ++steps;
        if ((lim1->read() && dir->read()) || (lim2->read() && !dir->read())) {
            ITM_print("Run %d = %d\n", runs+1, steps);
            dir->write(!dir->read());
            total += steps;
            steps = 0;
            ++runs;
        }
    }
    average = total / runs;
    steps = 0;
    threshold = average * PERCENT / 100;
    ITM_print("average = %d total = %d thrs %d\n", average, total, threshold);
    vTaskDelay(500);

    TickType_t startTime;
	TickType_t endTime;
	TickType_t lastTime = -1;
    int step_size = 5;
    runs = 0;
    while (1) {
        startTime = xTaskGetTickCount();
    	RIT_start(average, 10);
        endTime = xTaskGetTickCount();
        vTaskDelay(100);
        ++runs;
        if (!lim1->read() && !lim2->read()) {
            // Didnt hit the limit switch so missing steps
            // Speed too fast
            ITM_print("Limit switch not hit\n");
            current_pps /= 1.2;
            break;
        }
        if (lim1->read() && lim2->read()) {
            ITM_print("Error both limit switches on\n");
            current_pps /= 1.2;
            break;
        }
        dir->write(!dir->read()); // we have hit just one limit switch so we need to change direction
        TickType_t resultTime = endTime - startTime;
        if (resultTime == lastTime) {
            break;
        }
        lastTime = resultTime;
        ITM_print("Run %d, pps = %d, Time = %dms\n", runs, current_pps, resultTime);
        current_pps *= 1.2;
    }
	ITM_print("fastest time was = %dms\n", lastTime);
	ITM_print("highest pps was = %dpps\n", current_pps);
	int degreesPerSecond = current_pps / 400;
	ITM_print("highest RPM was = %d\n", degreesPerSecond * 60);
}

static void task2(void *pvParameters) {
    while(1) {
        if (lim1->read()) {
            Board_LED_Set(0, true);
            vTaskDelay(500);
            Board_LED_Set(0, false);
        }
    }
}

static void task3(void *pvParameters) {
    while(1) {
        if (lim2->read()) {
            Board_LED_Set(1, true);
            vTaskDelay(500);
            Board_LED_Set(1, false);
        }
    }
}

int main(void) {
    prvSetupHardware();
    ITM_init();
    sbRIT = xSemaphoreCreateBinary();
    step = new DigitalIoPin(STEP.port, STEP.pin, DigitalIoPin::output, true);
    dir  = new DigitalIoPin(DIR.port, DIR.pin, DigitalIoPin::output, true);
    lim1 = new DigitalIoPin (LIM1.port, LIM1.pin, DigitalIoPin::pullup, true);
    lim2 = new DigitalIoPin (LIM2.port, LIM2.pin, DigitalIoPin::pullup, true);
    uart = new LpcUart(cfg1);
    Chip_RIT_Init(LPC_RITIMER);
    Chip_RIT_Disable(LPC_RITIMER);
    NVIC_SetPriority(RITIMER_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
    xTaskCreate(task1, "task1", configMINIMAL_STACK_SIZE + 1000, NULL, (tskIDLE_PRIORITY + 2UL),(TaskHandle_t *) NULL);
    xTaskCreate(task2, "task2", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL), (TaskHandle_t *) NULL);
    xTaskCreate(task3, "task3", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL), (TaskHandle_t *) NULL);

    /* Start the scheduler */
    vTaskStartScheduler();
    return 1;
}
