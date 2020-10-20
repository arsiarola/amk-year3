/*
 * Fmutex.cpp
 *
 *  Created on: 15.8.2017
 *      Author: krl
 */

#include "Fmutex.h"
#include <mutex>
#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */

Fmutex::Fmutex() {
	// TODO Auto-generated constructor stub
	mutex = xSemaphoreCreateMutex();
}

Fmutex::~Fmutex() {
	// TODO Auto-generated destructor stub
}

void Fmutex::lock()
{
	xSemaphoreTake(mutex, portMAX_DELAY);
}

void Fmutex::unlock()
{
	xSemaphoreGive(mutex);
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

int Fmutex::readChar() {
	int c;
	lock();
	c = Board_UARTGetChar();
	unlock();
	return c;
}
