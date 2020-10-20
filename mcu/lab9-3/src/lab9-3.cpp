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
#include "event_groups.h"
#include "semphr.h"
#include "ITM_write.h"
#include "DigitalIoPin.h"
#include "Syslog.h"

#define TASK_BIT(num) (1 << num)
#define WAIT_BITS (TASK_BIT(1) | TASK_BIT(2) | TASK_BIT(3))

static DigitalIoPin sw1(0, 17, DigitalIoPin::pullup, true);
static DigitalIoPin sw2(1, 11, DigitalIoPin::pullup, true);
static DigitalIoPin sw3(1, 9, DigitalIoPin::pullup, true);


static TaskHandle_t vTask1Handle;
static TaskHandle_t vTask2Handle;
static TaskHandle_t vTask3Handle;
static TaskHandle_t vTask4Handle;
static Syslog uart = Syslog();
static EventGroupHandle_t eventGroup;

static void prvSetupHardware(void) {
    SystemCoreClockUpdate();
    Board_Init();
    /* Initial LED0 state is off */
    Board_LED_Set(0, false);
}

static void taskFunction(int num, DigitalIoPin &sw) {
    while (true) {
        if (sw.read()) {
            ITM_print("Task%d pressed\n", num);
            xEventGroupSetBits(eventGroup, TASK_BIT(num));
            while (sw.read());
        }
    }
}

static void task1(void *pvParameters) {
    taskFunction(1, sw1);
}

static void task2(void *pvParameters) {
    taskFunction(2, sw2);
}

static void task3(void *pvParameters) {
    taskFunction(3, sw3);
}

static void task4(void *pvParameters) {
    EventBits_t uxBits;
    do {
        uxBits = xEventGroupWaitBits(
                eventGroup,
                WAIT_BITS,
                pdTRUE,
                pdTRUE,
                10'000
                );
        if ((uxBits & WAIT_BITS) != WAIT_BITS ) { // time elapsed
            uart.print("Fail\n\r");
            for (int i = 1; i <= 3; ++i) {
                if ((uxBits & TASK_BIT(i)) == 0) {
                    uart.print("Task%d didnt meet the deadline, elapsed ticks = %lu\r\n", i, xTaskGetTickCount());
                }
            }
        }
        else {
            uart.print("Success, all tasks were done before deadline\n\r");
        }
    } while ((uxBits & WAIT_BITS) == WAIT_BITS);
    vTaskSuspend(vTask4Handle);
}
int main(void) {
    prvSetupHardware();
    ITM_init();
    ITM_print("starting\n");
    eventGroup = xEventGroupCreate();

    xTaskCreate(task1, "task1", configMINIMAL_STACK_SIZE + 256, NULL, (tskIDLE_PRIORITY + 1UL), &vTask1Handle);
    xTaskCreate(task2, "task2", configMINIMAL_STACK_SIZE + 256, NULL, (tskIDLE_PRIORITY + 1UL), &vTask2Handle);
    xTaskCreate(task3, "task3", configMINIMAL_STACK_SIZE + 256, NULL, (tskIDLE_PRIORITY + 1UL), &vTask3Handle);
    xTaskCreate(task4, "watchdog", configMINIMAL_STACK_SIZE + 256, NULL, (tskIDLE_PRIORITY + 2UL), &vTask4Handle);

    /* Start the scheduler */
    vTaskStartScheduler();
    return 1;
}
