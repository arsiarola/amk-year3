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

    bitmapTo2dArray(bitmap, width, height, bmChars);

    size_t requiredWidth   = getRequiredWidth(width);
    size_t requiredHeight  = getRequiredHeight(height);
    uint8_t index = 0;
    uint8_t reg = startRegister;
    for (int y = 0; y < requiredHeight; ++y){ for (int x = 0; x < requiredWidth; ++x) {
        if (reg > 7 || index > 7) {
            break;
        }

        int total = 0;
        for (int i = 0; i < 8; ++i) {
            total += bmChars[index][i];
        }
        if (total == 0) {
            lcdSetCursor(curX, curY);
            lcdPutChar('@');
        }
        else {
            lcdCreateChar((uint8_t *) bmChars[index], reg);
            lcdSetCursor(curX, curY);
            lcdPutChar(reg);
            printf("reg = %d\n", reg);
            ++reg;
        }
        ++index;
        ++curX;
    }
    ++curY;
    curX = cursorX;
    }
    bmRequiredWidth   = requiredWidth;
    bmRequiredHeight   = requiredHeight;
    bmCurX = cursorX;
    bmCurY = cursorY;

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
        lcdPrintf("%*s", endX - startX, "");
    }
}

void clearCustomChars() {
    for (int j = 0; j < 8; ++j) { for (int i = 0; i < 8; ++i) {
        bmCharRegisters[j][i] = 0;
    } }
}
