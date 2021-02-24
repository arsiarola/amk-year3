#include "bitmapHelper.h"
void bitmapTo2dArray(const uint8_t* bitmap, size_t width, size_t height, uint8_t outArray[8][8]) {
    size_t requiredWidth   = getRequiredWidth(width);
    size_t requiredHeight  = getRequiredHeight(height);

    for (int y = 0; y < requiredHeight; ++y) {
        for (int x = 0; x < requiredWidth; ++x) {
            int index = x + y*requiredWidth;
            if (index > 7) {
                break;
            }
            for (int j = 0; j < 8; ++j) {
                for (int i = 0; i < 5; ++i) {
                    if ((y*width + x*5 + j*width + i) < width*height) {
                        outArray[index][j] |= (bitmap[y*width*8 + x*5 + j*width + i] ? 1 : 0) << (4-i);
                    }
                }
            }
        }
    }
}


size_t getRequiredWidth(size_t width) {
    return (width / 5) + (width % 5 > 0 ? 1 : 0);
}

size_t getRequiredHeight(size_t height) {
    return (height / 5) + (height % 5 > 0 ? 1 : 0);
}

