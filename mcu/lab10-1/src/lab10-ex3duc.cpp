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
#include "semphr.h"
#include "queue.h"
#include "timers.h"
#include <cstdio>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Syslog.h"
#include "ITM_write.h"
#include "LpcUart.h"

static Syslog mutex = Syslog();
static QueueHandle_t queue;
static TimerHandle_t oneShot;
static TimerHandle_t autoReload;
//static TickType_t lastReceive {0};
static int wordNumber { 0 };
//LpcUart *mutex;

//LpcPinMap none = { .port = -1, .pin = -1 }; // unused pin has negative values in it
//LpcPinMap txpin1 = { .port = 0, .pin = 18 }; // transmit pin that goes to Arduino D4
//LpcPinMap rxpin1 = { .port = 0, .pin = 13 }; // receive pin that goes to Arduino D3
//LpcUartConfig cfg1 = { .pUART = LPC_USART0, .speed = 115200, .data =
//UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1, .rs485 = false,
//		.tx = txpin1, .rx = rxpin1, .rts = none, .cts = none };

static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();
	Board_Init();
	ITM_init();
	/* Initial LED0 state is off */
	Board_LED_Set(0, false);
	Board_LED_Set(1, false);
	Board_LED_Set(2, false);
}

void task1(void *pvParameter)
{
	char command[60];
	while (1)
	{
		int c = mutex.read();
		if (c != EOF)
		{
			xTimerReset(oneShot, 0);
			command[wordNumber] = c;
			command[wordNumber + 1] = '\0';
			if ((command[wordNumber] == '\r') || (command[wordNumber] == '\n'))
			{
				if (strncmp(command, "help", 4) == 0)
				{
					mutex.print(
							"\r\nIf no characters are received in 30 seconds all the characters received so far are discarded.\n");
					mutex.print("\rThe commands are:\n");
					mutex.print(
							"\r\tinterval <number> - set the led toggle interval in second(default is 5 seconds)\n");
					mutex.print(
							"\r\ttime - show elapsed time since the last led toggle\r\n");
				}
				else if (strncmp(command, "interval ", 9) == 0)
				{
//					int length = strtol(command + 9, NULL, 10);
//					if (length <= 0)
//					{
//						length = 5;
//					}
					float length;
					if (sscanf(command + 9, "%f", &length) == 1) {
						xTimerChangePeriod(autoReload, length*configTICK_RATE_HZ,0);
					}
					char buffer[80];
					snprintf(buffer, 80,
												"\r\nToggle timer changed to: %ums\r\n",
												xTimerGetPeriod(autoReload));
										mutex.print(buffer);				}
				else if (strncmp(command, "time", 4) == 0)
				{
					float elapsedTime = (xTaskGetTickCount()
							- xTimerGetExpiryTime(autoReload)
							+ xTimerGetPeriod(autoReload)) / 1000.0;

					char buffer[80];
					snprintf(buffer, 80,
							"\r\nElapsed time since the last led toggle: %.1fs\r\n",
							elapsedTime);
					mutex.print(buffer);
//					ITM_print("\r\nElapsed time since the last led toggle: %.1fs\r\n",(xTaskGetTickCount()-lastReceive) / 1000.0);
				}
				mutex.print("\r\n");
				wordNumber = 0;
				command[0] = '\0';
			}
			else
			{
				mutex.write(&c);
				if (++wordNumber == 59)
				{
					wordNumber = 0;
					command[0] = '\0';
					mutex.print("\r\nFULL!!!\r\n");
				}

			}
		}
//		mutex->read(&command[wordNumber], 1, portMAX_DELAY);
//		xTimerReset(oneShot, 0);
//
//		if ((command[wordNumber] == '\r') || (command[wordNumber] == '\n'))
//		{
//			if (strncmp(command, "help", 4) == 0)
//			{
//				mutex->write(
//						"\r\nIf no characters are received in 30 seconds all the characters received so far are discarded.\n");
//				mutex->write("\rThe commands are:\n");
//				mutex->write(
//						"\r\tinterval <number> - set the led toggle interval in second(default is 5 seconds)\n");
//				mutex->write(
//						"\r\ttime - show elapsed time since the last led toggle\r\n");
//			}
//			else if (strncmp(command, "interval ", 9) == 0)
//			{
//				int length = strtol(command + 9, NULL, 10);
//				if (length <= 0)
//				{
//					length = 5;
//				}
//				xTimerChangePeriod(autoReload, length*configTICK_RATE_HZ, 0);
//			}
//			else if (strncmp(command, "time", 4) == 0)
//			{
//				float elapsedTime = (xTaskGetTickCount()
//						- xTimerGetExpiryTime(autoReload)
//						+ xTimerGetPeriod(autoReload)) / 1000.0;
//
//				char buffer[80];
//				snprintf(buffer, 80,
//						"\r\nElapsed time since the last led toggle: %.1fs\r\n",
//						elapsedTime);
//				mutex->write(buffer);
//				//					ITM_print("\r\nElapsed time since the last led toggle: %.1fs\r\n",(xTaskGetTickCount()-lastReceive) / 1000.0);
//			}
//			mutex->write("\r\n");
//			wordNumber = 0;
//			command[0] = '\0';
//		}
//		else
//		{
//			mutex->write(command[wordNumber]);
//			if (++wordNumber == 60)
//			{
//				wordNumber = 0;
//				mutex.write("\r\nFULL!!!\r\n");
//			}
//
//		}
	}
}

//static void oneShotCallBack(TimerHandle_t timer)
//{
//	wordNumber = 0;
//	mutex.write("[Inactive]\r\n");
//}
//
//static void autoReloadCallBack(TimerHandle_t timer)
//{
//		Board_LED_Toggle(1);
//		lastReceive = xTaskGetTickCount();
//}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* the following is required if runtime statistics are to be collected */
extern "C"
{

	void vConfigureTimerForRunTimeStats(void)
	{
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

int main(void)
{
	prvSetupHardware();
	queue = xQueueCreate(2, sizeof(char[20]));

//	mutex = new LpcUart(cfg1);
	oneShot = xTimerCreate("one shot", 3000, pdFALSE, nullptr,
			[](TimerHandle_t xTimer)
			{
				wordNumber = 0;
				mutex.print("[Inactive]\r\n");
			});

	autoReload = xTimerCreate("auto reload", 5000, pdTRUE, nullptr,
			[](TimerHandle_t xTimer)
			{
				Board_LED_Toggle(1);
//				lastReceive = xTaskGetTickCount();
			});

	xTimerStart(oneShot, 0);
	xTimerStart(autoReload, 0);
	xTaskCreate(task1, "command task", configMINIMAL_STACK_SIZE * 4, NULL,
			(tskIDLE_PRIORITY + 1UL), (TaskHandle_t *) NULL);
	vTaskStartScheduler();

//	delete mutex;
	return 0;
}
