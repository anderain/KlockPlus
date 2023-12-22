#ifndef _GRAPHICS_DEF_H_
#define _GRAPHICS_DEF_H_

typedef unsigned long GP_IMAGE_COLOR;
typedef unsigned char GP_IMAGE_INDEX;

typedef struct tagSmtImage {
	int w;
	int h;
	int paletteLength;
	const GP_IMAGE_COLOR *palette;
	const GP_IMAGE_INDEX *raw;
} GP_IMAGE, *LPGP_IMAGE;

typedef struct tagSmtRect {
	int x;
	int y;
	int w;
	int h;
} GP_RECT;

#endif
