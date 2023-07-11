#include <stdint.h>

#include "window.h"
#include "global.h"
#include "util.h"
#include "resource.h"

Window gMainWindow_s = { 0 };
Window* gMainWindow = &gMainWindow_s;

static struct CurrentProgramPage
{
	HWND handle;
	int num_controls;
	UIControl** controls;

} cur_program_page = { 0 };

LRESULT CALLBACK main_wnd_proc(HWND window_handle, UINT message, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK page_wnd_proc(HWND page_handle, UINT message, WPARAM wparam, LPARAM lparam);
BOOL CALLBACK page_enum_child_proc(HWND ui_handle, LPARAM lparam);

extern void change_window_theme(_In_ HWND handle);

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
	wc.lpszClassName = WNDCLASSNAME;

	if (RegisterClassExW(&wc) == 0)
		error_exit();

	// Calculate size and position of the main window
	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);

	int width = (screen_width / 2) + 16;
	int height = (screen_height / 2) + 39;
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
		
			if (cur_program_page.handle != NULL)
			{
				int parent_width = RECT_WIDTH(gMainWindow->cur_window_rect);
				int parent_height = RECT_HEIGHT(gMainWindow->cur_window_rect);

				if (SetWindowPos(cur_program_page.handle, NULL, 0, 0, parent_width, parent_height, SWP_NOACTIVATE) == 0)
				{
					DWORD error = GetLastError();
					messageboxf(MB_OK, L"Error", L"Windows Error: %d", error);
				}
			}

			break;
		}
		case WM_GETMINMAXINFO:
		{
#define PROVISIONALLY_WIDTH_EXTRA 16 // Don't know yet where the 16 pixels extra and 39 pixels extra are coming from. But they are necessery for this
#define PROVISIONALLY_HEIGHT_EXTRA 39

			LPMINMAXINFO lpMMI = (LPMINMAXINFO)lparam;
			int screen_width = GetSystemMetrics(SM_CXSCREEN);
			int screen_height = GetSystemMetrics(SM_CYSCREEN);

			lpMMI->ptMinTrackSize.x = (screen_width / 2) + PROVISIONALLY_WIDTH_EXTRA;
			lpMMI->ptMinTrackSize.y = (screen_height / 2) + PROVISIONALLY_HEIGHT_EXTRA;
			break;
			break;
		}
	}

	return DefWindowProcW(window_handle, message, wparam, lparam);
}

void load_page(_In_ int id)
{
	cur_program_page.handle = CreateDialogW(NULL, MAKEINTRESOURCE(id), gMainWindow->handle, page_wnd_proc);

	if (cur_program_page.handle == NULL)
		error_exit();

	int num_page_controls = 0;

	HWND hChild = GetWindow(cur_program_page.handle, GW_CHILD);
	while (hChild != NULL)
	{
		num_page_controls++;
		hChild = GetWindow(hChild, GW_HWNDNEXT);
	}

	cur_program_page.controls = HEAP_ALLOCZ(sizeof(UIControl*) * num_page_controls);

	if (cur_program_page.controls == NULL)
	{
		// TODO: Handle error
		error_exit();
	}

	// Custom UIControl struct
	EnumChildWindows(cur_program_page.handle, page_enum_child_proc, 0);

	SendMessageW(cur_program_page.handle, WM_INITIALIZED, 0, 0);
}

static inline void page_on_resize(_In_ HWND page_handle, _In_ LPARAM lparam)
{
	int page_new_width = LOWORD(lparam);
	int page_new_height = HIWORD(lparam);

	for (int i = 0; i < cur_program_page.num_controls; i++)
	{
		UIControl* cur_ctrl = cur_program_page.controls[i];

		int ui_x = (int)((cur_ctrl->x + 1) * 0.5f * page_new_width);
		int ui_y = (int)((1 - cur_ctrl->y) * 0.5f * page_new_height);
		int ui_width = (int)(cur_ctrl->width * 0.5f * page_new_width);
		int ui_height = (int)(cur_ctrl->height * 0.5f * page_new_height);

		SetWindowPos(cur_ctrl->handle, NULL, ui_x, ui_y, ui_width, ui_height, SWP_NOZORDER | SWP_NOACTIVATE);

		/*switch (cur_ctrl->type)
		{
			case UICT_BUTTON:
			{
				change_ctrl_fontsize(cur_ctrl->handle, (int)(((float)ui_height / 3.0f) * 2.0f));
				break;
			}
		}*/
	}
}

