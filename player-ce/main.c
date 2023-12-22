// * KlockPlusCE
// ------------------------------
// Entry Point
// ------------------------------

#include <windows.h>
#include <stdlib.h>
#include "resource.h"
#include "graphics/graph.h"
#include "ce-utils.h"
#include "device.h"
#include "input.h"
#include "scene/scene.h"

#define APP_NAME "KlockPlusCE"

// Global Variables:
HINSTANCE	hInst;
HWND		hWndMain;
BOOL		quitFlag = FALSE;
DWORD		previousTick;
int			ScreenWidth;
int			ScreenHeight;

// Flush Area
GP_RECT FlushArea = { 0, 0, 0, 0 };

void		(* fnFlushWindow)(HDC, RECT *) = NULL;

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass			(HINSTANCE hInstance, LPTSTR szWindowClass);
BOOL				InitInstance			(HINSTANCE, int);
LRESULT CALLBACK	WndProc					(HWND, UINT, WPARAM, LPARAM);
void				Redraw					();
void				FlushWindowCePortrait		(HDC, LPRECT);
void				FlushWindowCeLandscape	(HDC, LPRECT);
void				FlushWindowWin32			(HDC, LPRECT);

int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
	MSG msg;
	// HACCEL hAccelTable;

	// Initialize random function
	srand(GetTickCount());

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}
	// Initialize Graphics
	else {
		int		videoMode;
		RECT	rt, *pRect = &rt;
		HDC		hdc = GetDC(hWndMain);
		BOOL	portraitMode = PORTRAIT_MODE_NO;

		// Detect portrait mode
		if (IS_WIN_CE && (ScreenHeight > ScreenWidth)) {
			portraitMode = PORTRAIT_MODE_LEFT;
		}
		// Test: force
		// portraitMode = PORTRAIT_MODE_LEFT;
		// portraitMode = PORTRAIT_MODE_RIGHT;

		// Collect device infomation
		// pass hdc for compatible bitmap testing
		Device_CollectInfo(hdc);

		// [DO NOT] Popup device info
		// Device_PopupInfo();

		// Initialize video buffer
		videoMode = GP_Initialize(hdc, VIDEO_BUFFER_WIDTH, VIDEO_BUFFER_HEIGHT, portraitMode);

		// Failed to create video buffer
		if (VIDEO_BUFFER_MODE_FAILED == videoMode) {
			MessageBox(hWndMain, _T("Fail to initialize Video Buffer!"), _T("ERROR"), MB_OK);
			ReleaseDC(hWndMain , hdc);
			return -1;
		}

		// Get area of main window
		GetClientRect(hWndMain, &rt);

		// Choose blt function
		if (IS_WIN_CE) {
			// Portrait
			if (portraitMode != PORTRAIT_MODE_NO) {
				FlushArea.x = (pRect->right - pRect->left - VIDEO_BUFFER_HEIGHT) / 2;
				FlushArea.y = (pRect->bottom - pRect->top - VIDEO_BUFFER_WIDTH) / 2;
				FlushArea.w = VIDEO_BUFFER_HEIGHT;
				FlushArea.h = VIDEO_BUFFER_WIDTH;
				fnFlushWindow = FlushWindowCePortrait;
			}
			// Landspace
			else {
				FlushArea.x = (pRect->right - pRect->left - VIDEO_BUFFER_WIDTH) / 2;
				FlushArea.y = (pRect->bottom - pRect->top - VIDEO_BUFFER_HEIGHT) / 2;
				FlushArea.w = VIDEO_BUFFER_WIDTH;
				FlushArea.h = VIDEO_BUFFER_HEIGHT;
				fnFlushWindow = FlushWindowCeLandscape;
			}
		} 
		// Windows PC
		else {
			fnFlushWindow = FlushWindowWin32;
		}

		// Use white as bg color when video mode is 2bit monochrome
		if (VIDEO_BUFFER_MODE_2BIT == videoMode) {
			FillRect(hdc, &rt, GetStockObject(WHITE_BRUSH));
		}
		// Use black
		else {
			FillRect(hdc, &rt, GetStockObject(BLACK_BRUSH));
		}

		// Debug use white
		// FillRect(hdc, &rt, GetStockObject(WHITE_BRUSH));

		// Finish video buffer
		ReleaseDC(hWndMain , hdc);
	}

	// Clear input state
	Input_ClearAllState();

	// On screen alive in portrait mode
	if (GP_Config.portraitMode) {
		Input_OnScreen_SetVisible(1);
	}

	// hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_CE_THE_GAME);

	previousTick = GetTickCount();

	// Switch to scene
	Scene_To(&SCENE_DEFAULT);

	// Draw first frame
	Redraw(0);

	InvalidateRect(hWndMain, NULL, FALSE);
	UpdateWindow(hWndMain);

	// Main message loop
	while (!quitFlag) 
	{
		DWORD delta = GetTickCount() - previousTick;

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			DispatchMessage(&msg);
		}

