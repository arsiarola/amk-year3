#include "Fmutex.h"
#include <mutex>

Fmutex::Fmutex() {
	semaphore = xSemaphoreCreateMutex();
}
Fmutex::~Fmutex() {
	/* delete semaphore */
	/* (not needed if object lifetime is known
	 * to be infinite) */
}
void Fmutex::lock() {
	xSemaphoreTake(semaphore, portMAX_DELAY);
}
void Fmutex::unlock() {
	xSemaphoreGive(semaphore);
}

void Fmutex::write(const char* str) {
    lock();
    Board_UARTPutSTR(str);
    unlock();
}
