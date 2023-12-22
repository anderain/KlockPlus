#include <math.h>
#include <stdlib.h>
#include "graph.h"
#include "../ce-utils.h"
#include "../device.h"

HDC			hdcBuffer				= NULL;
HBITMAP		hBitmapBuffer			= NULL;
HBITMAP		hBitmapOld				= NULL;
BYTE *		BitmapPixels			= NULL;
int			BitmapPixelsSize		= 0;
BYTE *		FrameBuffer				= NULL;
int	 		FrameBufferSize			= 0;
int			VideoBitMode			= -1;

// Params for Set Pixel
int GP_SP_Param_X = 0;
int	GP_SP_Param_Y = 0;
int	GP_SP_Param_R = 0;
int	GP_SP_Param_G = 0;
int	GP_SP_Param_B = 0;

extern HWND hWndMain;

GPCONFIG GP_Config	 = {
	0,	// Width
	0,	// Height
	0,	// HDC
	0,	// Is initialized?
	0,	// Portrait Mode
	0	// Function of PutDisp
};

int GP_CreateVideoBuffer(HDC hdc, int screenWidth, int screenHeight, int modeToSet, int portraitMode) {

	BITMAPINFO*		pbmi				= NULL;
	int				paletteSize			= 0;
	int				bmiSizeWithPalette	= 0;
	BYTE *			pixelBuffer			= NULL;
	int				bitCount;
	int				w, h, i;

	GP_Config.width = w = screenWidth;
	GP_Config.height = h = screenHeight;

	// Basic infomation
	switch (modeToSet) {
		//-----------------------------
		// * 2 BIT MODE
		//-----------------------------
		case VIDEO_BUFFER_MODE_2BIT: {

			paletteSize = 4;
			BitmapPixelsSize = w / 4 * h; 	// 4 PIXEL = 1 BYTE
			bitCount = 2;

			// Width alignment
			// if (WidthInBytes % 4 != 0) WidthInBytes = (WidthInBytes / 4 + 1) * 4;

			// Select SetPixel function
			// GP_Config.fnSetPixel = IGP_SetPixel;

			break;
		}
		//-----------------------------
		// * 8 BIT MODE
		//-----------------------------
		case VIDEO_BUFFER_MODE_8BIT: {
			paletteSize = 256;
			BitmapPixelsSize = w * 1 * h; // 1 BYTE = 8 BITS
			bitCount = 8;

			// Select SetPixel function
			// GP_Config.fnSetPixel = IGP_SetPixel;
 
			break;
		}
		//-----------------------------
		// * 16 BIT MODE
		//-----------------------------
		case VIDEO_BUFFER_MODE_16BIT: {
			paletteSize = 0;
			BitmapPixelsSize = w * 2 * h; // 2 BYTE = 16 BITS
			bitCount = 16;

			// Select SetPixel function
			// GP_Config.fnSetPixel = IGP_SetPixel;

			break;
		}
		//-----------------------------
		// * 32 BIT MODE
		//-----------------------------
		case VIDEO_BUFFER_MODE_32BIT: {
			paletteSize = 0;
			BitmapPixelsSize = w * 4 * h; // 4 BYTE = 32 BITS
			bitCount = 32;

			// Select SetPixel function
			// GP_Config.fnSetPixel = IGP_SetPixel;

			break;
		}
		//-----------------------------
		// NOT DEFINED, FAILED!
		//-----------------------------
		default: {
			return VIDEO_BUFFER_MODE_FAILED;
		}
	}

	// Allocate framebuffer
	FrameBufferSize = w * h * 3;
	FrameBuffer = (BYTE *)malloc(FrameBufferSize);

	// Failed to allocate
	if (!FrameBuffer) {
		MessageBox(NULL, _T("FAILED TO ALLOCATE FRAME BUFFER"), _T("ERROR Frame Buffer"), MB_OK);
		return VIDEO_BUFFER_MODE_FAILED;
	}

	VideoBitMode = modeToSet;
	
	// Header size to allocate
	bmiSizeWithPalette = sizeof(BITMAPINFO) + paletteSize * sizeof(RGBQUAD);

	// Create memory DC
	hdcBuffer = CreateCompatibleDC(hdc);

	GP_Config.width = w;
	GP_Config.height = h;
	
	// Allocate header
	pbmi = (BITMAPINFO *)malloc(bmiSizeWithPalette);
	memset(pbmi, 0, bmiSizeWithPalette);

	// Write palette
	switch (VideoBitMode) {
		//-----------------------------
		// * 2 BIT MODE
		//-----------------------------
		// 4 colors in palette
		//-----------------------------
		case VIDEO_BUFFER_MODE_2BIT: {
			// Grayscale color:
			BYTE grayscale[] = { 0x00, 0x80, 0xc0, 0xff };
			// Write palette
			for (i = 0; i < paletteSize; ++i) {
				pbmi->bmiColors[i].rgbRed		= grayscale[i];
				pbmi->bmiColors[i].rgbGreen		= grayscale[i];
				pbmi->bmiColors[i].rgbBlue		= grayscale[i];
				pbmi->bmiColors[i].rgbReserved	= 0;
			}
			break;
		}
		//-----------------------------
		// * 8 BIT MODE
		//-----------------------------
		// 256 colors in palette
		//-----------------------------
		case VIDEO_BUFFER_MODE_8BIT: {
			// Write palette
			for (i = 0; i < paletteSize; ++i) {
				// pbmi->bmiColors[i].rgbRed		= WIN_PALETTE_256[i][0];
				// pbmi->bmiColors[i].rgbGreen		= WIN_PALETTE_256[i][1];
				// pbmi->bmiColors[i].rgbBlue		= WIN_PALETTE_256[i][2];
				// pbmi->bmiColors[i].rgbReserved	= 0;
				
				// USE RGB233

				const int r2bit = (i & 0xc0) >> 6;
				const int g3bit = (i & 0x38) >> 3;
				const int b3bit = (i & 0x7);

				pbmi->bmiColors[i].rgbRed		= (BYTE)(r2bit * 255 / 3);
				pbmi->bmiColors[i].rgbGreen		= (BYTE)(g3bit * 255 / 7);
				pbmi->bmiColors[i].rgbBlue		= (BYTE)(b3bit * 255 / 7);
				pbmi->bmiColors[i].rgbReserved	= 0;
			}
			break;
		}
	}

	pbmi->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth			= portraitMode ? h : w;
	pbmi->bmiHeader.biHeight		= -(portraitMode ? w : h); // from top to bottom
	pbmi->bmiHeader.biPlanes		= 1;
	pbmi->bmiHeader.biBitCount		= bitCount;
	pbmi->bmiHeader.biSizeImage		= 0;
	pbmi->bmiHeader.biCompression	= 0;
	pbmi->bmiHeader.biXPelsPerMeter	= 0;
	pbmi->bmiHeader.biYPelsPerMeter	= 0;
	pbmi->bmiHeader.biClrImportant	= 0;
	// Note: when creating 2bit DIBsection
	// biClrUsed should be set to 0
	pbmi->bmiHeader.biClrUsed		= VideoBitMode == VIDEO_BUFFER_MODE_2BIT ? 0 : paletteSize;

	hBitmapBuffer = CreateDIBSection(hdcBuffer, pbmi, DIB_RGB_COLORS, (void**)&pixelBuffer, NULL, 0);

	// FAILED TO CREATE BITMAP
	if (!hBitmapBuffer || !pixelBuffer) {
		DWORD	errorCode;
		TCHAR	szBuf[200];
		TCHAR	szTitle[]	= _T("FAILED: VIDEO BUFFER");
		TCHAR	szFormat[]	= _T("Error Code = %d.\n")
							  _T("Failed to create %dbit bitmap!\n")
							  _T("Image Size: %dx%d\n")
							  _T("Buffer Size = %d");

		errorCode = GetLastError();
		wsprintf(szBuf, szFormat, errorCode, bitCount, w, h, BitmapPixelsSize);
		MessageBox(hWndMain, szBuf, szTitle, MB_OK);

		// Delete memory DC
		DeleteDC(hdcBuffer);
		hdcBuffer = NULL;
		free(pbmi);

		// Free framebuffer
		free(FrameBuffer);

		// Quit application
		DestroyWindow(hWndMain);
		return VIDEO_BUFFER_MODE_FAILED;
	}
	
	// Save old bitmap
	hBitmapOld = SelectObject(hdcBuffer, hBitmapBuffer);

	BitmapPixels				= pixelBuffer;	
	GP_Config.hdcBuffer			= hdcBuffer;
	GP_Config.portraitMode		= portraitMode;
	GP_SelectPutDisp();

	// Delete bitmap header
	free(pbmi);

	// Set ready flag!
	GP_Config.ready = TRUE; 

	return modeToSet;
}

