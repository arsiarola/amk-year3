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
static Syslog uart = Syslog();
static EventGroupHandle_t eventGroup;

static void prvSetupHardware(void) {
    SystemCoreClockUpdate();
    Board_Init();
    /* Initial LED0 state is off */
    Board_LED_Set(0, false);
}

static void taskFunction(int num, TaskHandle_t &vTaskHandle, DigitalIoPin sw) {
    int count = 0;
    while (count < num) {
        if (sw.read()) {
            ++count;
            while(sw.read());
        }
    }

    ITM_print("Task%d waits for sync\n", num);
    xEventGroupSync( eventGroup,
            TASK_BIT(num),
            WAIT_BITS,
            portMAX_DELAY);
    uart.print("Task%d, elapsed ticks = %lu\r\n", num, xTaskGetTickCount());
    vTaskSuspend(vTaskHandle);
}

static void task1(void *pvParameters) {
    taskFunction(1, vTask1Handle, sw1);
}

static void task2(void *pvParameters) {
    taskFunction(2, vTask2Handle, sw2);
}

static void task3(void *pvParameters) {
    taskFunction(3, vTask3Handle, sw3);
}

int main(void) {
    prvSetupHardware();
    ITM_init();
    ITM_print("starting\n");
    eventGroup = xEventGroupCreate();

    xTaskCreate(task1, "task1", configMINIMAL_STACK_SIZE + 256, NULL, (tskIDLE_PRIORITY + 1UL), &vTask1Handle);
    xTaskCreate(task2, "task2", configMINIMAL_STACK_SIZE + 256, NULL, (tskIDLE_PRIORITY + 1UL), &vTask2Handle);
    xTaskCreate(task3, "task3", configMINIMAL_STACK_SIZE + 256, NULL, (tskIDLE_PRIORITY + 1UL), &vTask3Handle);

    /* Start the scheduler */
    vTaskStartScheduler();
    return 1;
}
