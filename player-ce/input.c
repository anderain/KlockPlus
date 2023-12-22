#include <stdlib.h>
#include "graphics/graph.h"
#include "input.h"
#include "ce-utils.h"

int Input_KeyState[INPUT_BUTTON_EXIT];

void Input_ClearAllState() {
	memset(Input_KeyState, 0x0, sizeof(Input_KeyState));
}

//-------------------------------------------
//
// On screen Pad
//
//-------------------------------------------

static int OnScreenPadVisible = 0;

typedef struct tagON_SCREEN_PAD{
	int x;
	int y;
	const GP_IMAGE *image;
	int key;
} ON_SCREEN_PAD;

static const ON_SCREEN_PAD OnScreenPadList[] = {
	{ 8,	200, 	&imgOsb_a,				INPUT_BUTTON_A		},
	{ 48,	184, 	&imgOsb_b,				INPUT_BUTTON_B		},
	{ 280,	180, 	&imgOsb_arrow_right,	INPUT_BUTTON_RIGHT 	},
	{ 240,	200, 	&imgOsb_arrow_down,		INPUT_BUTTON_DOWN	},
	{ 240,	160, 	&imgOsb_arrow_up,		INPUT_BUTTON_UP		},
	{ 200,	180, 	&imgOsb_arrow_left,		INPUT_BUTTON_LEFT	},
	{ 296,	8, 		&imgOsb_close,			INPUT_BUTTON_EXIT	}
};

static const int OnScreenPadListLengh = sizeof(OnScreenPadList) / sizeof(OnScreenPadList[0]);

int Input_OnScreen_GetVisible() {
	return OnScreenPadVisible;
}

void Input_OnScreen_SetVisible(int visible) {
	OnScreenPadVisible = visible;
}

void Input_OnScreen_Draw() {
	int i;

	if (!OnScreenPadVisible) return;

	for (i = 0; i < OnScreenPadListLengh; ++i) {
		const ON_SCREEN_PAD *pad = &OnScreenPadList[i];
		if (Input_IsKeyDown(pad->key)) {
			GP_DrawImage(pad->image, pad->x + 2, pad->y + 2, NULL);
		} else {
			GP_DrawImage(pad->image, pad->x, pad->y, NULL);
		}
		// GP_Line(pad->x, pad->y, pad->x + pad->image->w, pad->y, 0);
		// GP_Line(pad->x + pad->image->w, pad->y, pad->x + pad->image->w, pad->y + pad->image->h, 0);
		// GP_Line(pad->x + pad->image->w, pad->y + pad->image->h, pad->x, pad->y + pad->image->h, 0);
		// GP_Line(pad->x, pad->y + pad->image->h, pad->x, pad->y , 0);
	}
}

// Flush area for touch event

extern GP_RECT FlushArea;

// Return True if close button pressed

void Input_OnScreen_Detect(int x, int y) {
	int translatedX;
	int translatedY;
	int i;

	if (!OnScreenPadVisible) return;

	switch (GP_Config.portraitMode) {
		case PORTRAIT_MODE_LEFT:
			translatedX = y - FlushArea.x;
			translatedY = VIDEO_BUFFER_HEIGHT - 1 - (x - FlushArea.x);
			break;
		case PORTRAIT_MODE_RIGHT:
			translatedX = VIDEO_BUFFER_WIDTH - (y - FlushArea.x);
			translatedY = x - FlushArea.x;
			break;
		default:
			translatedX = x - FlushArea.x;
			translatedY = y - FlushArea.y;
			break;
	}
	
// Framebuffer zoom in PC
#if !(IS_WIN_CE)
	translatedX /= VIDEO_BUFFER_WIN32_ZOOM;
	translatedY /= VIDEO_BUFFER_WIN32_ZOOM;
#endif

	if (translatedX < 0 || translatedX >= VIDEO_BUFFER_WIDTH ||
		translatedY < 0 || translatedY >= VIDEO_BUFFER_HEIGHT ) {
		return;
	}

	for (i = 0; i < OnScreenPadListLengh; ++i) {
		const ON_SCREEN_PAD *pad = &OnScreenPadList[i];
		if (translatedX > pad->x && translatedX < pad->x + pad->image->w &&
			translatedY > pad->y && translatedY < pad->y + pad->image->h) {
			// TODO: control here
			Input_KeyState[pad->key] = 1;
			return;
		}
	}
}