int GP_GetVideoBitMode() {
	return VideoBitMode;
}

int GP_Initialize(HDC hdc, int screenWidth, int screenHeight, int portraitMode) {
	// Default video mode: 8bit
	int modeToSet = VIDEO_BUFFER_MODE_8BIT;

#if (IS_WIN_CE)
	// * Detect CE Version
	//   WinCE 1.x : FORCE 2bit
	if (DeviceInfo.osInfo.dwMajorVersion < 2) {
		modeToSet = VIDEO_BUFFER_MODE_2BIT;
	}
	// * WinCE 2.0 or later
	//   Depend on device compatible bitmap's bits of pixel
	else {
		switch (DeviceInfo.bitmap.bmBitsPixel) {
			case 2:  modeToSet = VIDEO_BUFFER_MODE_2BIT;  break;
			case 8:  modeToSet = VIDEO_BUFFER_MODE_8BIT;  break;
			case 16: modeToSet = VIDEO_BUFFER_MODE_16BIT; break;
			case 32: modeToSet = VIDEO_BUFFER_MODE_32BIT; break;
		}
	}
#else
	// * Win32
	modeToSet = VIDEO_BUFFER_MODE_32BIT;
#endif
	
#ifdef FOCE_VIDEO_BUFFER_MODE
	modeToSet = FOCE_VIDEO_BUFFER_MODE;
#endif

	return GP_CreateVideoBuffer(hdc, screenWidth, screenHeight, modeToSet, portraitMode);
}

