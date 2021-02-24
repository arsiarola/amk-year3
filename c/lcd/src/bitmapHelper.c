#include "bitmapHelper.h"

void bitmapTo2dArray(const uint8_t* bitmap, size_t width, size_t height, uint8_t outArray[8][8]) {
    size_t requiredWidth   = getRequiredWidth(width);
    size_t requiredHeight  = getRequiredHeight(height);

    printf("requiredHeight = %u\n", requiredHeight);
    printf("requiredWidth = %u\n", requiredWidth);

    for (int y = 0; y < requiredHeight; ++y) {
        for (int x = 0; x < requiredWidth; ++x) {
            int index = x + y*requiredWidth;
            if (index > 7) {
                break;
            }
            for (int j = 0; j < 8; ++j) {
                outArray[index][j] = 0;
                for (int i = 0; i < 5; ++i) {
                    if ((y*width + x*5 + j*width + i) < width*height) {
                        outArray[index][j] |= (bitmap[y*width*8 + x*5 + j*width + i] ? 1 : 0) << (4-i);
                        printf("%d,", bitmap[y*width*8 + x*5 + j*width + i] ? 1 : 0);
                    }
                }
                printf("\n");
            }
            printf("\n");
            printf("\n");
        }
    }
}


size_t getRequiredWidth(size_t width) {
    return (width / 5) + (width % 5 > 0 ? 1 : 0);
}

size_t getRequiredHeight(size_t height) {
    return (height / 8) + (height % 8 > 0 ? 1 : 0);
}

