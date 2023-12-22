#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <windows.h>

typedef struct tagDEVICE_INFO {
	OSVERSIONINFO osInfo;
	BITMAP bitmap;
} DEVICE_INFO;

extern DEVICE_INFO DeviceInfo;

void Device_CollectInfo(HDC hdc);
void Device_PopupInfo();

#endif