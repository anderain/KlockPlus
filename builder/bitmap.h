#ifndef _KBMP_H_
#define _KBMP_H_

typedef unsigned short  BMP_WORD;
typedef unsigned int    BMP_DWORD;
typedef long            BMP_LONG;
typedef unsigned char   BMP_BYTE;

typedef struct {
    BMP_WORD bfType;		// specifies the file type
    BMP_DWORD bfSize;		// specifies the size in bytes of the bitmap file
    BMP_DWORD bfReserved;	// reserved; must be 0
    BMP_DWORD bfOffBits;	// specifies the offset in bytes from the bitmapfileheader to the bitmap bits
} K_BITMAPFILEHEADER;

typedef struct {
    BMP_DWORD biSize;			// specifies the number of bytes required by the struct
    BMP_LONG biWidth;			// specifies width in pixels
    BMP_LONG biHeight;			// specifies height in pixels
    BMP_WORD biPlanes;			// specifies the number of color planes, must be 1
    BMP_WORD biBitCount;		// specifies the number of bits per pixel
    BMP_DWORD biCompression;	// specifies the type of compression
    BMP_DWORD biSizeImage;		// size of image in bytes
    BMP_LONG biXPelsPerMeter;	// number of pixels per meter in x axis
    BMP_LONG biYPelsPerMeter;	// number of pixels per meter in y axis
    BMP_DWORD biClrUsed;		// number of colors used by the bitmap
    BMP_DWORD biClrImportant;	// number of colors that are important
} K_BITMAPINFOHEADER;

BMP_BYTE *k_load_bmp(const char *filename, K_BITMAPFILEHEADER *, K_BITMAPINFOHEADER *);

#endif