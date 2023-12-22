#ifndef _CE_UTILS_H_
#define _CE_UTILS_H_

#include <windows.h>

char *		w2c			(const TCHAR *s, char *c);
TCHAR *		c2w			(const char *c, TCHAR *s);
double		my_atof		(const char *s);
void		my_ftoa		(float Value, char* Buffer);
char*		my_itoa		(int num, char* str, int base);
long		my_atoi		(const char* S);

BOOL ToogleVisibleTaskbar(BOOL visible);

#if defined(UNICODE) && !defined(_T)
#define _T(x) L##x
#endif

#ifdef VER_PLATFORM_WIN32_CE
#	define IS_WIN_CE	1
#else
#	define IS_WIN_CE	0
#endif

#if !(IS_WIN_CE)
#	define USE_DEBUG_CONSOLE
#endif

#ifdef USE_DEBUG_CONSOLE

void 	Debug_Open		(int nCmdShow);
void 	Debug_Close		();
void 	Debug_Puts		(const char * text);
int		Debug_Printf	(const char *fmt, ...);

#else

#define Debug_Open(nCmdShow)	0
#define Debug_Close()			0
#define Debug_Puts(text)		0
#define Debug_Printf()		0

#endif // USE_DEBUG_CONSOLE

#endif // _UTILS_H_