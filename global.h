#ifndef GLOBAL_H
#define GLOBAL_H

#include <windows.h>

typedef struct Settings
{
	COLORREF background, accent, foreground;
	wchar_t font[32];
} Settings;

#endif

extern Settings gSettings;