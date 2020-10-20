/*
 * Fmutex.h
 *
 *  Created on: Aug 27, 2020
 *      Author: 1andr
 */

#ifndef FMUTEX_H_
#define FMUTEX_H_

#include "FreeRTOS.h"
#include "semphr.h"

class Fmutex {
public:
	Fmutex();
	virtual ~Fmutex();
	void lock();
	void unlock();
	void write(const char* str);
private:
	SemaphoreHandle_t semaphore;
};

#endif /* FMUTEX_H_ */
