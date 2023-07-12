#include "util.h"
#include "resource.h"

#define EDITBOX_WS (WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT)

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