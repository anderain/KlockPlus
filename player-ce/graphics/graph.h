#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <windows.h>
#include "defs.h"
#include "assets.h"
#include "pixel.h"

#define VIDEO_BUFFER_WIDTH			320
#define VIDEO_BUFFER_HEIGHT			240
#define VIDEO_BUFFER_WIN32_ZOOM		2

// Failed
#define VIDEO_BUFFER_MODE_FAILED	0x00

// 2Bit grayscale 
// For CE1 or CE2 Palm-size
#define VIDEO_BUFFER_MODE_2BIT		2

// #define VIDEO_BUFFER_MODE_4BIT	4

// 8Bit Palette
// For CE2 or later / HPC
#define VIDEO_BUFFER_MODE_8BIT		8

// 16Bit
// Tested on CE6
#define VIDEO_BUFFER_MODE_16BIT		16

// #define VIDEO_BUFFER_MODE_24BIT	23

// 32Bit
// For Win32
#define VIDEO_BUFFER_MODE_32BIT		32

// Portrait
#define PORTRAIT_MODE_NO			0
#define PORTRAIT_MODE_LEFT			1
#define PORTRAIT_MODE_RIGHT			2

typedef struct tagGPCONFIG {
	int width;
	int height;
	HDC hdcBuffer;
	int ready;
	int portraitMode;
	void (* fnPutDisp)();
} GPCONFIG;

extern GPCONFIG GP_Config;

int		GP_Initialize				(HDC hdc, int screenWidth, int screenHeight, int portraitMode);
int		GP_GetVideoBitMode			();
void	GP_Close					();
void	GP_Clear					();
void    GP_FrameBuffer_FillWhite    ();
void 	GP_SetPixel                 (int x, int y, GP_IMAGE_COLOR color);
void	GP_DrawImage				(const GP_IMAGE *image, int _x, int _y, const GP_RECT *pRect);
void	GP_Line						(int x0, int y0, int x1, int y1, GP_IMAGE_COLOR color);
void	GP_DrawTextHZK12			(const BYTE *text, int dx, int dy, GP_IMAGE_COLOR color, int drawShadow, GP_IMAGE_COLOR shadowColor);
void	GP_DrawText					(const BYTE *text, int dx, int dy, GP_IMAGE_COLOR color, int drawShadow, GP_IMAGE_COLOR shadowColor);

#endif