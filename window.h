#ifndef WINDOW_H
#define WINDOW_H

#include <windows.h>

#define APP_VERSION L"v3.0"

#ifdef _DEBUG
	#ifdef _WIN64
		#define WINDOW_TITLE L"PasswortManager - Beta "APP_VERSION L" Debug x64"
	#else
		#define WINDOW_TITLE L"PasswortManager - Beta "APP_VERSION L" Debug x86"
	#endif
#else
	#ifdef _WIN64
		#define WINDOW_TITLE L"PasswortManager - Beta "APP_VERSION L" Release x64"
	#else
		#define WINDOW_TITLE L"PasswortManager - Beta "APP_VERSION L" Release x86"
	#endif
#endif

#define WNDCLASSNAME L"PasswortManagerWndClass"
#define WINDOW_STYLE (WS_VISIBLE | WS_OVERLAPPEDWINDOW)

#define WM_INITIALIZED (WM_USER + 1)

typedef struct Window
{
	HWND handle;
	RECT cur_client_rect;
	RECT cur_window_rect;
} Window;

void init_window(_In_ HINSTANCE hinstance);
void load_page(_In_ int id);

#endif // WINDOW_H

extern Window* gMainWindow;