void GP_Close() {
	if (!hdcBuffer) return;

	GP_Config.ready = 0;

	SelectObject(hdcBuffer, hBitmapOld);
	DeleteObject(hBitmapBuffer);
	DeleteDC(hdcBuffer);
}

void GP_Clear() {
	memset(FrameBuffer, 0x00, FrameBufferSize);;
	memset(BitmapPixels, 0x00, BitmapPixelsSize);
}

void GP_FrameBuffer_FillWhite() {
	memset(FrameBuffer, 0xff, FrameBufferSize);
}


//---------------------------------------------------------------------
//
// * Set Pixel
//
//---------------------------------------------------------------------

// Set pixel
_inline void IGP_SetPixel() {
	BYTE *raw;
	int offset;

	// offset = 3 * (GP_SP_Param_Y * VIDEO_BUFFER_WIDTH + GP_SP_Param_X)

	offset = (GP_SP_Param_Y << 8) + (GP_SP_Param_Y << 6);
	offset = offset + GP_SP_Param_X;
	offset = (offset << 1) + offset;

	raw = FrameBuffer + offset;

	raw[0] = GP_SP_Param_R;
	raw[1] = GP_SP_Param_G;
	raw[2] = GP_SP_Param_B;
}

// Fast grayscale
const int FAST_GRAY[] = {
	0, 0, 0,
	1, 1, 1,
	2, 2, 2,
	3, 3, 3
};

// 2Bit - Portrait Left
static void IGP_PutDisp2Bit_PL() {
	int x, y, i = 0, t = 0, offsetByte, offsetBit;
	BYTE *pixel8 = BitmapPixels;
	BYTE colorGray;

	for (y = 0; y < VIDEO_BUFFER_HEIGHT; ++y) {
		t = VIDEO_BUFFER_HEIGHT - 1 - y;
		for (x = 0; x < VIDEO_BUFFER_WIDTH; ++x, i += 3) {
			colorGray = _2BIT_GRAY_FAST( FrameBuffer[i + 0], FrameBuffer[i + 1], FrameBuffer[i + 2]);
			offsetByte = (x << 6) - (x << 2) + (t >> 2);
			offsetBit = t & 3;
			pixel8[offsetByte] |= (colorGray << ((3 - offsetBit) << 1));
		}
	}
}

