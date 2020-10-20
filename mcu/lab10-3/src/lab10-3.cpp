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

#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"
#include "ITM_write.h"
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

LpcUart *s_uart;

static TimerHandle_t s_inactivityTimer;
static TimerHandle_t s_ledToggleTimer;


static TaskHandle_t s_vTask1Handle;
static TickType_t s_interval = 5000;

#define BUF_SIZE 128
#define CMD_SIZE 32
#define HELP     "help"
#define INTERVAL "interval"
#define TIME     "time"
static char s_buffer[BUF_SIZE] = "";
static int s_bufferIndex = 0;
static int s_prefixLength = 0;
static bool s_active = true;

static int timerToString(TickType_t ms, char *str, int size) {
    return snprintf(str, size, "%u.%1lus", ms / 1000, ms % 1000 / 100);
}

static int printPrefix(LpcUart* uart, const char *message = "") {
    char buffer[128];
    s_prefixLength = snprintf(buffer, 128, "[%s] ", message);
    uart->print("%s", buffer);
}

static int printTimerPrefix(LpcUart* uart, TimerHandle_t &timer) {
    char buffer[8];
    timerToString(xTimerGetPeriod(timer), buffer, 8);
    return printPrefix(s_uart, buffer);
}

// Ugly buffer clear, is there any other way?
static void clearBuffer(LpcUart* uart, char *buffer, int &bufSize) {
    ITM_print("prefixlen = %d\n", s_prefixLength);
	for (int i = 0; i < bufSize + s_prefixLength; ++i) {
		uart->write(8);
	}
	for (int i = 0; i < bufSize + s_prefixLength; ++i) {
		uart->write(' ');
	}
	for (int i = 0; i < bufSize + s_prefixLength; ++i) {
		uart->write(8);
	}
    s_prefixLength = 0;
    buffer[0] = '\0';
    bufSize = 0;
}

static void clearAndWritePrefix(LpcUart* uart, char *buffer, int &bufSize, const char *message = "") {
    clearBuffer(uart, buffer, bufSize);
    printPrefix(uart, message);
}

static void handleBuffer(const char *buffer) {
    char cmd[CMD_SIZE];
    int length;
    if (sscanf(buffer, "%s%n", cmd, &length) == 1) {
        if (strcmp(cmd, HELP) == 0) {
            s_uart->write(
                    "help              -> display usage instructions\r\n"
                    "interval <number> -> set the led toggle interval in seconds (default is 5 seconds)\r\n"
                    "time              -> prints the number of seconds with 0.1s accuracy since the last led toggle\r\n"
            );
        }

        else if (strcmp(cmd, INTERVAL) == 0) {
            float temp;
            if (sscanf(buffer + length, "%f", &temp) == 1) {
                s_interval = temp * 1000;
                ITM_print("set led interval: %lums\n", s_interval);
                xTimerChangePeriod(s_ledToggleTimer, s_interval, 100);
            }
            // I had problems with converting from floats before so I made this code below
            // just for fun to exctract the value by using uint and array of 3 uint for decimal
            // Also got to try this do {} while(0) concept with breaks
            // And saving the position of last sscanf with newPos
            /* do { */
            /*     TickType_t value; */
            /*     TickType_t interval; */
            /*     bool exctracted = false; */
            /*     const char* newPos = buffer + length; */
            /*     if (sscanf(newPos, "%lu%n", &value, &length) != 1) { */
            /*          break; */
            /*     } */
            /*     else { */
            /*         interval = value * 1000; */
            /*         exctracted = true; */
            /*     } */

            /*     newPos += length; */
            /*     ITM_print("newPos:%s\n", newPos); */
            /*     char dot; */
            /*     if (sscanf(newPos, "%c%n", &dot, &length) != 1 || */
            /*         (dot != '.' && dot != ',')) { */
            /*         break; */
            /*     } */

            /*     newPos += length; */
            /*     ITM_print("newPos:%s\n", newPos); */
            /*     TickType_t decimal[3] = { 0 }; */
            /*     if (sscanf(newPos, "%1lu%1lu%1lu", */
            /*                 &decimal[0],  &decimal[1],  &decimal[2]) < 1) { */
            /*         break; */
            /*     } */

            /*     interval += decimal[0] * 100; */
            /*     interval += decimal[1] * 10; */
            /*     interval += decimal[2]; */
            /* } while (0); */

        }

        else if (strcmp(cmd, TIME) == 0) {
            TickType_t lastToggle = xTimerGetPeriod(s_ledToggleTimer) - (xTimerGetExpiryTime(s_ledToggleTimer) - xTaskGetTickCount());
            ITM_print("lastToggle = %u\n", lastToggle);
            s_uart->write("Last Toggle: ");
            char temp[8];
            timerToString(lastToggle, temp, 8);
            s_uart->print("%s\r\n", temp);
        }
    }
}

