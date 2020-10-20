#include "Fmutex.h"
#include <mutex>
#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */

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

void Fmutex::print(const char *format, ...) {
    char buffer [256];
    lock();
    va_list argptr;
    va_start(argptr, format);
    vsnprintf (buffer, 256, format, argptr);
    va_end(argptr);
    Board_UARTPutSTR(buffer);
    unlock();
}

void Fmutex::print(const char c) {
	lock();
    Board_UARTPutChar(c);
    unlock();

}

