/*
 * Syslog.cpp
 *
 *  Created on: 25 Aug 2020
 *      Author: DucVoo
 */
#include "Syslog.h"
#include <stdarg.h> /* va_list, va_start, va_arg, va_end */

Syslog::Syslog()
{
	// TODO Auto-generated constructor stub
	this->mutex = xSemaphoreCreateMutex();
}

Syslog::~Syslog()
{
	// TODO Auto-generated destructor stub
	vSemaphoreDelete(mutex);
}

int Syslog::read()
{
	if (mutex != NULL)
	{
		if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE)
		{
			int c = Board_UARTGetChar();
			xSemaphoreGive(mutex);
			return c;
		}
	}
	return -1;
}

void Syslog::write(int *description)
{
	if (mutex != NULL)
	{
		if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE)
		{
			Board_UARTPutChar(*description);
			xSemaphoreGive(mutex);
		}
	}
}

void Syslog::writeString(const char *description)
{
	if (mutex != NULL)
	{
		if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE)
		{
			Board_UARTPutSTR(description);
			xSemaphoreGive(mutex);
		}
	}
}
void Syslog::writeString(const std::string description)
{
	writeString(description.c_str());
}

void Syslog::print(const char *format, ...)
{
	char buffer[128];
	if (mutex != NULL)
	{
		if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE)
		{
			va_list argptr;
			va_start(argptr, format);
			vsnprintf(buffer, 128, format, argptr);
			va_end(argptr);
			Board_UARTPutSTR(buffer);
			xSemaphoreGive(mutex);
		}
	}
}
