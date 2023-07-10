#include "window.h"
#include "global.h"
#include "util.h"
#include "resource.h"

Window gMainWindow_s = { 0 };
Window* gMainWindow = &gMainWindow_s;

HWND current_page = NULL;

LRESULT CALLBACK main_wnd_proc(HWND window_handle, UINT message, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK page_wnd_proc(HWND page_handle, UINT message, WPARAM wparam, LPARAM lparam);

void init_window(_In_ HINSTANCE hinstance)
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
	wc.lpszClassName = WNDCLASSNAME;
	wc.lpfnWndProc = main_wnd_proc;
	wc.lpszClassName = MAIN_WINDOW_CLASSNAME;

	if (RegisterClassExW(&wc) == 0)
		error_exit();

	// Calculate size and position of the main window
	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);

	int width = (int)(((float)screen_width / 3.0f) * 2.0f);
	int height = (int)(((float)screen_height / 3.0f) * 2.0f);
	int x = (screen_width - width) / 2;
	int y = (screen_height - height) / 2;

	gMainWindow->handle = CreateWindowExW(0, WNDCLASSNAME, WINDOW_TITLE, WINDOW_STYLE, x, y, width, height, NULL, NULL, NULL, 0);

	if (gMainWindow->handle == NULL)
		error_exit();

	SendMessageW(gMainWindow->handle, WM_INITIALIZED, 0, 0);
}

LRESULT CALLBACK main_wnd_proc(HWND window_handle, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
		case WM_INITIALIZED:
		{
			load_page(IDD_PAGE_1);
			break;
		}
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			break;
		}
		case WM_SIZE:
		{
			gMainWindow->cur_client_rect = get_client_rect(window_handle);
			gMainWindow->cur_window_rect = get_window_rect(window_handle);
		
			if (current_page != NULL)
			{
				SetWindowPos(current_page, HWND_TOPMOST, 0, 0, RECT_WIDTH(gMainWindow->cur_client_rect), RECT_HEIGHT(gMainWindow->cur_client_rect), SWP_NOACTIVATE);
	}

			break;
		}
	}

	return DefWindowProcW(window_handle, message, wparam, lparam);
}

void load_page(_In_ int id)
{
	current_page = CreateDialogW(NULL, MAKEINTRESOURCE(id), gMainWindow->handle, page_wnd_proc);

	if (current_page == NULL)
		error_exit();
}

LRESULT CALLBACK page_wnd_proc(HWND page_handle, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
		case WM_INITDIALOG:
		{
			HBRUSH background_brush = CreateSolidBrush(RGB(255, 0, 0));

			SetClassLongPtr(page_handle, GCLP_HBRBACKGROUND, (LONG_PTR)background_brush);

			DeleteObject(background_brush);

			// Position the page
			SetWindowPos(page_handle, HWND_TOPMOST, 0, 0, RECT_WIDTH(gMainWindow->cur_client_rect), RECT_HEIGHT(gMainWindow->cur_client_rect), SWP_NOACTIVATE);

			break;
		}
	}

	return DefWindowProcW(page_handle, message, wparam, lparam);
}