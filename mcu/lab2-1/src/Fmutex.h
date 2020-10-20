#include "FreeRTOS.h"
#include "semphr.h"

class Fmutex {
public:
	Fmutex();
	virtual ~Fmutex();
	void lock();
	void unlock();
private:
	SemaphoreHandle_t semaphore;
};
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