#if defined(_WIN32_WCE_EMULATION) || (IS_WIN_CE == 0)
		// Windows PC or Emulation, slow down
		if (GP_Config.ready && delta > 100) {
#else
		if (GP_Config.ready && delta > 0) {
#endif
			previousTick = GetTickCount();
			
			// Update
			if (CurrentScene && CurrentScene->fnUpdate) {
				CurrentScene->fnUpdate(delta);
			}

			// Redraw
			Redraw(delta);

			InvalidateRect(hWndMain, NULL, FALSE);
			UpdateWindow(hWndMain);
		}
	}

	return msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass)
{
	WNDCLASS	wc;

    wc.style			= CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc		= (WNDPROC) WndProc;
    wc.cbClsExtra		= 0;
    wc.cbWndExtra		= 0;
    wc.hInstance		= hInstance;
    wc.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CE_THE_GAME));
    wc.hCursor			= 0;
    wc.hbrBackground	= (HBRUSH) GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName		= 0;
    wc.lpszClassName	= szWindowClass;

	return RegisterClass(&wc);
}

extern BYTE *BitmapPixels, *FrameBuffer;

void Redraw(int delta) {
	GP_Clear();
	if (CurrentScene && CurrentScene->fnRedraw) {
		CurrentScene->fnRedraw(delta); 
	}
	Input_OnScreen_Draw();
	GP_Config.fnPutDisp();
}

void CenterWindowForPC() {
	RECT winRect;

	// Get windows position
	GetWindowRect(hWndMain, &winRect);

	// Move windows to center 
	SetWindowPos(
		hWndMain,
		HWND_TOP,
		(ScreenWidth - (winRect.right - winRect.left)) / 2,
		(ScreenHeight - (winRect.bottom - winRect.top)) / 2,
		0, 0,
		SWP_NOSIZE
	);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	TCHAR	szWindowClass[] = _T("CE-The-Game");
	RECT	clientRect;
	int		winWidth, winHeight;
	DWORD	windowStyle;

	hInst = hInstance;

	// Open debug window
	Debug_Open(nCmdShow);

	MyRegisterClass(hInstance, szWindowClass);

	ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	clientRect.left	= 0;
	clientRect.top	= 0;

#if !(IS_WIN_CE)
	// Note:
	// `AdjustWindowRect` not exist in CE
	//
	clientRect.right	= VIDEO_BUFFER_WIDTH * VIDEO_BUFFER_WIN32_ZOOM;
	clientRect.bottom	= VIDEO_BUFFER_HEIGHT * VIDEO_BUFFER_WIN32_ZOOM;
	windowStyle			= WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	AdjustWindowRect(&clientRect, windowStyle, FALSE);
#else
	clientRect.right	= ScreenWidth;
	clientRect.bottom	= ScreenHeight;
	windowStyle			= WS_VISIBLE;
#endif

	winWidth	= clientRect.right - clientRect.left;
	winHeight	= clientRect.bottom - clientRect.top;

	hWndMain = CreateWindow(szWindowClass, _T("CE The Game"), windowStyle,
		0, 0, winWidth, winHeight, NULL, NULL, hInstance, NULL);

	if (!hWndMain) {
		return FALSE;
	}

#if (IS_WIN_CE)
	ShowWindow(hWndMain, nCmdShow);
	ToogleVisibleTaskbar(FALSE);
	UpdateWindow(hWndMain);
	SetWindowPos(hWndMain, HWND_TOP, 0, 0, ScreenWidth, ScreenHeight, SWP_SHOWWINDOW);
#else
	ShowWindow(hWndMain, nCmdShow);
	UpdateWindow(hWndMain);
	CenterWindowForPC();
#endif

	return TRUE;
}

