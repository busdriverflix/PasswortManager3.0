#ifndef GLOBAL_H
#define GLOBAL_H

#include <windows.h>

#define DBB() __debugbreak()

#define CFG_ASTERISK_PASSWORD	0x01
#define CFG_USE_CHARACTERS		0x02
#define CFG_USE_NUMBERS			0x04
#define CFG_USE_SYMBOLS			0x08

typedef struct Settings
{
	COLORREF background, accent, foreground;

	unsigned int config;

} Settings;

#endif

extern Settings* gSettings;
extern wchar_t master_password[];

extern wchar_t gFont[];