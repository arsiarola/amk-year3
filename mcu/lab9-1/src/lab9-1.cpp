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

#define WAIT_BIT (1 << 0)

static DigitalIoPin sw1(0, 17, DigitalIoPin::pullup, true);
static DigitalIoPin sw2(1, 11, DigitalIoPin::pullup, true);
static DigitalIoPin sw3(1, 9, DigitalIoPin::pullup, true);
Syslog uart = Syslog();
EventGroupHandle_t eventGroup;

static void prvSetupHardware(void) {
    SystemCoreClockUpdate();
    Board_Init();
    /* Initial LED0 state is off */
    Board_LED_Set(0, false);
}

static void taskPrint(int num) {
    char buffer[40] = "";
    snprintf(buffer, 40, "Task %d, elapsed ticks: %lu\n\r",
             num, xTaskGetTickCount());
    uart.print(buffer);
}

static TickType_t randomInt(int min, int max) {
    if (max < min) {
        TickType_t temp = min;
        min = max;
        max = temp;
    }
    ITM_print("random = %d, ", rand());
    TickType_t random = (float)rand() / (float)RAND_MAX * (max-min+1) + min;
    ITM_print("delay = %d\n", random);
    return random;
}

static void taskHandle(int num) {
    vTaskDelay(100);
    while (xEventGroupWaitBits(
                eventGroup,
                WAIT_BIT,
                pdFALSE,
                pdTRUE,
                portMAX_DELAY)
            & (WAIT_BIT) != WAIT_BIT) {
    }
    srand(time(NULL) * num);

    while (true) {
        taskPrint(num);
        vTaskDelay(randomInt(1000, 2000));
    }
}

TaskHandle_t vTask1Handle;
static void task1(void *pvParameters) {
    vTaskDelay(100);
    while (true) {
        if (sw1.read() || sw3.read()) {
            xEventGroupSetBits(eventGroup, WAIT_BIT);
            ITM_print("set the bit\n");
            vTaskSuspend(vTask1Handle);
        }
    }
}

static void task2(void *pvParameters) {
    taskHandle(2);
}

static void task3(void *pvParameters) {
    taskHandle(3);
}

static void task4(void *pvParameters) {
    taskHandle(4);
}

int main(void) {
    prvSetupHardware();
    ITM_init();
    ITM_print("test\n");
    eventGroup = xEventGroupCreate();

    xTaskCreate(task1, "task1", configMINIMAL_STACK_SIZE + 512, NULL, (tskIDLE_PRIORITY + 1UL), &vTask1Handle);
    xTaskCreate(task2, "task2", configMINIMAL_STACK_SIZE + 512, NULL, (tskIDLE_PRIORITY + 1UL),(TaskHandle_t *) NULL);
    xTaskCreate(task3, "task3", configMINIMAL_STACK_SIZE + 512, NULL, (tskIDLE_PRIORITY + 1UL),(TaskHandle_t *) NULL);
    xTaskCreate(task4, "task4", configMINIMAL_STACK_SIZE + 512, NULL, (tskIDLE_PRIORITY + 1UL),(TaskHandle_t *) NULL);

    /* Start the scheduler */
    vTaskStartScheduler();
    return 1;
}
