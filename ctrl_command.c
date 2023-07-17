#include "util.h"
#include "resource.h"
#include "window.h"
#include "global.h"
#include "program.h"

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

void show_password_checkbox_new_click(_In_ HWND checkbox_handle, _In_ HWND parent_handle)
{
	BOOL checked = (SendMessage(checkbox_handle, BM_GETCHECK, 0, 0) == BST_CHECKED);

	HWND passwordbox_handle = GetDlgItem(parent_handle, IDC_TEXTBOX_PASSWORD);

	if (checked)
	{
		SendMessage(passwordbox_handle, EM_SETPASSWORDCHAR, L'•', 0);
	}
	else
	{
		SendMessage(passwordbox_handle, EM_SETPASSWORDCHAR, 0, 0);
	}

	//RedrawWindow(textbox_handle, NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
	redraw_window(passwordbox_handle);
}

void checkbox_cfg_click(_In_ HWND checkbox_handle, _In_ HWND parent_handle, _In_ int cmd_id)
{
	BOOL checked = (SendMessage(checkbox_handle, BM_GETCHECK, 0, 0) == BST_CHECKED);

	switch (cmd_id)
	{
		case IDC_CHECK_CHARS_BIG:
			gSettings->config = checked ? (gSettings->config | CFG_USE_BIG_CHARS) : (gSettings->config & ~CFG_USE_BIG_CHARS);
			break;
		case IDC_CHECK_CHARS_SMALL:
			gSettings->config = checked ? (gSettings->config | CFG_USE_SMALL_CHARS) : (gSettings->config & ~CFG_USE_SMALL_CHARS);
			break;
		case IDC_CHECK_NUMBERS:
			gSettings->config = checked ? (gSettings->config | CFG_USE_NUMBERS) : (gSettings->config & ~CFG_USE_NUMBERS);
			break;
		case IDC_CHECK_SYMBOLS:
			gSettings->config = checked ? (gSettings->config | CFG_USE_SYMBOLS) : (gSettings->config & ~CFG_USE_SYMBOLS);
			break;
	}
}

void login_button_click(_In_ HWND button_handle, _In_ HWND parent_handle)
{
	// Get the inputted password from the edit control
	HWND textbox_handle = GetDlgItem(parent_handle, IDC_EDIT1);

	// Copy the password the user typed to the global master_password string
	SendMessage(textbox_handle, WM_GETTEXT, sizeof(wchar_t) * 32, (LPARAM)master_password);

	// Unload the previous page
	unload_current_page();

	// Reposition window and change window style
	switch_window_position_and_style(WND_POS_TYPE_RESIZE);

	// Load in next page
	load_page(IDD_PAGE_2);
}

void new_button_click(_In_ HWND button_handle, _In_ HWND parent_handle)
{
	unload_current_page();

	// Reposition window and change window style
	switch_window_position_and_style(WND_POS_TYPE_NORESIZE);

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

void save_button_click(_In_ HWND button_handle, _In_ HWND parent_handle)
{
	int x = 0;
}

void abort_button_click(_In_ HWND button_handle, _In_ HWND parent_handle)
{
	unload_current_page();

	// Reposition window and change window style
	switch_window_position_and_style(WND_POS_TYPE_RESIZE);

	// Load in next page
	load_page(IDD_PAGE_2);
}

void generate_button_click(_In_ HWND button_handle, _In_ HWND parent_handle)
{
	if ((gSettings->config & (CFG_USE_BIG_CHARS | CFG_USE_SMALL_CHARS | CFG_USE_NUMBERS | CFG_USE_SYMBOLS)) == 0)
	{
		// None of the specified flags are set, so return
		return;
	}

	HWND slider_handle = GetDlgItem(parent_handle, IDC_LENGTH_SLIDER);
	HWND password_box_handle = GetDlgItem(parent_handle, IDC_TEXTBOX_PASSWORD);

	int length = SendMessageW(slider_handle, TBM_GETPOS, 0, 0);

	wchar_t buffer[31]; // Don't forget null terminator

	generate_password(buffer, length, gSettings->config);

	SetWindowTextW(password_box_handle, buffer);
}