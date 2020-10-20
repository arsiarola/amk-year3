/*
 * Syslog.h
 *
 *  Created on: 25 Aug 2020
 *      Author: DucVoo
 */

#ifndef SYSLOG_H_
#define SYSLOG_H_

#include "FreeRTOS.h"
#include "semphr.h"
#include <string>


class Syslog {
	public:
		Syslog();
		virtual ~Syslog();
		int read();
		void write(int *description);
		void writeString(const char* description);
		void writeString(const std::string description);
		void print(const char *format, ...);



	private:
		SemaphoreHandle_t mutex;
};



#endif /* SYSLOG_H_ */
