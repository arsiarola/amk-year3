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

#include "ITM_write.h"
#include "DigitalIoPin.h"

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

#define SCT_PWM            LPC_SCT0 /* Use SCT0 for PWM */
#define SCT_PWM_PIN_OUT   0        /* COUT0 [index 2] Controls LED */
#define SCT_PWM_INDEX        2       /* Index of LED PWM */
#define SCT_PWM_RATE    1 	      /* PWM frequency 1 KHz */

/* Systick timer tick rate, to change duty cycle */
#define TICKRATE_HZ     1000        /* 1 ms Tick rate */

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
static DigitalIoPin sw1(0, 17, DigitalIoPin::pullup, true);
static DigitalIoPin sw2(1, 11, DigitalIoPin::pullup, true);
static DigitalIoPin sw3(1, 9, DigitalIoPin::pullup, true);

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
	Chip_SWM_MovablePinAssign(SWM_SCT0_OUT0_O, 3);
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
/**
 * @brief	Handle interrupt from SysTick timer
 * @return	Nothing
 */
void SysTick_Handler(void){
	/* This is only used to wakeup up the device from sleep */
}
#ifdef __cplusplus
}
#endif


void SCT_Init(void);

/* Example entry point */
int main(void)
{
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
	Chip_SCTPWM_SetOutPin(SCT_PWM, SCT_PWM_INDEX, SCT_PWM_PIN_OUT);

	/* Start with 5% duty cycle */
	int dutycycle = 95; // in percent
	Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_INDEX, Chip_SCTPWM_PercentageToTicks(SCT_PWM, dutycycle));


	Chip_SCTPWM_Start(SCT_PWM);

	/* Enable SysTick Timer */
	SysTick_Config(SystemCoreClock / TICKRATE_HZ);
	int increment_small = 5;
	int increment_big = 15;
	ITM_print("dutycycle = %d%%\n", dutycycle); // higher dutycycle = dimmer led

	Board_LED_Set(0, false);
	Board_LED_Set(1, false);

	while (1) {
		if (sw1.read()) {
			dutycycle += sw2.read() ? increment_big : increment_small;
			if (dutycycle > 100) dutycycle = 100;
			Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_INDEX,
							Chip_SCTPWM_PercentageToTicks(SCT_PWM, dutycycle));
			ITM_print("dutycycle = %d%%\n", dutycycle); // higher dutycycle = dimmer led
			while (sw1.read());
		}
		if (sw3.read()) {
			dutycycle -= sw2.read() ? increment_big : increment_small;
			if (dutycycle < 0) dutycycle = 0;
			Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_INDEX,
							Chip_SCTPWM_PercentageToTicks(SCT_PWM, dutycycle));
			ITM_print("dutycycle = %d%%\n", dutycycle); // higher dutycycle = dimmer led
			while (sw3.read());
		}

		__WFI();
	}
	return 0;
}

