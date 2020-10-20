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
#define SCT_PWM_OUT    1        /* COUT0 [index 2] Controls LED */
#define SCT_PWM_PIN_OUT    0        /* COUT0 [index 2] Controls LED */
#define SCT_PWM_RATE    50	      /* PWM frequency 50 Hz */

/* Systick timer tick rate, to change duty cycle */
#define TICKRATE_HZ     1000      /* 1 us Tick rate */

#define  MIN_DUTY 0.001 // in seconds
#define  MAX_DUTY 0.002 // in seconds

/*****************************************************************************
* Public types/enumerations/variables
****************************************************************************/
DigitalIoPin sw1(0, 8, DigitalIoPin::pullup, true);
DigitalIoPin sw2(1, 6, DigitalIoPin::pullup, true);
DigitalIoPin sw3(1, 8, DigitalIoPin::pullup, true);
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
Chip_SWM_MovablePortPinAssign(SWM_SCT0_OUT0_O, 0, 10); // simulator output port/pin
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



/* Example entry point */
int main(void)
{
/* Generic Initialization */
SystemCoreClockUpdate();
/* Enable SysTick Timer */
SysTick_Config(SystemCoreClock / TICKRATE_HZ);
Board_Init();
ITM_init();

/* Initialize the SCT as PWM and set frequency */
Chip_SCTPWM_Init(SCT_PWM);
Chip_SCTPWM_SetRate(SCT_PWM, SCT_PWM_RATE);

/* Setup Board specific output pin */
app_setup_pin();

/* Use SCT0_OUT1 pin */
Chip_SCTPWM_SetOutPin(SCT_PWM, SCT_PWM_OUT, SCT_PWM_PIN_OUT);

	const float middle = (MAX_DUTY + MIN_DUTY) / 2; // in us
	float dutycycle = middle;
	Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_OUT, SystemCoreClock * dutycycle);
	Chip_SCTPWM_Start(SCT_PWM);


	float increment = 0.000001;
	int count = 0;
	bool pressed = false;
	bool goto_middle = false;
	while (1) {
//		if (goto_middle == false && sw2.read()) {
//			goto_middle = true;
//		}
//		else if (goto_middle) {
//			if (dutycycle > middle) {
//				dutycycle -= increment;
//				if (dutycycle < middle) {
//					dutycycle = middle;
//				}
//			}
//			else if (dutycycle < middle) {
//				dutycycle += increment;
//				if (dutycycle > middle) {
//					dutycycle = middle;
//				}
//			}
//			if (dutycycle == middle) {
//				goto_middle = false;
//			}
//			Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_OUT, SystemCoreClock * dutycycle);
//		}
		if (sw2.read()) {
			dutycycle = middle;
			pressed = true;
		}

		else if (sw1.read()) {
			dutycycle -= increment;
			pressed = true;
		}
		else if (sw3.read()) {
			dutycycle += increment;
			pressed = true;
		}

		if (pressed) {
			if (dutycycle < MIN_DUTY)      dutycycle = MIN_DUTY;
			else if (dutycycle > MAX_DUTY) dutycycle = MAX_DUTY;
			Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_OUT, SystemCoreClock * dutycycle);
			pressed = false;
		}


		ITM_print("duty cycle in ticks = %u, ticks per cycle = %u, middle = %d\n", Chip_SCTPWM_GetDutyCycle(SCT_PWM, SCT_PWM_OUT), Chip_SCTPWM_GetTicksPerCycle(SCT_PWM), goto_middle);

		__WFI();
	}
	return 0;
}
