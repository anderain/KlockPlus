#ifndef _RTC_H_
#define _RTC_H_

#define SCA 0xD201D002
#define SCB 0x422B0009
#define SCE 0x80010070

typedef int (*pGetTicks)(void);
const static unsigned int sc003B[] = { SCA, SCB, SCE, 0x03B };
#define RTC_GetTicks (*(pGetTicks)sc003B)

typedef void (*pSetDateTime)(unsigned char *timestr);
const static unsigned int sc023E[] = { SCA, SCB, SCE, 0x23E };
#define RTC_SetDateTime (*(pSetDateTime)sc023E)

typedef void (*pGetTime)(unsigned int* hour, unsigned int* minute, unsigned int* second, unsigned int* millisecond);
const static unsigned int sc03A[] = { SCA, SCB, SCE, 0x03A };
#define RTC_GetTime (*(pGetTime)sc03A)

#endif