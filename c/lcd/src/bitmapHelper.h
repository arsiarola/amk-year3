#ifndef BITMAPHELPER_H_
#define BITMAPHELPER_H_
#include <stdio.h>
#include <stdint.h>
void bitmapTo2dArray(const uint8_t* bitmap, size_t width, size_t height, uint8_t outArray[8][8]);
size_t getRequiredWidth(size_t width);
size_t getRequiredHeight(size_t height);

#endif /* BITMAPHELPER_H_ */
