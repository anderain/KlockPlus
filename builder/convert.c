#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitmap.h"
#include "../core/kasset.h"

// int k_convert_bmp_asset

int k_convert_bmp_asset(
    const char*     filename,
    int *           p_width,
    int *           p_height,
    unsigned char** p_raw
) {
	K_BITMAPFILEHEADER  bm_heaer;
    K_BITMAPINFOHEADER  info_header;
    unsigned char *     bmp_data;
    unsigned char *     converted;
    int                 width;
    int                 height;

    int x, y, pitch, bmp_pitch;

    bmp_data = k_load_bmp(filename, &bm_heaer, &info_header);

    if (bmp_data == NULL) {
        return KBMPE_FAILED_LOAD;
    }

    if (info_header.biBitCount != 24) {
        free(bmp_data);
        return KBMPE_INVALID_DEPTH;
    }

    if (info_header.biWidth > 128) {
        free(bmp_data);
        return KBMPE_TOO_LARGE;
    }

    width           = info_header.biWidth;
    height          = info_header.biHeight;
    pitch           = width / 8 + (width % 8 ? 1 : 0);
    converted       = (unsigned char *)malloc(pitch * height);
    bmp_pitch       = info_header.biSizeImage / height;
    memset(converted, 0, pitch * height);

    for (y = 0; y < height; ++y) {
        const unsigned char *bmp_row = bmp_data + bmp_pitch * (height - 1 - y);
        for (x = 0; x < width; ++x) {
            const unsigned char *pixel = bmp_row + 3 * x;
            const int brightness = (pixel[0] + pixel[1] + pixel[2]) / 3;
            const int dot = brightness < 127;
            unsigned char *dest = converted + y * pitch + x / 8;
            (*dest) |= (dot << (7 - x % 8));
        }
    }

    free(bmp_data);

    *p_width    = width;
    *p_height   = height;
    *p_raw      = converted;

    return KBMPE_NO_ERROR;
}