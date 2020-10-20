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
#include <stdio.h>
#include <stdlib.h>     /* srand, rand */
#include <string.h>

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

#include "FreeRTOS.h"
#include "task.h"
#include "Fmutex.h"
#include "ITM_write.h"
#include "semphr.h"
#include "user_vcom.h"


/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#define TASK_SIZE 10
#define LIST_SIZE 139
/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
const char list[][139] = { "Be careful.", "Be careful driving.", "Can you translate this for me?", "Chicago is very different from Boston.", "Don't worry.", "Everyone knows it.", "Everything is ready.", "Excellent.", "From time to time.", "Good idea.", "He likes it very much.", "Help!", "He's coming soon.", "He's right.", "He's very annoying.", "He's very famous.", "How are you?", "How's work going?", "Hurry!", "I ate already.", "I can't hear you.", "I'd like to go for a walk.", "I don't know how to use it.", "I don't like him.", "I don't like it.", "I don't speak very well.", "I don't understand.", "I don't want it.", "I don't want that.", "I don't want to bother you.", "I feel good.", "If you need my help, please let me know.", "I get off of work at 6.", "I have a headache.", "I hope you and your wife have a nice trip.", "I know.", "I like her.", "I'll call you when I leave.", "I'll come back later.", "I'll pay.", "I'll take it.", "I'll take you to the bus stop.", "I lost my watch.", "I love you.", "I'm an American.", "I'm cleaning my room.", "I'm cold.", "I'm coming to pick you up.", "I'm going to leave.", "I'm good, and you?", "I'm happy.", "I'm hungry.", "I'm married.", "I'm not busy.", "I'm not married.", "I'm not ready yet.", "I'm not sure.", "I'm sorry, we're sold out.", "I'm thirsty.", "I'm very busy. I don't have time now.", "I need to change clothes.", "I need to go home.", "I only want a snack.", "Is Mr. Smith an American?", "Is that enough?", "I think it's very good.", "I think it tastes good.", "I thought the clothes were cheaper.", "It's longer than 2 miles.", "I've been here for two days.", "I've heard Texas is a beautiful place.", "I've never seen that before.", "I was about to leave the restaurant when my friends arrived.", "Just a little.", "Just a moment.", "Let me check.", "Let me think about it.", "Let's go have a look.", "Let's practice English.", "May I speak to Mrs. Smith please?", "More than that.", "Never mind.", "Next time.", "No.", "Nonsense.", "No, thank you.", "Nothing else.", "Not recently.", "Not yet.", "Of course.", "Okay.", "Please fill out this form.", "Please take me to this address.", "Please write it down.", "Really?", "Right here.", "Right there.", "See you later.", "See you tomorrow.", "See you tonight.", "She's pretty.", "Sorry to bother you.", "Stop!", "Take a chance.", "Take it outside.", "Tell me.", "Thanks for everything.", "Thanks for your help.", "Thank you.", "Thank you miss.", "Thank you sir.", "Thank you very much.", "That looks great.", "That's alright.", "That's enough.", "That's fine.", "That's it.", "That smells bad.", "That's not fair.", "That's not right.", "That's right.", "That's too bad.", "That's too many.", "That's too much.", "The book is under the table.", "They'll be right back.", "They're the same.", "They're very busy.", "This doesn't work.", "This is very difficult.", "This is very important.", "Try it.", "Very good, thanks.", "We like it very much.", "Would you take a message please?", "Yes, really.", "You're beautiful.", "You're very nice.", "You're very smart." };
/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Sets up system hardware */
static void prvSetupHardware(void) {
	SystemCoreClockUpdate();
	Board_Init();

	/* Initial LED0 state is off */
	Board_LED_Set(0, false);
}

Fmutex guard;
SemaphoreHandle_t semaphore;

static void vReadTask(void *pvParameters) {
	char str[80] = "";
	char buffer[128] = "";
	uint8_t len = 0;
	while (1) {
		//ITM_print("%d\n", len);
		uint32_t received = USB_receive((uint8_t *)str, 79);
		str[received] = 0; /* make sure we have a zero at the end so that we can print the data */
		USB_send((uint8_t*)str, received);
		strncat(buffer, str, 128 - len);
		len = received + len >= 128 ? 128 : received + len;
		if (strchr(str, '\n') == NULL && strchr(str, '\r') == NULL && len < 128) {
			continue;
		}
		USB_send((uint8_t*)"\r\n", 2);
		if (strchr(buffer, '?') != NULL) {
			xSemaphoreGive(semaphore);
		}
		buffer[0] = '\0';
		len = 0;
	}
}

const char *thinking = "[Oracle] I find your lack of faith disturbing\r\n";
static void vReplyTask(void *pvParameters) {
	while (1) {
		if (xSemaphoreTake(semaphore, portMAX_DELAY) == pdTRUE) {
			USB_send((uint8_t*) thinking, strlen(thinking));
			vTaskDelay(configTICK_RATE_HZ * 3);
			char message[64] = "[Oracle] ";
			strncat(message, list[xTaskGetTickCount() % LIST_SIZE], 64 - strlen(message) - 3);
			strcat(message, "\r\n");
			USB_send((uint8_t*) message, strlen(message));
			vTaskDelay(configTICK_RATE_HZ * 2);
		}
	}
}

static void ReadSerial(void *pvParameters) {
	int c;
	char buffer[128] = "";
	uint8_t len = 0;
	char *stat;
	char *list;
	while (1) {
		c = guard.readChar();
		if (c == EOF) {
			vTaskDelay(3);
			continue;
		}
		buffer[len] = c;
		if (c == '\n' || c == '\r' || len >= 126){
			buffer[len+1] = '\0';
			guard.print("\r\n");
			stat = strstr(buffer, "stat");
			list = strstr(buffer, "list");

			if (stat != NULL) {
				char runTime [64];
				vTaskGetRunTimeStats(runTime);
				guard.print("%s", runTime);
			}
			else if (list != NULL) {
				char taskList [64];
				vTaskList(taskList);
				guard.print("%s", taskList);
			}
			len = 0;
			buffer[0] = '\0';
		}
		else {
			guard.print("%c", c);
			++len;
		}
	}
}

	/*****************************************************************************
	 * Public functions
	 ****************************************************************************/

	/* the following is required if runtime statistics are to be collected */
	extern "C" {

	void vConfigureTimerForRunTimeStats(void) {
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
int main(void) {
	prvSetupHardware();
	ITM_init();

	semaphore = xSemaphoreCreateCounting(TASK_SIZE, 0);

	xTaskCreate(cdc_task, "CDC",
				configMINIMAL_STACK_SIZE + 128, NULL, (tskIDLE_PRIORITY + 1UL),
				(TaskHandle_t *) NULL);

	xTaskCreate(vReadTask, "vReadSerial",
			configMINIMAL_STACK_SIZE + 256, NULL, (tskIDLE_PRIORITY + 1UL),
			(TaskHandle_t *) NULL);

	xTaskCreate(vReplyTask, "vReply",
			configMINIMAL_STACK_SIZE + 256, NULL, (tskIDLE_PRIORITY + 1UL),
			(TaskHandle_t *) NULL);

	xTaskCreate(ReadSerial, "Debug",
				configMINIMAL_STACK_SIZE + 256, NULL, (tskIDLE_PRIORITY + 1UL),
				(TaskHandle_t *) NULL);


	xSemaphoreGive(semaphore);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}

