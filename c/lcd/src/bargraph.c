#include "bargraph.h"
#include "lcd.h"

// return -1 if bitmap too big or goes out of bounds of the screen
int drawBitMap(const uint8_t* bitmap, size_t width, size_t height, uint8_t cursorX, uint8_t cursorY) {
    uint8_t curX = cursorX;
    uint8_t curY = cursorY;
    size_t requiredWidth;
    size_t requiredHeight;

    requiredWidth   = width / 5;
    requiredWidth  += width % 5 > 0 ? 1 : 0;
    requiredHeight  = height / 8;
    requiredHeight += height % 8 > 0 ? 1 : 0;
    printf("requiredHeight = %u\n", requiredHeight);
    printf("requiredWidth = %u\n", requiredWidth);

    int charCount = requiredWidth * requiredHeight;
    // we have only 8 register to use for creating custom characters
    if (charCount > 8) {
        return -1;
    }
    //TODO add checking for out of bounds if we want that?
    /* if (curX * details.fontWidth + requiredWidth > details.fontWidth * */

    for (int j = 0; j < 8; ++j) {
        for (int i = 0; i < 8; ++i) {
            chars[j][i] = 0;
        }
    }

    int index = 0;
    for (int y = 0; y < requiredHeight; ++y) {
        for (int x = 0; x < requiredWidth; ++x) {
            index = x + y*requiredWidth;
            for (int j = 0; j < 8; ++j) {
                for (int i = 0; i < 5; ++i) {
                    if ((y*width + x*5 + j*width + i) < width*height) {
                        printf("%d,", bitmap[y*width*8 + x*5 + j*width + i]? 1 : 0);
                        chars[index][j] |= (bitmap[y*width*8 + x*5 + j*width + i] ? 1 : 0) << (4-i);
                    }
                }
                printf("\n");
            }
            printf("\n");
            printf("\n");
            lcdCreateChar((uint8_t *) chars[index], index);
            lcdSetCursor(curX++, curY);
            lcdSend(index, 1); // 1 means HIGH
        }
        ++curY;
        curX = cursorX;
    }
}

