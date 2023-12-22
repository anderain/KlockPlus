#ifndef _PIXEL_H_
#define _PIXEL_H_

extern int		SETPIXEL_x;
extern int		SETPIXEL_y;
extern BYTE		SETPIXEL_cR, SETPIXEL_cG, SETPIXEL_cB;

#define PREPARE_SET_PIXEL_COLOR(color) (						\
			GP_SP_Param_R = (BYTE)(((color) & 0xff0000) >> 16),	\
			GP_SP_Param_G = (BYTE)(((color) & 0x00ff00) >> 8),	\
			GP_SP_Param_B = (BYTE)(((color) & 0x0000ff) >> 0)		\
		)

// Set pixel
void GP_SetPixel();

// Putdisp: Send contents of framebufer to bitmap pixels
// Select putdisp function by bpp / landsacpe or portrait
void GP_SelectPutDisp();

// Params for Set Pixel
extern int 	GP_SP_Param_X;
extern int	GP_SP_Param_Y;
extern int	GP_SP_Param_R;
extern int	GP_SP_Param_G;
extern int	GP_SP_Param_B;

#define _2BIT_GRAY_FAST(r, g, b)		(FAST_GRAY[((r) + (g) + (b)) >> 6])
#define RGB888_TO_2BIT_GRAY(r, g, b)		((((r) + (g) + (b)) / 3) >> 6)
//#define RGB888_TO_2BIT_GRAY(r, g, b)	(_2BIT_GRAY_FAST(r, g, b))

#define RGB888_TO_BYTE_RGB233(r,g,b)	((unsigned char)(((r) >> 6 << 6) | ((g) >> 5 << 3) | ((b) >> 5)))
#define RGB888_TO_WORD_RGB555(r,g,b)	((unsigned short)(((r) >> 3 << 10) | ((g) >> 3 << 5) | ((b) >> 3)))

// #define EMULATE_2BIT					1
// #define FOCE_VIDEO_BUFFER_MODE		16

#define COLOR_TRANSPARENT 0xff00ff

#endif