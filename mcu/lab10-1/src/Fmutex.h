/*
 * Fmutex.h
 *
 *  Created on: 15.8.2017
 *      Author: krl
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
	void print(const char *format, ...);
	void print(const char c);
	int readChar();
private:
	SemaphoreHandle_t mutex;
};
#endif /* FMUTEX_H_ */