static void task1(void *pvParameters) {
    int received;
    char c;
    s_uart->write(
            "Give commands by typing the name and pressing enter. "
            "Backspace functionality works also.\r\n"
            "Give command help to see which commands are available.\r\n"
            "Current interval is in the angled brackets\r\n"
    );
    printTimerPrefix(s_uart, s_ledToggleTimer);
    while (1) {
        // Reading one character at a time just so we can have backspace functioning correctly
        // in the terminal (to replace the previous character with empty character)
    	// Default behavior just mover cursor back
        received = s_uart->read(c);
        if (received > 0) {
            if (!s_active) {
                clearBuffer(s_uart, s_buffer, s_bufferIndex);
                printTimerPrefix(s_uart, s_ledToggleTimer);
                s_active = true;
            }
        	xTimerReset(s_inactivityTimer, 10);
			// 8 ascii = backspace
			if (c == 8) {
				if (s_bufferIndex > 0) {
					s_uart->write(8);
					s_uart->write(' ');
					s_uart->write(8);
					--s_bufferIndex;
				}
			}
			else {
				s_uart->write(c);
				s_buffer[s_bufferIndex++] = c;
			}


            if (!(c == '\n' || c == '\r' || s_bufferIndex >= BUF_SIZE-1)) {
                continue;
            }

            s_uart->write("\n\r");
            s_buffer[s_bufferIndex] = '\0'; // buffer hasnt been terminated yet
            ITM_print("buffer: %s\n", s_buffer);
            handleBuffer(s_buffer);
            printTimerPrefix(s_uart, s_ledToggleTimer);
            s_buffer[0] = '\0';
            s_bufferIndex = 0;
        }
    }
}

static void prvSetupHardware(void) {
    SystemCoreClockUpdate();
    Board_Init();
    /* Initial LED0 state is off */
    Board_LED_Set(0, false);
}

int main(void) {
    prvSetupHardware();
    ITM_init();
    ITM_print("starting\n");
    s_uart = new LpcUart(cfg1);
    s_ledToggleTimer = xTimerCreate(
    				"led-toggle",
                      5'000,
                      pdTRUE,
                      (void *) 0,
					  [] (TimerHandle_t xTimer) {
    					Board_LED_Toggle(1);
    				  }
    );

    s_inactivityTimer = xTimerCreate(
        				"inactivity",
                          10'000,
                          pdFALSE,
                          (void *) 0,
    					  [] (TimerHandle_t xTimer) {
                          clearAndWritePrefix(s_uart, s_buffer, s_bufferIndex, "Inactive");
                          s_active = false;
                            ITM_print("Buffer cleared\n");
        				  }
    );

    xTimerStart(s_ledToggleTimer, 0);

    xTaskCreate(task1 , "task1", configMINIMAL_STACK_SIZE * 4, NULL, (tskIDLE_PRIORITY + 1UL), &s_vTask1Handle);


    /* Start the scheduler */
    vTaskStartScheduler();
    return 1;
}