void FlushWindowCePortrait(HDC hdc, LPRECT pRect) {
	BitBlt(
		hdc, // dest
		FlushArea.x, FlushArea.y,
		FlushArea.w, FlushArea.h,
		GP_Config.hdcBuffer, // src
		0, 0, SRCCOPY // src_x, src_y, mode
	);
}

void FlushWindowCeLandscape(HDC hdc, LPRECT pRect) {
	BitBlt(
		hdc,  // dest
		FlushArea.x, FlushArea.y,
		FlushArea.w, FlushArea.h,
		GP_Config.hdcBuffer, // src
		0, 0, SRCCOPY  // src_x, src_y, mode
	);
}

void FlushWindowWin32(HDC hdc, LPRECT pRect) {
	StretchBlt(
		// dest
		hdc,
		// x, y, w, h
		pRect->left, pRect->top, pRect->right, pRect->bottom,
		// src
		GP_Config.hdcBuffer,
		// src (x y w h)
		0, 0, VIDEO_BUFFER_WIDTH, VIDEO_BUFFER_HEIGHT,
		// mode
		SRCCOPY
	);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	int wmId, wmEvent;
	PAINTSTRUCT ps;

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_LBUTTONDOWN: {
			int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            Input_OnScreen_Detect(x, y);
            // Force exit
            if (Input_IsKeyDown(INPUT_BUTTON_EXIT)) {
            	if (MessageBox(hWnd, _T("Are you sure to exit game?"), _T("Exit"), MB_YESNO) == IDYES) {
	            	DestroyWindow(hWnd);
	            	break;
	            }
	            Input_KeyStateClear(INPUT_BUTTON_EXIT);
            }
			break;
		}
		case WM_LBUTTONUP: {
			Input_ClearAllState();
			break;
		}
		case WM_KEYDOWN:
			// {
			// 	TCHAR wbuf[100];
			// 	wsprintf(wbuf, _T("Key pressd = %d"), wParam);
			// 	MessageBox(NULL, wbuf, _T("key"), 0);
			// }
			// Force Exit
			if (wParam == VK_TAB || wParam == VK_ESCAPE) {
				DestroyWindow(hWnd);
			}
			// Toggle on-screen-button
			else if (wParam == VK_UP || wParam == VK_RETURN || wParam == VK_F23) {
				Input_OnScreen_SetVisible(!Input_OnScreen_GetVisible());
			}
			// Toggle portrait mode
			else if (wParam == VK_DOWN) {
				if (GP_Config.portraitMode == PORTRAIT_MODE_LEFT) {
					GP_Config.portraitMode = PORTRAIT_MODE_RIGHT;
					GP_SelectPutDisp();
				}
				else if (GP_Config.portraitMode == PORTRAIT_MODE_RIGHT) {
					GP_Config.portraitMode = PORTRAIT_MODE_LEFT;
					GP_SelectPutDisp();
				}
			}
			// Game input 
			else if (wParam == 'W') {
				Input_SetKeyDown(INPUT_BUTTON_UP);
			}
			else if (wParam == 'S') {
				Input_SetKeyDown(INPUT_BUTTON_DOWN);
			}
			else if (wParam == 'A') {
				Input_SetKeyDown(INPUT_BUTTON_LEFT);
			}
			else if (wParam == 'D') {
				Input_SetKeyDown(INPUT_BUTTON_RIGHT);
			}
			else if (wParam == 'K') {
				Input_SetKeyDown(INPUT_BUTTON_A);
			}
			else if (wParam == 'L') {
				Input_SetKeyDown(INPUT_BUTTON_B);
			}
			break;
		case WM_KEYUP:
			Input_ClearAllState();
			break;
		case WM_CREATE: 
			break;
		case WM_ERASEBKGND:
			return TRUE;
		case WM_PAINT: {
			RECT rt;
			hdc = BeginPaint(hWnd, &ps);
			GetClientRect(hWnd, &rt);
			if (fnFlushWindow != NULL) fnFlushWindow(hdc, &rt);
			EndPaint(hWnd, &ps);
			break;
		}
		case WM_DESTROY:
			Debug_Close();
			GP_Close();
			PostQuitMessage(0);
			ToogleVisibleTaskbar(TRUE);
			quitFlag = TRUE;
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}
