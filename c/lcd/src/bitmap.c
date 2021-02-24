#include "bitmap.h"
#include "bitmapHelper.h"
#include "lcd.h"


uint8_t bmCharRegisters[8][8] = { 0 }; // 8 is the height of the font and we have 8 register avaialble
uint8_t bmChars[8][8] = { 0 };
size_t  bmRequiredWidth = 0;
size_t  bmRequiredHeight = 0;
uint8_t bmCurX = 0;
uint8_t bmCurY = 0;

// return -1 if bitmap too big or goes out of bounds of the screen
int drawBitMap(const uint8_t* bitmap, size_t width, size_t height, uint8_t cursorX, uint8_t cursorY, uint8_t startRegister) {
    if (startRegister > 7) {
        printf("startRegister cannot be greater than 7\n");
        return -1;
    }
    uint8_t curX = cursorX;
    uint8_t curY = cursorY;
    size_t requiredWidth   = getRequiredWidth(width);
    size_t requiredHeight  = getRequiredHeight(height);

    printf("requiredHeight = %u\n", requiredHeight);
    printf("requiredWidth = %u\n", requiredWidth);

    int charCount = requiredWidth * requiredHeight;
    // we have only 8 register to use for creating custom characters
    if (charCount > 8) {
        return -1;
    }


    //TODO add checking for out of bounds if we want that?
    /* if (curX * details.fontWidth + requiredWidth > details.fontWidth * */

    clearCustomChars();

    bitmapTo2dArray(bitmap, width, height, bmChars);

    for (int y = 0; y < requiredHeight; ++y) {
        for (int x = 0; x < requiredWidth; ++x) {
            int index = x + y*requiredWidth;
            int reg = index + startRegister;
            // since index just gets bigger, if it goes over 7 we know that
            // we cant dont have anymore space in the registers
            if (reg > 7) {
                break;
            }
            lcdCreateChar((uint8_t *) bmChars[index], reg);
            lcdSetCursor(curX++, curY);
            lcdSend(index, 1); // 1 means HIGH
        }
        ++curY;
        curX = cursorX;
    }
    bmRequiredWidth   = requiredWidth;
    bmRequiredHeight   = requiredHeight;
}

void bmClearCoordinates(uint8_t startX,uint8_t startY, uint8_t endX,uint8_t endY) {
    if (startX > endX) {
        startX = startX + endX;
        endX   = startX - endX;
        startX = startX - endX;
    }
    if (startY > endY) {
        startY = startY + endY;
        endY   = startY - endY;
        startY = startY - endY;
    }
    for (int i = 0; i < endY - startY; ++i) {
        lcdSetCursor(startX, startY + i);
        lcdPrint("%*s", endX - startX, "");
    }
}

void clearCustomChars() {
    for (int j = 0; j < 8; ++j) {
        for (int i = 0; i < 8; ++i) {
            bmCharRegisters[j][i] = 0;
        }
    }
}