// 2Bit - Portrait Right
static void IGP_PutDisp2Bit_PR() {
	int x, y, i = 0, t = 0, offsetByte, offsetBit;
	BYTE *pixel8 = BitmapPixels;
	BYTE colorGray;

	for (y = 0; y < VIDEO_BUFFER_HEIGHT; ++y) {
		offsetBit = y & 3;
		for (x = 0; x < VIDEO_BUFFER_WIDTH; ++x, i += 3) {
			colorGray = _2BIT_GRAY_FAST( FrameBuffer[i + 0], FrameBuffer[i + 1], FrameBuffer[i + 2]);
			t = VIDEO_BUFFER_WIDTH - 1 - x;
			offsetByte = (t << 6) - (t << 2) + (y >> 2);
			pixel8[offsetByte] |= (colorGray << ((3 - offsetBit) << 1));
		}
	}
}

// 2Bit - Landscape
static void IGP_PutDisp2Bit_LS() {
	int x, y, i = 0, t = 0, offsetByte, offsetBit;
	BYTE *pixel8 = BitmapPixels;
	BYTE colorGray;

	for (y = 0; y < VIDEO_BUFFER_HEIGHT; ++y) {
		t = (y << 6) + (y << 4); // t = y * 320 / 4
		for (x = 0; x < VIDEO_BUFFER_WIDTH; ++x, i += 3) {
			colorGray = _2BIT_GRAY_FAST( FrameBuffer[i + 0], FrameBuffer[i + 1], FrameBuffer[i + 2]);
			// offsetByte = y * 320 / 4 + (x / 4);
			offsetByte = t + (x >> 2);
			offsetBit = x & 3;
			pixel8[offsetByte] |= (colorGray << ((3 - offsetBit) << 1));
		}
	}
}

// 8Bit - Portrait Left
static void IGP_PutDisp8Bit_PL() {
	int x, y, i = 0, j = 0;
	BYTE *pixel8 = BitmapPixels;

	for (y = 0; y < VIDEO_BUFFER_HEIGHT; ++y) {
		for (x = 0; x < VIDEO_BUFFER_WIDTH; ++x, i += 3) {
			// j = x * VIDEO_BUFFER_HEIGHT + y;
			// equiv to above, h = 240
			j = (x << 8) - (x << 4) + VIDEO_BUFFER_HEIGHT - 1 - y;

			pixel8[j] = RGB888_TO_BYTE_RGB233(
				FrameBuffer[i + 0],
				FrameBuffer[i + 1],
				FrameBuffer[i + 2]
			);
		}
	}
}

// 8Bit - Portrait Right
static void IGP_PutDisp8Bit_PR() {
	int x, y, i = 0, j = 0;
	BYTE *pixel8 = BitmapPixels;

	int t;
	for (y = 0; y < VIDEO_BUFFER_HEIGHT; ++y) {
		for (x = 0; x < VIDEO_BUFFER_WIDTH; ++x, i += 3) {
			t = VIDEO_BUFFER_WIDTH - x - 1;
			j = (t << 8) - (t << 4) + y;
			pixel8[j] = RGB888_TO_BYTE_RGB233(
				FrameBuffer[i + 0],
				FrameBuffer[i + 1],
				FrameBuffer[i + 2]
			);
		}
	}
}

// 8Bit - Portrait Left
static void IGP_PutDisp8Bit_LS() {
	int x, y, i = 0, j = 0;
	BYTE *pixel8 = BitmapPixels;

	for (y = 0; y < VIDEO_BUFFER_HEIGHT; ++y) {
		for (x = 0; x < VIDEO_BUFFER_WIDTH; ++x, i += 3, ++j) {
			pixel8[j] = RGB888_TO_BYTE_RGB233(
				FrameBuffer[i + 0],
				FrameBuffer[i + 1],
				FrameBuffer[i + 2]
			);
		}
	}
}

