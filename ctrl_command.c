#include "util.h"
#include "resource.h"
#include "window.h"
#include "global.h"

void show_password_checkbox_click(_In_ HWND checkbox_handle, _In_ HWND parent_handle)
{
	BOOL checked = (SendMessage(checkbox_handle, BM_GETCHECK, 0, 0) == BST_CHECKED);

	HWND textbox_handle = GetDlgItem(parent_handle, IDC_EDIT1);

	if (checked)
	{
		SendMessage(textbox_handle, EM_SETPASSWORDCHAR, L'•', 0);
	}
	else
	{
		SendMessage(textbox_handle, EM_SETPASSWORDCHAR, 0, 0);
	}

	//RedrawWindow(textbox_handle, NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
	redraw_window(textbox_handle);

	SetFocus(textbox_handle);
}

void login_button_click(_In_ HWND button_handle, _In_ HWND parent_handle)
{
	// Get the inputted password from the edit control
	HWND textbox_handle = GetDlgItem(parent_handle, IDC_EDIT1);

	// Copy the password the user typed to the global master_password string
	SendMessage(textbox_handle, WM_GETTEXT, sizeof(wchar_t) * 32, (LPARAM)master_password);

	// Unload the previous page
	unload_current_page();

	// Set the main window style to include maximize box and thickframe
	DWORD main_window_style = get_window_style(gMainWindow->handle);
	main_window_style |= (WS_MAXIMIZEBOX | WS_THICKFRAME);
	set_window_style(gMainWindow->handle, main_window_style);

	// Change position and size of the main window
	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);

	int width = (int)(((float)screen_width / 3.0f) * 2.0f);
	int height = (int)(((float)screen_height / 3.0f) * 2.0f);

	int x = (screen_width - width) / 2;
	int y = (screen_height - height) / 2;

	SetWindowPos(gMainWindow->handle, NULL, x, y, width, height, SWP_NOZORDER | SWP_NOACTIVATE);

	UpdateWindow(gMainWindow->handle);

	// Load in next page
	load_page(IDD_PAGE_2);
}

void new_button_click(_In_ HWND button_handle, _In_ HWND parent_handle)
{
	unload_current_page();

	// Change window size
	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);

	int width = (screen_width / 2) + 16;
	int height = (screen_height / 2) + 39;
	int x = (screen_width - width) / 2;
	int y = (screen_height - height) / 2;

	SetWindowPos(gMainWindow->handle, NULL, x, y, width, height, SWP_NOZORDER | SWP_NOACTIVATE);

	DWORD main_window_style = get_window_style(gMainWindow->handle);
	main_window_style &= ~(WS_MAXIMIZEBOX | WS_THICKFRAME);
	set_window_style(gMainWindow->handle, main_window_style);

	UpdateWindow(gMainWindow->handle);

	load_page(IDD_PAGE_3);
}

void length_slider_on_change(_In_ HWND slider_handle, _In_ HWND parent_handle)
{
	int slider_pos = SendMessageW(slider_handle, TBM_GETPOS, 0, 0);

	HWND slider_display_label = GetDlgItem(parent_handle, IDC_LABEL_LENGTH_NUM);

	wchar_t buffer[4] = { 0 };
	swprintf_s(buffer, 4, L"%d", slider_pos);

	SetWindowTextW(slider_display_label, buffer);

	force_redraw_window(slider_display_label);
}