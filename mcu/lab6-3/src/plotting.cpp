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
#include <algorithm>    // std::max


#define RIGHT_DIR false
#define LEFT_DIR true

#define SIMULATOR 1

#if SIMULATOR == 1
#define LIMX1PIN  9
#define LIMX2PIN  29
#define LIMY1PIN  3
#define LIMY2PIN  29
#else
#define LIMX1PIN  27
#define LIMX2PIN  28
#endif /*SIMULATOR*/

LpcPinMap LIMX1 = { .port = 0, .pin = LIMX1PIN };
LpcPinMap LIMX2 = { .port = 0, .pin = LIMX2PIN };
LpcPinMap LIMY1 = { .port = 1, .pin = LIMY1PIN };
LpcPinMap LIMY2 = { .port = 0, .pin = LIMY2PIN };
LpcPinMap XMOTOR = { .port = 0, .pin = 24 };
LpcPinMap YMOTOR = { .port = 0, .pin = 27 };
LpcPinMap XDIR = { .port = 1, .pin = 0 };
LpcPinMap YDIR = { .port = 0, .pin = 28 };


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
DigitalIoPin *xMotor;
DigitalIoPin *yMotor;
DigitalIoPin *xDir;
DigitalIoPin *yDir;
DigitalIoPin *lim1;
DigitalIoPin *lim2;

static int current_pps = 400;
static uint32_t average = 0;
static uint32_t threshold = 0;

int pps;

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

void initValues(int x1_, int y1_, int x2_, int y2_) {
    xDir->write(x2_ > x1_);
    yDir->write(y2_ > y1_);
    x1              = x1_ < x2_ ? x1_ : x2_;
    x2              = x1_ > x2_ ? x1_ : x2_;
    y1              = y1_ < y2_ ? y1_ : y2_;
    y2              = y1_ > y2_ ? y1_ : y2_;
    ITM_print("%d,%d    %d,%d\n", x1,y1, x2,y2);
    dx              = abs(x2-x1);
    dy              = abs(y2-y1);
    xGreater        = (dx > dy);
    m_new           = xGreater ? 2 * dy : 2 * dx;
    slope_error_new = m_new - (xGreater ? dx : dy);
    prevX           = x1;
    prevY           = y1;
    steps           = std::max(dx, dy);
    count           = 0;
    x               = x1;
    y               = y1;
    prevX           = x;
    prevY           = y;
    ITM_print("dx = %d, dy = %d\n", dx, dy);
}

void bresenham() {
    xMotor->write((bool)(x != prevX));
    yMotor->write((bool)(y != prevY));
    ITM_print("count = %d, steps = %d\n", steps, count);
    ITM_print("(%d,%d)  %d,%d\n", x,y, (bool)(x != prevX), (bool)(y != prevY));
    prevX = x;
    prevY = y;

    slope_error_new += m_new;
    if (xGreater) {
        ++x;
        if (slope_error_new >= 0) {
            slope_error_new -= 2 * dx;
            ++y;
        }
    }
    else {
        ++y;
        if (slope_error_new >= 0) {
            slope_error_new -= 2 * dy;
            ++x;
        }
    }
    ++count;
    xMotor->write(false);
    yMotor->write(false);
}



extern "C" {
    void RIT_IRQHandler(void) {
        Chip_RIT_ClearIntStatus(LPC_RITIMER); // clear IRQ flag
        portBASE_TYPE xHigherPriorityWoken = pdFALSE;
        bresenham();
        if (count > steps) {
            stop_polling();
            xSemaphoreGiveFromISR(sbRIT, &xHigherPriorityWoken);
        }
        else {
            start_polling(pps);
        }

        // End the ISR and (possibly) do a context switch
        portEND_SWITCHING_ISR(xHigherPriorityWoken);
    }
}

void RIT_start(int count, int pps_) {
    RIT_count = count;
    // disable timer during configuration
    pps = pps_;
    start_polling(pps);
    xSemaphoreTake(sbRIT, portMAX_DELAY);
}


void plotLine(int x1_,int y1_, int x2_,int y2_, int pps) {
    initValues(x1_,y1_, x2_,y2_);
    RIT_start(1, pps);
}

#define PERCENT 10
static void task1(void *pvParameters) {
    while (lim1->read() || lim2->read()); // Wait for limit switches to be open
    plotLine(-100,-50, 0,-50, 100);
    plotLine(0,-50, -50,-100, 100);
    plotLine(-50,-100, -100,-50, 100);
    while (1) {
        ITM_print("starting plotting\n");
        vTaskDelay(2000);
        plotLine(0, 0, 10, 50, 100);
        vTaskDelay(2000);
        // plotLine(0, 0, 10, 50, 100);
    }

    vTaskDelay(500);

}

static void task2(void *pvParameters) {
    while (1) {
        if (lim1->read()) {
            Board_LED_Set(0, true);
            vTaskDelay(500);
            Board_LED_Set(0, false);
        }
    }
}

static void task3(void *pvParameters) {
    while (1) {
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
    xMotor = new DigitalIoPin(XMOTOR.port, XMOTOR.pin, DigitalIoPin::output, true);
    yMotor = new DigitalIoPin(YMOTOR.port, YMOTOR.pin, DigitalIoPin::output, true);
    xDir  = new DigitalIoPin(XDIR.port, XDIR.pin, DigitalIoPin::output, true);
    yDir  = new DigitalIoPin(YDIR.port, YDIR.pin, DigitalIoPin::output, true);
    lim1 = new DigitalIoPin (LIMX1.port, LIMX1.pin, DigitalIoPin::pullup, true);
    lim2 = new DigitalIoPin (LIMX2.port, LIMX2.pin, DigitalIoPin::pullup, true);
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
