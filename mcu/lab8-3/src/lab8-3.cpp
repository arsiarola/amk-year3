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

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
    (byte & 0x80 ? '1' : '0'), \
    (byte & 0x40 ? '1' : '0'), \
    (byte & 0x20 ? '1' : '0'), \
    (byte & 0x10 ? '1' : '0'), \
    (byte & 0x08 ? '1' : '0'), \
    (byte & 0x04 ? '1' : '0'), \
    (byte & 0x02 ? '1' : '0'), \
    (byte & 0x01 ? '1' : '0')

#define DEFAULT_CODE 0b11110110
#define TIMEOUT 15000

enum PressCmd { reset, configure, press };
enum DoorStatus { closed, open, configuring };

static QueueHandle_t queue;
static SemaphoreHandle_t doorSem;

static DigitalIoPin *sw3;
static TickType_t filter = 250; // ms
static uint8_t correctCode =  0b11110110;
static TickType_t lastPress = 0;
DoorStatus doorStatus = DoorStatus::closed;

static void prvSetupHardware(void) {
    SystemCoreClockUpdate();
    Board_Init();
    /* Initial LED0 state is off */
    Board_LED_Set(0, false);
}

typedef struct ButtonPress_t {
    uint8_t value;
    PressCmd cmd;
} ButtonPress;

static void handleButton(int isrNum, uint8_t sendValue) {
    TickType_t isrEntryTick = xTaskGetTickCountFromISR();
    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(isrNum));
    portBASE_TYPE xHigherPriorityWoken = pdFALSE;
    ITM_print("isrEntryTick = %u\n", isrEntryTick);
    TickType_t pressTime = isrEntryTick - lastPress;
    if (pressTime >= filter) {
        ButtonPress press = { .value = sendValue, .cmd = PressCmd::press};
        xQueueSendToBack(queue, &(press), 0);
    }
    else {
        //ITM_print("Too fast %ums!\n", pressTime);
    }
    lastPress = isrEntryTick;
    portEND_SWITCHING_ISR(xHigherPriorityWoken);
}

extern "C" {
    void PIN_INT0_IRQHandler(void) {
        handleButton(0, 0);
        ITM_print("sw1\n");
    }
    void PIN_INT1_IRQHandler(void) {
        handleButton(1, 1);
        ITM_print("sw2\n");
    }
}

static void handleDoorLed(DoorStatus status) {
    Board_LED_Set(0, status == DoorStatus::closed);
    Board_LED_Set(1, status == DoorStatus::open);
    Board_LED_Set(2, status == DoorStatus::configuring);
}

static void task1(void *pvParameters) {
    ButtonPress press;
    uint8_t newCode = 0;
    uint8_t newCodeLength = 0;
    BaseType_t queueStatus;
    while (1) {
    	queueStatus = xQueueReceive(queue, &press, TIMEOUT);
        if (queueStatus == pdPASS) {
            switch (press.cmd) {
                case PressCmd::reset:
                    correctCode = DEFAULT_CODE;
                    break;
                case PressCmd::configure:
                    doorStatus = DoorStatus::configuring;
                    xSemaphoreGive(doorSem);
                    newCodeLength = 0;
                    newCode = 0;
                    break;

                case PressCmd::press:
                    if (doorStatus == DoorStatus::open) break;
                    newCode = newCode << 1; // make space for the new bit
                    newCode |= 0b00000001 & press.value;
                    ++newCodeLength;
                    if (doorStatus == DoorStatus::configuring && newCodeLength == 8) {
                        correctCode = newCode;
                        newCodeLength = 0;
                        newCode = 0;
                        ITM_print("newCode = %u\n", newCode);
                        doorStatus = DoorStatus::closed;
                        xSemaphoreGive(doorSem);
                    }


                    else if (newCodeLength >= 8 && newCode == correctCode) {
                        //Open the doorSem
                        doorStatus = DoorStatus::open;
                        xSemaphoreGive(doorSem);
                        newCodeLength = 0;
                        newCode = 0;
                    }
                    break;
            }

            }
        else if (queueStatus == errQUEUE_EMPTY) {
            newCodeLength = 0;
            newCode = 0;
        }
        ITM_print("code: " BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(correctCode));
        ITM_print("new : " BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(newCode));
        ITM_print("length = %u\n", newCodeLength);
    }
}

static void task2(void *pvParameters) {
    uint32_t sw3PressedTime = 0;
    uint32_t delay = 100;
    while (1) {
        if (sw3PressedTime <= 3000+delay && sw3PressedTime > 3000) {
            ITM_print("sw3 pressed for 3 sec\n");
            ButtonPress press = { .value = 0, .cmd = PressCmd::configure};
            xQueueSendToBack(queue, &(press), 0);
        }
        if (sw3->read()) sw3PressedTime += delay;
        else sw3PressedTime = 0;
        vTaskDelay(delay);
    }
}

static void task3(void *pvParameters) {
    while (1) {
        if (xSemaphoreTake(doorSem, portMAX_DELAY) == pdTRUE) {
            handleDoorLed(doorStatus);
            switch (doorStatus) {
                case DoorStatus::open:
                    vTaskDelay(5000);
                    doorStatus = DoorStatus::closed;
                    handleDoorLed(doorStatus);
                    break;
                case DoorStatus::closed:
                    break;
                case DoorStatus::configuring:
                    break;
            }
        }
    }
}

int main(void) {
    prvSetupHardware();
    ITM_init();
    sw3 =  new DigitalIoPin(1, 9, DigitalIoPin::pullup, true);
    queue = xQueueCreate(10, sizeof(ButtonPress));
    doorSem = xSemaphoreCreateBinary();

    Chip_PININT_Init(LPC_GPIO_PIN_INT);
    Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_PININT);
    Chip_SYSCTL_PeriphReset(RESET_PININT);

    /* Confiure interrupts */
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

    xTaskCreate(task1, "task1", configMINIMAL_STACK_SIZE + 512, NULL, (tskIDLE_PRIORITY + 1UL),(TaskHandle_t *) NULL);
    xTaskCreate(task2, "task2", configMINIMAL_STACK_SIZE + 512, NULL, (tskIDLE_PRIORITY + 1UL),(TaskHandle_t *) NULL);
    xTaskCreate(task3, "task3", configMINIMAL_STACK_SIZE + 512, NULL, (tskIDLE_PRIORITY + 2UL),(TaskHandle_t *) NULL);
    xSemaphoreGive(doorSem);
    /* Start the scheduler */
    vTaskStartScheduler();
    return 1;
}
