#include "window.h"
#include "global.h"
#include "util.h"

Window gMainWindow_s = { 0 };
Window* gMainWindow = &gMainWindow_s;

LRESULT CALLBACK main_wnd_proc(HWND window_handle, UINT message, WPARAM wparam, LPARAM lparam);

BOOL window_init(_In_ HINSTANCE hinstance)
{
	WNDCLASSEXW wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.hInstance = hinstance;

#ifdef _DEBUG
	wc.hbrBackground = CreateSolidBrush(RGB(255, 0, 255));
#else
	wc.hbrBackground = CreateSolidBrush(gSettings->background);
#endif

	wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
	// TODO: wc.hIcon
	wc.lpfnWndProc = main_wnd_proc;
	wc.lpszClassName = MAIN_WINDOW_CLASSNAME;

	if (RegisterClassExW(&wc) == 0)
		return FALSE;

	// Calculate size and position of the main window
	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);

	int width = (int)((float)((float)screen_width / 3.0f) * 2.0f); // Two thirds of the screen
	int height = (int)((float)((float)screen_height / 3.0f) * 2.0f);

	int x = (screen_width / 2) - (width / 2);
	int y = (screen_height / 2) - (height / 2);

	gMainWindow->handle = CreateWindowExW(0, wc.lpszClassName, L"Passwort Manager v3.0", MAIN_WINDOW_STYLE, x, y,
		width, height, NULL, NULL, NULL, 0);

	if (gMainWindow == NULL)
		return FALSE;
}

LRESULT CALLBACK main_wnd_proc(HWND window_handle, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
		
	}

	return DefWindowProcW(window_handle, message, wparam, lparam);
}