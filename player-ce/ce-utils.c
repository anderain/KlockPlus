#include <string.h>
#include "ce-utils.h"

char * w2c(const TCHAR *s, char *c) {
    int i;
    for (i = 0; s[i]; ++i) {
        c[i] = (char)s[i];
    }
    c[i] = 0;
    return c;
}

TCHAR * c2w(const char *c, TCHAR *s) {
    int i;
    for (i = 0; c[i]; ++i) {
        s[i] = c[i];
    }
    s[i] = 0;
    return s;
}

BOOL ToogleVisibleTaskbar(BOOL visible) {
#if (IS_WIN_CE)
	HWND hWndTaskbar = FindWindow(_T("HHTaskBar"), NULL);
	if (!visible) {
		SetWindowPos(hWndTaskbar, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
	} else {
		SetWindowPos(hWndTaskbar, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	}
#endif
	return visible;
}

double my_atof(const char *str)
{
  int sign;
  double number = 0.0, power = 1.0;

  while(*str == ' ' || *str == '\n' || *str == '\t' || *str == '\r')
    ++str;

  sign = (*str == '-') ? -1 : 1; // Save the sign

  if(*str == '-' || *str == '+') // Skip the sign
    str++;

  while(*str >= '0' && *str <= '9') // Digits before the decimal point
  {
    number = 10.0 * number + (*str - '0');
    str++;
  }

  if(*str == '.') // Skip the decimal point
    str++;

  while(*str >= '0' && *str <= '9') // Digits after the decimal point
  {
    number = 10.0 * number + (*str - '0');
    power *= 10.0;
    str++;
  }


   return sign * number / power;
}

void my_ftoa(float Value, char* Buffer)
 {
     union
     {
         float f;
     
         struct
         {
             unsigned int    mantissa_lo : 16;
             unsigned int    mantissa_hi : 7;    
             unsigned int     exponent : 8;
             unsigned int     sign : 1;
         };
     } helper;
     
     unsigned long mantissa;
     signed char exponent;
     unsigned int int_part;
     char frac_part[3];
     int i, count = 0;
     
     helper.f = Value;
     //mantissa is LS 23 bits
     mantissa = helper.mantissa_lo;
     mantissa += ((unsigned long) helper.mantissa_hi << 16);
     //add the 24th bit to get 1.mmmm^eeee format
     mantissa += 0x00800000;
     //exponent is biased by 127
     exponent = (signed char) helper.exponent - 127;
     
     //too big to shove into 8 chars
     if (exponent > 18)
     {
         Buffer[0] = 'I';
         Buffer[1] = 'n';
         Buffer[2] = 'f';
         Buffer[3] = '\0';
         return;
     }
     
     //too small to resolve (resolution of 1/8)
     if (exponent < -3)
     {
         Buffer[0] = '0';
         Buffer[1] = '\0';
         return;
     }
     
     count = 0;
     
     //add negative sign (if applicable)
     if (helper.sign)
     {
         Buffer[0] = '-';
         count++;
     }
     
     //get the integer part
     int_part = mantissa >> (23 - exponent);    
     //convert to string
     _itoa(int_part, &Buffer[count], 10);
     
     //find the end of the integer
     for (i = 0; i < 8; i++)
         if (Buffer[i] == '\0')
         {
             count = i;
             break;
         }        
 
     //not enough room in the buffer for the frac part    
     if (count > 5)
         return;
     
     //add the decimal point    
     Buffer[count++] = '.';
     
     //use switch to resolve the fractional part
     switch (0x7 & (mantissa  >> (20 - exponent)))
     {
         case 0:
             frac_part[0] = '0';
             frac_part[1] = '0';
             frac_part[2] = '0';
             break;
         case 1:
             frac_part[0] = '1';
             frac_part[1] = '2';
             frac_part[2] = '5';            
             break;
         case 2:
             frac_part[0] = '2';
             frac_part[1] = '5';
             frac_part[2] = '0';            
             break;
         case 3:
             frac_part[0] = '3';
             frac_part[1] = '7';
             frac_part[2] = '5';            
             break;
         case 4:
             frac_part[0] = '5';
             frac_part[1] = '0';
             frac_part[2] = '0';            
             break;
         case 5:
             frac_part[0] = '6';
             frac_part[1] = '2';
             frac_part[2] = '5';            
             break;
         case 6:
             frac_part[0] = '7';
             frac_part[1] = '5';
             frac_part[2] = '0';            
             break;
         case 7:
             frac_part[0] = '8';
             frac_part[1] = '7';
             frac_part[2] = '5';                    
             break;
     }
     
     //add the fractional part to the output string
     for (i = 0; i < 3; i++)
         if (count < 7)
             Buffer[count++] = frac_part[i];
     
     //make sure the output is terminated
     Buffer[count] = '\0';

	 {
		int j = 0;
		char *cBuf = Buffer;
		for (j = strlen(cBuf) - 1; j > 0 && cBuf[j] == '0'; --j) {
			cBuf[j] = 0;
		}
		if (j >= 1 && cBuf[j] == '.') cBuf[j] = 0;
	 }
 }

long my_atoi(const char* S)
{
    long num = 0;
 
    int i = 0;
 
    // run till the end of the string is reached, or the
    // current character is non-numeric
    while (S[i] && (S[i] >= '0' && S[i] <= '9'))
    {
        num = num * 10 + (S[i] - '0');
        i++;
    }
 
    return num;
}

/* A utility function to reverse a string  */
void itoa_reverse(char str[], int length)
{
    int start = 0;
    int end = length -1;
    while (start < end)
    {
        char t = *(str+start);
		*(str+start) = *(str+end);
		*(str+end) = t;
        start++;
        end--;
    }
}
 
// Implementation of itoa()
char* my_itoa(int num, char* str, int base)
{
    int i = 0;
    BOOL isNegative = FALSE;
 
    /* Handle 0 explicitly, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
 
    // In standard itoa(), negative numbers are handled only with
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = TRUE;
        num = -num;
    }
 
    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }
 
    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';
 
    str[i] = '\0'; // Append string terminator
 
    // Reverse the string
    itoa_reverse(str, i);
 
    return str;
}

#ifdef USE_DEBUG_CONSOLE
#include <stdio.h>
#include <stdarg.h>
#include "resource.h"

#define DEBUG_TEXT_COL  80
#define DEBUG_TEXT_ROW  20

HWND                hwndDebug = NULL;
extern HINSTANCE    hInst;
const TCHAR         DebugWindowClass[] = _T("CTG_DebugWindow");
char                DebugText[DEBUG_TEXT_ROW][DEBUG_TEXT_COL];
int                 DebugTextPos = 0;
int                 DebugTextCount = 0;

void Debug_Redraw(HDC hdc, LPRECT rt) {
    int i, j, t;

    HFONT hFont, hOldFont; 
    hFont = (HFONT)GetStockObject(ANSI_FIXED_FONT); 
    hOldFont = (HFONT)SelectObject(hdc, hFont);

    SetTextColor(hdc, 0xffffff);
    SetBkColor(hdc, 0);

    FillRect(hdc, rt, GetStockObject(BLACK_BRUSH));

    t = DebugTextCount >= DEBUG_TEXT_COL ? DEBUG_TEXT_COL : DebugTextCount;

    j = DebugTextPos - 1;

    while (j < 0) {
        j += DEBUG_TEXT_ROW;
    }

    for (i = 0; i < t; ++i) {
        TextOutA(hdc, 0, i * 16, DebugText[j], strlen(DebugText[j]));
        --j;
        if (j < 0) {
            j = DEBUG_TEXT_ROW - 1;
        }
    }

    SelectObject(hdc, hOldFont); 
}

LRESULT CALLBACK Debug_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_PAINT: {
            RECT rt;
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            GetClientRect(hWnd, &rt);
            Debug_Redraw(hdc, &rt);
            EndPaint(hWnd, &ps);
            break;
        }
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

int Debug_Printf(const char *format, ...) {
   va_list aptr;
   int ret;
   char buf[200];

   va_start(aptr, format);
   ret = vsprintf(buf, format, aptr);
   va_end(aptr);

   Debug_Puts(buf);

   return ret;
}


void Debug_Puts(const char * text) {
    char buf[200];

    sprintf(buf, "[%5d] %s", DebugTextCount, text);

    strncpy(DebugText[DebugTextPos], buf, DEBUG_TEXT_COL - 1);

    DebugTextPos++;
    DebugTextCount++;

    if (DebugTextPos >= DEBUG_TEXT_ROW) DebugTextPos = 0;

    InvalidateRect(hwndDebug, NULL, TRUE);
}

ATOM Debug_RegisterClass() {
    WNDCLASS    wc;

    wc.style            = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc      = (WNDPROC) Debug_WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = hInst;
    wc.hIcon            = LoadIcon(hInst, MAKEINTRESOURCE(IDI_CE_THE_GAME));
    wc.hCursor          = 0;
    wc.hbrBackground    = (HBRUSH) GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName     = 0;
    wc.lpszClassName    = DebugWindowClass;

    return RegisterClass(&wc);
}

void Debug_Open(int nCmdShow) {
    RECT clientRect;
    DWORD windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    int w, h;

    clientRect.left = 0;
    clientRect.top  = 0;
    clientRect.right = 560;
    clientRect.bottom = 16 * 20;

    AdjustWindowRect(&clientRect, windowStyle, FALSE);

    w = clientRect.right - clientRect.left;
    h = clientRect.bottom - clientRect.top;

    Debug_RegisterClass();

    hwndDebug = CreateWindow(DebugWindowClass, _T("CTG LOGS"), WS_VISIBLE,
        0, 0, w, h, NULL, NULL, hInst, NULL);

    ShowWindow(hwndDebug, nCmdShow);
    UpdateWindow(hwndDebug);
    SetWindowPos(hwndDebug, HWND_TOP, 10, 10, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);

    memset(DebugText, 0, DEBUG_TEXT_COL * DEBUG_TEXT_ROW);

    Debug_Puts("CE-the-Game Debug Logs");
    Debug_Puts("Usage : Debug_Puts / Debug_Printf");
}

void Debug_Close() {
    if (hwndDebug) DestroyWindow(hwndDebug);
}

#endif