// 16Bit - Portrait Left
static void IGP_PutDisp16Bit_PL() {
	int x, y, i = 0, j = 0;
	WORD *pixel16 = (void *)BitmapPixels;

	for (y = 0; y < VIDEO_BUFFER_HEIGHT; ++y) {
		for (x = 0; x < VIDEO_BUFFER_WIDTH; ++x, i += 3) {
			// j = x * VIDEO_BUFFER_HEIGHT + y;
			// equiv to above, h = 240
			j = (x << 8) - (x << 4) + VIDEO_BUFFER_HEIGHT - 1 - y;

			pixel16[j] = RGB888_TO_WORD_RGB555(
				FrameBuffer[i + 0],
				FrameBuffer[i + 1],
				FrameBuffer[i + 2]
			);
		}
	}
}

// 16Bit - Portrait Right
static void IGP_PutDisp16Bit_PR() {
	int x, y, i = 0, j = 0;
	WORD *pixel16 = (void *)BitmapPixels;

	int t;
	for (y = 0; y < VIDEO_BUFFER_HEIGHT; ++y) {
		for (x = 0; x < VIDEO_BUFFER_WIDTH; ++x, i += 3) {
			t = VIDEO_BUFFER_WIDTH - x - 1;
			j = (t << 8) - (t << 4) + y;

			pixel16[j] = RGB888_TO_WORD_RGB555(
				FrameBuffer[i + 0],
				FrameBuffer[i + 1],
				FrameBuffer[i + 2]
			);
		}
	}
}

// 16Bit - Landscape
static void IGP_PutDisp16Bit_LS() {
	int x, y, i = 0, j = 0;
	WORD *pixel16 = (void *)BitmapPixels;

	for (y = 0; y < VIDEO_BUFFER_HEIGHT; ++y) {
		for (x = 0; x < VIDEO_BUFFER_WIDTH; ++x, i += 3, ++j) {
			pixel16[j] = RGB888_TO_WORD_RGB555(
				FrameBuffer[i + 0],
				FrameBuffer[i + 1],
				FrameBuffer[i + 2]
			);
		}
	}
}

// 32Bit - Portrait Left
static void IGP_PutDisp32Bit_PL() {
	int x, y, i = 0, j = 0;
	BYTE *pixel8 = (void *)BitmapPixels;

	for (y = 0; y < VIDEO_BUFFER_HEIGHT; ++y) {
		for (x = 0; x < VIDEO_BUFFER_WIDTH; ++x, i += 3) {
			// j = (x * VIDEO_BUFFER_HEIGHT + y) * 4;
			// equiv to above, h = 240
			j = ((x << 8) - (x << 4) + VIDEO_BUFFER_HEIGHT - 1 - y) << 2;

			pixel8[j + 0] = FrameBuffer[i + 2];
			pixel8[j + 1] = FrameBuffer[i + 1];
			pixel8[j + 2] = FrameBuffer[i + 0];
		}
	}
}

// 32Bit - Portrait Right
static void IGP_PutDisp32Bit_PR() {
	int x, y, i = 0, j = 0;
	BYTE *pixel8 = (void *)BitmapPixels;

	int t;
	for (y = 0; y < VIDEO_BUFFER_HEIGHT; ++y) {
		for (x = 0; x < VIDEO_BUFFER_WIDTH; ++x, i += 3) {
			t = VIDEO_BUFFER_WIDTH - x - 1; 
			j = ((t << 8) - (t << 4) + y) << 2;

			pixel8[j + 0] = FrameBuffer[i + 2];
			pixel8[j + 1] = FrameBuffer[i + 1];
			pixel8[j + 2] = FrameBuffer[i + 0];
		}
	}
}

// 32Bit - Landscape
static void IGP_PutDisp32Bit_LS() {
	int x, y, i = 0, j = 0;
	BYTE *pixel8 = (void *)BitmapPixels;

	for (y = 0; y < VIDEO_BUFFER_HEIGHT; ++y) {
		for (x = 0; x < VIDEO_BUFFER_WIDTH; ++x, i += 3, j += 4) {
			pixel8[j + 0] = FrameBuffer[i + 2];
			pixel8[j + 1] = FrameBuffer[i + 1];
			pixel8[j + 2] = FrameBuffer[i + 0];
		}
	}
}

