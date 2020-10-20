#include "FreeRTOS.h"
#include "semphr.h"

class Fmutex {
public:
	Fmutex();
	virtual ~Fmutex();
	void lock();
	void unlock();
private:
	SemaphoreHandle_t mutex;
};
Fmutex::Fmutex() {
	mutex = xSemaphoreCreateMutex();
}
Fmutex::~Fmutex() {
	/* delete semaphore */
	/* (not needed if object lifetime is known
	 * to be infinite) */
}
void Fmutex::lock() {
	xSemaphoreTake(mutex, portMAX_DELAY);
}
void Fmutex::unlock() {
	xSemaphoreGive(mutex);
}
