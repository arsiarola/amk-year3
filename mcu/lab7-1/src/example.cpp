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

#include "board.h"
#include "ITM_write.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#define SCT_PWM            LPC_SCT0 /* Use SCT0 for PWM */
#define SCT_PWM_PIN_OUT    1        /* COUT1 Generate square wave */
#define SCT_PWM_PIN_LED    0        /* COUT0 [index 2] Controls LED */
#define SCT_PWM_OUT        1        /* Index of OUT PWM */
#define SCT_PWM_LED        2        /* Index of LED PWM */
#define SCT_PWM_RATE   10000        /* PWM frequency 10 KHz */

/* Systick timer tick rate, to change duty cycle */
#define TICKRATE_HZ     1000        /* 1 ms Tick rate */

#define LED_STEP_CNT      20        /* Change LED duty cycle every 20ms */
#define OUT_STEP_CNT    1000        /* Change duty cycle every 1 second */

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Setup board specific pin muxing */
static void app_setup_pin(void)
{
	/* Enable SWM clock before altering SWM */
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);

#if defined(BOARD_NXP_LPCXPRESSO_1549)
	/* Connect SCT output 1 to PIO0_29 */
	Chip_SWM_MovablePinAssign(SWM_SCT0_OUT1_O, 29);
	Chip_SWM_MovablePortPinAssign(SWM_SCT0_OUT0_O, 1, 1);
#endif

	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif
void SysTick_Handler(void)
{
	/* This is only used to wakeup up the device from sleep */
}
#ifdef __cplusplus
}
#endif

/* Example entry point */
int main(void)
{
	uint32_t cnt1 = 0, cnt2 = 0;
	int led_dp = 0, led_step = 1, out_dp = 0;

	/* Generic Initialization */
	SystemCoreClockUpdate();
	Board_Init();
	ITM_init();

	/* Initialize the SCT as PWM and set frequency */
	Chip_SCTPWM_Init(SCT_PWM);
	Chip_SCTPWM_SetRate(SCT_PWM, SCT_PWM_RATE);

	/* Setup Board specific output pin */
	app_setup_pin();

	/* Use SCT0_OUT1 pin */
	Chip_SCTPWM_SetOutPin(SCT_PWM, SCT_PWM_OUT, SCT_PWM_PIN_OUT);
	Chip_SCTPWM_SetOutPin(SCT_PWM, SCT_PWM_LED, SCT_PWM_PIN_LED);

	/* Start with 0% duty cycle */
	Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_OUT, Chip_SCTPWM_GetTicksPerCycle(SCT_PWM));
	Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_LED, Chip_SCTPWM_GetTicksPerCycle(SCT_PWM));
	Chip_SCTPWM_Start(SCT_PWM);


	/* Enable SysTick Timer */
	SysTick_Config(SystemCoreClock / TICKRATE_HZ);

	int count = 0;
	int run = 0;

	ITM_print("test\n");

	while (1) {
		if (count >= 1000) {
			if (run > 2 || run < 0) run = 0;
			Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_OUT, Chip_SCTPWM_GetTicksPerCycle(SCT_PWM));
			Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_LED, Chip_SCTPWM_GetTicksPerCycle(SCT_PWM));
			if      (run == 0) Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_OUT, 0);
			else if (run == 1) Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_LED, 0);
			else if (run == 2) {
				Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_LED, 0);
				Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_OUT, 0);
			}

			ITM_print("run = %d\n", run);
			count = 0;
			++run;
		}
		++count;
		__WFI();
	}
	return 0;
}
