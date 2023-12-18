#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"

BMP_BYTE * k_load_bmp(const char *filename, K_BITMAPFILEHEADER *bmHeader, K_BITMAPINFOHEADER* biHeader) {
    FILE            *filePtr;       // our file pointer
    unsigned char   *bitmapImage;   // store image data
    int             imageIdx = 0;   // image index counter

    // open file in read binary mode
    filePtr = fopen(filename,"rb");
    if (filePtr == NULL) {
        return NULL;
    }

    // read the bitmap file header
    fread(&bmHeader->bfType, sizeof(bmHeader->bfType), 1,filePtr);
	fread(&bmHeader->bfSize, sizeof(bmHeader->bfSize), 1, filePtr);
	fread(&bmHeader->bfReserved, sizeof(bmHeader->bfReserved), 1, filePtr);
	fread(&bmHeader->bfOffBits, sizeof(bmHeader->bfOffBits), 1, filePtr);

    // verify that this is a .BMP file by checking bitmap id
    if (bmHeader->bfType != 0x4D42) {
        fclose(filePtr);
        return NULL;
    }

    // read the bitmap info header
    fread(biHeader, sizeof(K_BITMAPINFOHEADER), 1, filePtr); 

    // move file pointer to the beginning of bitmap data
    fseek(filePtr, bmHeader->bfOffBits, SEEK_SET);

    // allocate enough memory for the bitmap image data
    bitmapImage = (unsigned char*)malloc(biHeader->biSizeImage);

    // verify memory allocation
    if (!bitmapImage) {
        free(bitmapImage);
        fclose(filePtr);
        return NULL;
    }

    // read in the bitmap image data
    fread(bitmapImage,biHeader->biSizeImage,1,filePtr);

    // make sure bitmap image data was read
    if (bitmapImage == NULL) {
        fclose(filePtr);
        return NULL;
    }

    // swap the R and B values to get RGB (bitmap is BGR)
    // for (imageIdx = 0; imageIdx < biHeader->biSizeImage; imageIdx += 3) {
    //     tempRGB = bitmapImage[imageIdx];
    //     bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
    //     bitmapImage[imageIdx + 2] = tempRGB;
    // }

    // close file and return bitmap image data
    fclose(filePtr);

    return bitmapImage;
}