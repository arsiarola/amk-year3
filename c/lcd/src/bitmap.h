#ifndef BITMAP_H_
#define BITMAP_H_

#include <stdio.h>
#include <stdint.h>


int drawBitMap(const uint8_t* bitmap, size_t width, size_t height, uint8_t curX, uint8_t curY, uint8_t startRegister);
void bmClearCurrentImage();
void clearCustomChars();


//Created using dot2pic.com 10x16, aka 2x2 font on the screen
#endif /* BITMAP_H_ */
