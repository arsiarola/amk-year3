/*
 * @brief State Configurable Timer (SCT) PWM example
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "DigitalIoPin.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Fmutex.h"
#include "ITM_write.h"
#include "semphr.h"
#include "user_vcom.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#define SCT_PWM_RATE    1000	      /* PWM frequency */

/* Systick timer tick rate, to change duty cycle */
#define TICKRATE_HZ     1000      /* 1 us Tick rate */

/* types/enumerations/variables */
typedef struct LpcMap_t {
		uint8_t port;
		uint8_t pin;
        CHIP_SWM_PIN_MOVABLE_T movable;
        LPC_SCT_T *sct;
        uint8_t index;
        uint8_t outPin;
} LpcMap;
static LpcMap redMap = { .port = 0, .pin = 25 , .movable = SWM_SCT0_OUT0_O, .sct = LPC_SCT0, .index = 1, .outPin = 0};
static LpcMap greenMap = { .port = 0, .pin = 3, .movable = SWM_SCT0_OUT1_O, .sct = LPC_SCT0, .index = 2, .outPin = 1};
static LpcMap blueMap = { .port = 1, .pin = 1, .movable = SWM_SCT1_OUT0_O, .sct = LPC_SCT1, .index = 3, .outPin = 0};

/*****************************************************************************
* Private functions
****************************************************************************/

/* Setup board specific pin muxing */
static void app_setup_pin(void)
{
/* Enable SWM clock before altering SWM */
Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);

#if defined(BOARD_NXP_LPCXPRESSO_1549)
Chip_SWM_MovablePortPinAssign(redMap.movable, redMap.port, redMap.pin);
Chip_SWM_MovablePortPinAssign(greenMap.movable, greenMap.port, greenMap.pin);
Chip_SWM_MovablePortPinAssign(blueMap.movable, blueMap.port, blueMap.pin);
#endif

Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);
}

static void set_rgb(uint8_t r, uint8_t g, uint8_t b) {
    // 1 - needed since the leds are active low
	Chip_SCTPWM_SetDutyCycle(redMap.sct, redMap.index, Chip_SCTPWM_GetTicksPerCycle(redMap.sct) * (1 - (float) r / 255));
	Chip_SCTPWM_SetDutyCycle(greenMap.sct, greenMap.index, Chip_SCTPWM_GetTicksPerCycle(greenMap.sct) * (1 - (float) g / 255));
	Chip_SCTPWM_SetDutyCycle(blueMap.sct, blueMap.index, Chip_SCTPWM_GetTicksPerCycle(blueMap.sct) * (1 - (float) b / 255));
}

static void str_toupper(char * str) {
    while (*str) {
        *str = toupper((unsigned char) *str);
        ++str;
    }
}

static const char *COMMAND = "rgb";
#define STR_SIZE 80
#define BUF_SIZE 128
#define CMD_SIZE 32
static void vTask1 (void *pvParameters) {
	vTaskDelay(100); /* wait until semaphores are created */
	uint8_t len = 0;
	char str[STR_SIZE] = "";
	char buf[BUF_SIZE] = "";
    char cmd[CMD_SIZE] = "";
    int r = 0;
    int g = 0;
    int b = 0;
    uint8_t index = 0;
	while (1) {
        // if we have delay why does it not receive more characters than 1 at a time?
        /* vTaskDelay(5000); */
		uint32_t received = USB_receive((uint8_t *)str, STR_SIZE-1);
		if (received > 0) {
			USB_send((uint8_t*)str, received);
			str[received] = 0; /* make sure we have a zero at the end */

            // Backspace functionality for better usability
            char *temp = str;
            while (*temp && index < BUF_SIZE-1) { // -1 to leave space for null
                ITM_print("%d\n", *temp);
                // 8 ascii = backspace
                if (*temp == 8 && index > 0) --index;
                else                         buf[index++] = *temp;
                ++temp;
            }

			if (strchr(str, '\n') == NULL && strchr(str, '\r') == NULL && BUF_SIZE-1) {
				continue;
			}
			USB_send((uint8_t*)"\r\n", 2);
            buf[index] = '\0'; // buf hasnt been terminated yet
            ITM_print("buf: %s\n", buf);
			if (sscanf(buf, "%s", cmd) == 1 && strcmp(cmd, COMMAND) == 0) {
                str_toupper(buf); // user is able to type any case (we need to use %X then to get uppercase conversion)
                int value;
                if (sscanf(buf, "%*s #%2X%2X%2X", &r, &g, &b) == 3) {
                    ITM_print("r = %u, g = %u, b = %u\n", r, g, b);
                    set_rgb(r, g, b);
                }
                else ITM_print("ERROR: # or rgb values not found\n");
			}
            else ITM_print("ERROR: rgb command not found\n");
			buf[0] = '\0';
			index = 0;
		}
		//__WFI();
	}
}

/* Example entry point */
int main(void)
{
   /* Generic Initialization */
    SystemCoreClockUpdate();

    Board_Init();
    ITM_init();

    /* Initialize the SCT as PWM and set frequency */
    Chip_SCTPWM_Init(redMap.sct);
    Chip_SCTPWM_Init(blueMap.sct);
    Chip_SCTPWM_SetRate(redMap.sct, SCT_PWM_RATE);
    Chip_SCTPWM_SetRate(blueMap.sct, SCT_PWM_RATE);

    /* Setup Board specific output pin */
    app_setup_pin();

    /* Use SCT0_OUT1 pin */
    Chip_SCTPWM_SetOutPin(redMap.sct, redMap.index, redMap.outPin);
    Chip_SCTPWM_SetOutPin(greenMap.sct, greenMap.index, greenMap.outPin);
    Chip_SCTPWM_SetOutPin(blueMap.sct, blueMap.index, blueMap.outPin);

    Chip_SCTPWM_SetDutyCycle(redMap.sct, redMap.index, 0);
    Chip_SCTPWM_SetDutyCycle(greenMap.sct, greenMap.index, 0);
    Chip_SCTPWM_SetDutyCycle(blueMap.sct, blueMap.index, 0);
    Chip_SCTPWM_Start(redMap.sct);
    Chip_SCTPWM_Start(blueMap.sct);


    SysTick_Config(SystemCoreClock / TICKRATE_HZ);
	xTaskCreate(cdc_task, "CDC",
				configMINIMAL_STACK_SIZE + 128, NULL, (tskIDLE_PRIORITY + 1UL),
				(TaskHandle_t *) NULL);
	xTaskCreate(vTask1, "task1",
					configMINIMAL_STACK_SIZE + 512, NULL, (tskIDLE_PRIORITY + 1UL),
					(TaskHandle_t *) NULL);
	vTaskStartScheduler();


    return 1;
}
