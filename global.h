#ifndef GLOBAL_H
#define GLOBAL_H

#include <windows.h>

#define DBB() __debugbreak()

typedef struct Settings
{
	COLORREF background, accent, foreground;
	wchar_t font[32];

	BOOL asterisk_password;

} Settings;

#endif

extern Settings* gSettings;