// Select Putdisp function
void GP_SelectPutDisp() {
	switch (VideoBitMode) {
		case VIDEO_BUFFER_MODE_2BIT: {
			switch (GP_Config.portraitMode) {
				case PORTRAIT_MODE_LEFT:	GP_Config.fnPutDisp = IGP_PutDisp2Bit_PL;	return;
				case PORTRAIT_MODE_RIGHT:	GP_Config.fnPutDisp = IGP_PutDisp2Bit_PR;	return;
				default:					GP_Config.fnPutDisp = IGP_PutDisp2Bit_LS;	return;
			}
			return;
		}
		case VIDEO_BUFFER_MODE_8BIT: {
			switch (GP_Config.portraitMode) {
				case PORTRAIT_MODE_LEFT:	GP_Config.fnPutDisp = IGP_PutDisp8Bit_PL;	return;
				case PORTRAIT_MODE_RIGHT:	GP_Config.fnPutDisp = IGP_PutDisp8Bit_PR;	return;
				default:					GP_Config.fnPutDisp = IGP_PutDisp8Bit_LS;	return;
			}
			return;
		}
		case VIDEO_BUFFER_MODE_16BIT: {
			switch (GP_Config.portraitMode) {
				case PORTRAIT_MODE_LEFT:	GP_Config.fnPutDisp = IGP_PutDisp16Bit_PL;	return;
				case PORTRAIT_MODE_RIGHT:	GP_Config.fnPutDisp = IGP_PutDisp16Bit_PR;	return;
				default:					GP_Config.fnPutDisp = IGP_PutDisp16Bit_LS;	return;
			}
			return;
		}
		case VIDEO_BUFFER_MODE_32BIT: {
			switch (GP_Config.portraitMode) {
				case PORTRAIT_MODE_LEFT:	GP_Config.fnPutDisp = IGP_PutDisp32Bit_PL;	return;
				case PORTRAIT_MODE_RIGHT:	GP_Config.fnPutDisp = IGP_PutDisp32Bit_PR;	return;
				default:					GP_Config.fnPutDisp = IGP_PutDisp32Bit_LS;	return;
			}
			return;
		}
	}
}

//---------------------------------------------------------------------
//
// * Utils : SetPixel / Line / Draw Text / Draw Image
//
//---------------------------------------------------------------------

void GP_SetPixel(int x, int y, GP_IMAGE_COLOR color) {
	PREPARE_SET_PIXEL_COLOR(color);

	if (x < 0 || x >= VIDEO_BUFFER_WIDTH
	 || y < 0 || y >= VIDEO_BUFFER_HEIGHT) {
		return;
	}

	GP_SP_Param_X = x;
	GP_SP_Param_Y = y;
	IGP_SetPixel();
}

void GP_Line(int x0, int y0, int x1, int y1, GP_IMAGE_COLOR color) {
	int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1; 
	int err = dx + dy, e2;

	PREPARE_SET_PIXEL_COLOR(color);

	for (;;){
		GP_SP_Param_X = x0;
		GP_SP_Param_Y = y0;

		if (GP_SP_Param_X >= 0 && GP_SP_Param_X < VIDEO_BUFFER_WIDTH &&
			GP_SP_Param_Y >= 0 && GP_SP_Param_Y < VIDEO_BUFFER_HEIGHT) {
			IGP_SetPixel();
		}

		if (x0 == x1 && y0 == y1) break;
		e2 = 2 * err;
		if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
		if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
	}
}


void GP_DrawImage(const GP_IMAGE *image, int dx, int dy, const GP_RECT *pRect) {
	int x, y, j, ci;
	GP_RECT rect;
	GP_IMAGE_COLOR color;

	if (!image) return;

	if (pRect) {
		rect.x = pRect->x;
		rect.y = pRect->y;
		rect.w = pRect->w;
		rect.h = pRect->h;
	} else {
		rect.x = 0;
		rect.y = 0;
		rect.w = image->w;
		rect.h = image->h;
	}
	// Start position of font
	j = (image->h - 1 - rect.y - (rect.h - 1)) * image->w + rect.x;
	// Loop from bottom
	for (y = rect.h - 1; y >= 0; --y) {
		GP_SP_Param_Y = y + dy;
		if (GP_SP_Param_Y < 0 || GP_SP_Param_Y >= VIDEO_BUFFER_HEIGHT) {
			j += rect.w;
			continue;
		}
		for (x = 0; x < rect.w; ++x, ++j) {
			ci = image->raw[j];

			if (ci == 0) continue;
			
			GP_SP_Param_X = x + dx;
			if (GP_SP_Param_X < 0 || GP_SP_Param_X >= VIDEO_BUFFER_WIDTH) continue;

			color = image->palette[ci];
			PREPARE_SET_PIXEL_COLOR(color);
			
			IGP_SetPixel();
		}
		// next line
		j += image->w - rect.w;
	}
}