LRESULT CALLBACK page_wnd_proc(HWND page_handle, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
		case WM_INITIALIZED:
		{
			// Position the page
			SetWindowPos(page_handle, NULL, 0, 0, RECT_WIDTH(gMainWindow->cur_client_rect), RECT_HEIGHT(gMainWindow->cur_client_rect), SWP_NOACTIVATE);

			break;
		}
		case WM_CTLCOLORDLG:
		{
			return (INT_PTR)CreateSolidBrush(gSettings->background);
		}
		case WM_SIZE:
		{
			if (cur_program_page.num_controls < 1)
				break;

			page_on_resize(page_handle, lparam);
			break;
		}
		case WM_CTLCOLORSTATIC:
		{
			HDC hdcStatic = (HDC)wparam;
			HWND hWndStatic = (HWND)lparam;

			if (get_type_from_hwnd(hWndStatic) == UICT_CHECKBOX)
			{
				SetBkMode(hdcStatic, TRANSPARENT);
				SetTextColor(hdcStatic, gSettings->foreground);

				return (LRESULT)CreateSolidBrush(gSettings->background);
			}

			// Set the desired background color
			SetBkColor(hdcStatic, gSettings->background); // Example: Red background

			// Set the desired font color
			SetTextColor(hdcStatic, gSettings->foreground); // Example: Green font color

			return (LRESULT)GetStockObject(NULL_BRUSH);
		}
		case WM_CTLCOLORBTN:
		{
			HDC hdcButton = (HDC)wparam;
			HWND hWndButton = (HWND)lparam;

			SetBkMode(hdcButton, TRANSPARENT);

			HBRUSH background = CreateSolidBrush(gSettings->background);
			RECT ui_rect = get_client_rect(hWndButton);

			FillRect(hdcButton, &ui_rect, background);

			DeleteObject(background);

			// Return a handle to the background brush
			return (LRESULT)GetStockObject(NULL_BRUSH);
		}
	}

	return DefWindowProcW(page_handle, message, wparam, lparam);
}

// UI controls
BOOL CALLBACK page_enum_child_proc(HWND ui_handle, LPARAM lparam)
{
	UIControl* ctrl = HEAP_ALLOCZ(sizeof(UIControl));

	if (ctrl == NULL)
		return FALSE;

	ctrl->handle = ui_handle;
	ctrl->type = get_type_from_hwnd(ui_handle);
	
	RECT ui_rect = get_window_rect(ui_handle);
	POINT ui_pos = { ui_rect.left, ui_rect.top };
	ScreenToClient(cur_program_page.handle, &ui_pos);

	int16_t ui_x = (int16_t)ui_pos.x, ui_y = (int16_t)ui_pos.y,
		ui_width = (int16_t)(ui_rect.right - ui_rect.left),
		ui_height = (int16_t)(ui_rect.bottom - ui_rect.top);

	RECT parent_rect = get_window_rect(cur_program_page.handle);

	int16_t parent_page_width = (int16_t)RECT_WIDTH(parent_rect);
	int16_t parent_page_height = (int16_t)RECT_HEIGHT(parent_rect);

	// Get normalized size and position
	ctrl->x = (float)(2 * ui_x) / parent_page_width - 1;
	ctrl->y = (float)(1 - (float)(2 * ui_y) / parent_page_height);
	ctrl->width = (float)(2 * ui_width) / parent_page_width;
	ctrl->height = (float)(2 * ui_height) / parent_page_height;
	ctrl->cmd_id = GetDlgCtrlID(ui_handle);

	int size = 0;

	switch (ctrl->type)
	{
		case UICT_BUTTON:
		{
			size = (int)(((float)ui_height / 3.0f) * 2.5f);
			SetClassLongPtr(ui_handle, GCL_HCURSOR, LoadCursorW(NULL, IDC_HAND));
			break;
		}
		case UICT_TEXTBOX:
		case UICT_LABEL:
		{
			if (ctrl->cmd_id == IDC_EDIT1)
				SetFocus(ui_handle);

			size = (int)(((float)ui_height / 3.0f) * 2.5f);
			break;
		}
		case UICT_CHECKBOX:
		{
			change_window_theme(ui_handle);
			(int)(((float)ui_height / 3.0f) * 2.5f);
			break;
		}
		default:
		{
			size = 16;
			break;
		}
	}

	// Change the font
	HFONT font = create_font(gSettings->font, size);

	if (change_ctrl_font(ctrl->handle, font) == FALSE)
	{
		int byebye = 0;
	}

	// Add to the current page's controls array
	cur_program_page.controls[cur_program_page.num_controls++] = ctrl;
}

UICtrlType get_type_from_hwnd(_In_ HWND ui_handle)
{
	if (ui_handle == NULL)
	{
		error_msg(L"Fehler: Ungültiger UI-Handle!");
		return UICT_NONE;
	}

	wchar_t buffer[32] = { 0 };
	GetClassNameW(ui_handle, buffer, 32);

	if (WSTR_EQUALS(buffer, L"STATIC") || WSTR_EQUALS(buffer, L"Static"))
	{
		return UICT_LABEL;
	}
	else if (WSTR_EQUALS(buffer, L"BUTTON") || WSTR_EQUALS(buffer, L"Button"))
	{
		DWORD style = GetWindowLongPtr(ui_handle, GWL_STYLE);

		if (style & BS_CHECKBOX)
		{
			return UICT_CHECKBOX;
		}

		return UICT_BUTTON;
	}
	else if (WSTR_EQUALS(buffer, L"EDIT") || WSTR_EQUALS(buffer, L"Edit"))
	{
		return UICT_TEXTBOX;
	}

	return UICT_NONE;
}