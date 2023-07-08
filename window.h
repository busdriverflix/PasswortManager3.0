#ifndef WINDOW_H
#define WINDOW_H

#include <windows.h>

#define MAIN_WINDOW_CLASSNAME L"PasswordMainWindowClass"
#define MAIN_WINDOW_STYLE (WS_VISIBLE | WS_OVERLAPPEDWINDOW)

typedef struct Window
{
	HWND handle;

} Window;

BOOL window_init(_In_ HINSTANCE hinstance);

#endif // WINDOW_H

extern Window* gMainWindow;