// ASCII Font : 32 characters per row

void GP_DrawText(const BYTE *text, int dx, int dy, GP_IMAGE_COLOR color, int drawShadow, GP_IMAGE_COLOR shadowColor) {
	const GP_IMAGE *image = & imgFont_canele_12;
	GP_RECT rect = { 0, 0, 6, 14 };
	int i, j, x, y;

	if (drawShadow) {
		GP_DrawText(text, dx + 1, dy + 1, shadowColor, 0, 0);
	}

	PREPARE_SET_PIXEL_COLOR(color);

	for (i = 0; text[i]; ++i) {
		int col = text[i] % 32;
		int row = text[i] / 32;
		
		rect.x = col * 6;
		rect.y = row * 14;

		// Start position of font
		j = (image->h - 1 - rect.y - (rect.h - 1)) * image->w + rect.x;
		// Loop from bottom
		for (y = rect.h - 1; y >= 0; --y) {
			GP_SP_Param_Y = y + dy;
			if (GP_SP_Param_Y < 0 || GP_SP_Param_Y >= VIDEO_BUFFER_HEIGHT) {
				j += rect.w;
				continue;
			}
			for (x = 0; x < rect.w; ++x, ++j) {
				if (image->raw[j] == 0) continue;
				GP_SP_Param_X = x + dx;
				if (GP_SP_Param_X < 0 || GP_SP_Param_X >= VIDEO_BUFFER_WIDTH) continue;
				IGP_SetPixel();
			}
			// next line
			j += image->w - rect.w;
		}

		dx += rect.w;
	} 
}

// Font HZK16
// extern const unsigned char HZK12[];

// void GP_DrawTextHZK12(const BYTE *text, int dx, int dy, GP_IMAGE_COLOR color, int drawShadow, GP_IMAGE_COLOR shadowColor) {
// 	int j = 0;
// 	int posX = dx, fx;

// 	if (drawShadow) {
// 		GP_DrawTextHZK12(text, dx + 1, dy + 1, shadowColor, 0, 0);
// 	}

// 	PREPARE_SET_PIXEL_COLOR(color);

// 	while (text[j]) {
// 		BYTE a, b;
// 		const BYTE *hzkBuf;
// 		unsigned int offset;
// 		int k, l, m;
// 		if (text[j] & 0x80) {
// 			a = text[j];
// 			b = text[j + 1];
// 			j++; j++;
// 		}
// 		// ASCII
// 		else {
// 			a = 0xa3;
// 			b = text[j] + 128;
// 			j++;
// 		}
// 		// HZK 16
// 		// offset = (94 * ((unsigned int)a - 0xA0 - 1) + ((unsigned int)b - 0xA0 - 1)) * 32;
// 		// HZK 12
// 		offset = (94 * ((unsigned int)a - 0xA0 - 1) + ((unsigned int)b - 0xA0 - 1)) * 24;
// 		hzkBuf = HZK12 + offset;
// 		for (k = 0; k < 12; k++) {
// 			fx = 0;
// 			GP_SP_Param_Y = dy + k;
// 			if (GP_SP_Param_Y < 0 || GP_SP_Param_Y >= VIDEO_BUFFER_HEIGHT) continue;
// 			for (l = 0; l < 2 && fx < 12; l++) {
// 				for (m = 0; m < 8 && fx < 12; m++, fx++){
// 					int flag = 0x1 & (hzkBuf[(k << 1) + l] >> (7 - m));
// 					if (!flag) continue;
// 					GP_SP_Param_X = posX + (l << 3) + m;
// 					if (GP_SP_Param_X < 0 || GP_SP_Param_X >= VIDEO_BUFFER_WIDTH) continue;
// 					IGP_SetPixel();
// 				}
// 			}
// 		}	
// 		posX += 12;
// 	}
// }
