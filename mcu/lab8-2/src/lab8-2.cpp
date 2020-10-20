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
#include "LpcUart.h"

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

LpcUart *uart;
static QueueHandle_t queue;
static TickType_t filter = 50; // ms

static void prvSetupHardware(void) {
    SystemCoreClockUpdate();
    Board_Init();
    /* Initial LED0 state is off */
    Board_LED_Set(0, false);
}

typedef struct ButtonPress_t {
    int buttonNum;
    TickType_t tick;
} ButtonPress;

static void handleButton(int isrNum, int buttonNum) {
    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(isrNum));
    portBASE_TYPE xHigherPriorityWoken = pdFALSE;
    ButtonPress press = { .buttonNum = buttonNum, .tick = xTaskGetTickCountFromISR() };
    xQueueSendToBack(queue, &(press), 0);
    portEND_SWITCHING_ISR(xHigherPriorityWoken);
}


extern "C" {
    void PIN_INT0_IRQHandler(void) {
        handleButton(0, 1);
    }
    void PIN_INT1_IRQHandler(void) {
        handleButton(1, 2);
    }
    void PIN_INT2_IRQHandler(void) {
        handleButton(2, 3);
    }
}

static void msToString(char *buffer, int size, int ms) {
    if (ms < 1000)        snprintf(buffer, size, "%dms", ms);
    else if (ms >= 1000)  snprintf(buffer, size, "%.2fs", (float)ms/1000);
    else if (ms >= 60000) snprintf(buffer, size, "%.2fm", (float)ms/60000);
}

static void task1(void *pvParameters) {
    TickType_t lastPress = 0;
    ButtonPress press;
    char time[8];
    while (1) {
        if (xQueueReceive(queue, &press, portMAX_DELAY) == pdPASS) {
            TickType_t pressTime = press.tick - lastPress;
            msToString(time, 8, pressTime);
            if (pressTime > filter) {
                ITM_print("Button %d, %s\n", press.buttonNum, time);
            }
            else {
                ITM_print("Too fast press %s\n", time);
            }
            lastPress = press.tick;
        }
    }
}

#define STR_SIZE 80
#define BUF_SIZE 128
#define CMD_SIZE 32
#define MSG_SIZE 64
static void task2(void *pvParameters) {
    char str[STR_SIZE] = "";
    char buf[BUF_SIZE] = "";
    char cmd[CMD_SIZE] = "";
    char msg[MSG_SIZE] = "";
    int received;
    uint8_t index = 0;
    char c;
    while (1) {
        // reading one at a time just so we can have backspace functioning correctly
        //  in the terminal (doesnt erase the character by default just moves cursor back)
        received = uart->read(c);
        if (received > 0) {
            uart->write(c);
            // Backspace functionality for better usability
            // 8 ascii = backspace
            if (c == 8 && index > 0) {
                uart->write(' ');
                uart->write(8);
                --index;
            }
            else buf[index++] = c;

            if (c != '\n' && c != '\r' && index < BUF_SIZE-1) {
                continue;
            }
            uart->write("\n\r");
            buf[index] = '\0'; // buf hasnt been terminated yet
            ITM_print("buf: %s\n", buf);
            TickType_t value;
            if (sscanf(buf, "%s", cmd) == 1 && strcmp(cmd, "filter") == 0) {
                if (sscanf(buf, "%*s %u", &value) == 1) {
                    filter = value;
                    snprintf(msg, MSG_SIZE, "Filter updated to %u\n\r", filter);
                    uart->write(msg);
                }
            }

            buf[0] = '\0';
            index = 0;
        }
    }
}

int main(void) {
    prvSetupHardware();
    ITM_init();
    queue = xQueueCreate(10, sizeof(ButtonPress));
    uart = new LpcUart(cfg1);




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

    // sw3
    Chip_INMUX_PinIntSel(2, 1, 9);
    Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 9,
            IOCON_DIGMODE_EN | IOCON_MODE_INACT | IOCON_MODE_PULLUP);
    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(2));
    Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(2));
    Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(2));
    NVIC_ClearPendingIRQ(PIN_INT2_IRQn);
    NVIC_EnableIRQ(PIN_INT2_IRQn);

    xTaskCreate(task1, "task1", configMINIMAL_STACK_SIZE + 512, NULL, (tskIDLE_PRIORITY + 2UL),(TaskHandle_t *) NULL);
    xTaskCreate(task2, "read", configMINIMAL_STACK_SIZE + 512, NULL, (tskIDLE_PRIORITY + 2UL),(TaskHandle_t *) NULL);
    /* Start the scheduler */
    vTaskStartScheduler();
    return 1;
}
