#include <stdint.h>
#pragma warning(push, 0)
#pragma warning(disable: 4668)
#include <windows.h>
#pragma warning(pop)
#include <CommCtrl.h>

#include "window.h"
#include "ctrl_command.h"
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
	unsigned int id;

} cur_program_page = { 0 };

LRESULT CALLBACK main_wnd_proc(HWND window_handle, UINT message, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK page_wnd_proc(HWND page_handle, UINT message, WPARAM wparam, LPARAM lparam);
BOOL CALLBACK page_enum_child_proc(HWND ui_handle, LPARAM lparam);
LRESULT CALLBACK textbox_wnd_proc(HWND window_handle, UINT message, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

extern void reset_window_theme(_In_ HWND handle);

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
			wchar_t profile_data_file_path[MAX_PATH];
			swprintf_s(profile_data_file_path, MAX_PATH, L"%s\\PasswortManager\\ProfileData.profiles", get_roaming_folder_path_wstr());

			if (file_is_empty(profile_data_file_path))
			{
				load_page(IDD_PAGE_0);
			}
			else
			{
				load_page(IDD_PAGE_1);
			}

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

#ifdef _DEBUG
			wchar_t title_buffer[128] = { 0 };
			swprintf(title_buffer, 128, L"%s - %d x %d", WINDOW_TITLE, RECT_WIDTH(gMainWindow->cur_client_rect), RECT_HEIGHT(gMainWindow->cur_client_rect));

			SetWindowTextW(window_handle, title_buffer);
#endif

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
		}
	}

	return DefWindowProcW(window_handle, message, wparam, lparam);
}

void load_page(_In_ int id)
{
	cur_program_page.id = id;

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

void unload_current_page(void)
{
	// Free the controls
	for (int i = 0; i < cur_program_page.num_controls; i++)
	{
		HEAP_FREE(cur_program_page.controls[i]);
	}

	// Free the control array
	HEAP_FREE(cur_program_page.controls);

	// Destroy page window
	SendMessage(cur_program_page.handle, WM_CLOSE, 0, 0);

	ZeroMemory(&cur_program_page, sizeof(struct CurrentProgramPage));
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

		redraw_window(cur_ctrl->handle);
	}
}

static inline void page_on_command(_In_ HWND page_handle, _In_ WPARAM wparam, _In_ LPARAM lparam)
{
	int ctrl_id = LOWORD(wparam);
	HWND ctrl_handle = (HWND)lparam;

	switch (ctrl_id)
	{
		case IDC_WELCOME_CREATE:
			welcome_create_button_click(ctrl_handle, page_handle);
			break;
		case IDC_CHECK1:
			show_password_checkbox_click(ctrl_handle, page_handle);
			break;
		case IDC_BUTTON1:
			login_button_click(ctrl_handle, page_handle);
			break;
		case IDC_BUTTON_NEW:
			new_button_click(ctrl_handle, page_handle);
			break;
		case IDC_NEW_PASSWORD_VISIBILITY:
			show_password_checkbox_new_click(ctrl_handle, page_handle);
			break;
		case IDC_WELCOME_CHECK_PASSWORD_VISIBILITY:
			show_password_checkbox_welcome_click(ctrl_handle, page_handle);
			break;
		case IDC_CHECK_CHARS_BIG:
		case IDC_CHECK_CHARS_SMALL:
		case IDC_CHECK_NUMBERS:
		case IDC_CHECK_SYMBOLS:
			checkbox_cfg_click(ctrl_handle, page_handle, ctrl_id);
			break;
		case IDC_SAVE_PASSWORD:
			save_button_click(ctrl_handle, page_handle);
			break;
		case IDC_ABORT:
			abort_button_click(ctrl_handle, page_handle);
			break;
		case IDC_BUTTON_GENERATE:
			generate_button_click(ctrl_handle, page_handle);
			break;
		case IDC_COMBO2:
			combo_box_select(ctrl_handle, page_handle);
			break;
		case IDC_EDIT_PROFILES_BUTTON:
			edit_profiles_button_click(ctrl_handle, page_handle);
			break;
	}
}

static inline void page_draw_button(_In_ LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (lpDrawItemStruct == NULL)
	{
		error_msg(L"Fehler: Konnte den Button nicht zeichnen!");
		return;
	}

#pragma warning(push)
#pragma warning(disable: 6011)
	RECT button_rect = lpDrawItemStruct->rcItem;
#pragma warning(pop)

	int button_height = button_rect.bottom - button_rect.top;

	COLORREF button_color = gSettings->accent;
	COLORREF background_color = gSettings->background;
	COLORREF font_color = gSettings->foreground;
	const int corner_radius = 25;


	if (lpDrawItemStruct->itemState & ODS_SELECTED)
	{
		// Increase button color, because button is selected
		button_color = adjust_brightness(button_color, 15, TRUE);

		// Invert font color
		font_color = INVERT_COLOR(button_color);
	}

	// Create brushes
	HBRUSH button_brush = CreateSolidBrush(button_color);
	HBRUSH font_brush = CreateSolidBrush(font_color);
	HBRUSH background_brush = CreateSolidBrush(background_color);

	if (button_brush == NULL || font_brush == NULL || background_brush == NULL)
	{
		error_msg(L"Fehler: Konnte den Button nicht zeichnen!");
		return;
	}

	int fontsize = (button_height / 2);

	HFONT font = create_font(gFont, fontsize);

	if (font == NULL)
	{
		// Cleanup
		DeleteObject(button_brush);

		DeleteObject(font_brush);

		error_msg(L"Fehler: Konnte den Button nicht zeichnen!");
		return;
	}

	if (SelectObject(lpDrawItemStruct->hDC, font) == NULL)
	{
		error_msg(L"Fehler: Konnte den Button nicht zeichnen!");

		goto ERR_CLEANUP_EXIT;
	}

	// Draw button background
	if (FillRect(lpDrawItemStruct->hDC, &button_rect, background_brush) == 0)
	{
		error_msg(L"Fehler: Konnte den Button nicht zeichnen!");

		goto ERR_CLEANUP_EXIT;
	}

	HPEN hNullPen = (HPEN)GetStockObject(NULL_PEN);
	SelectObject(lpDrawItemStruct->hDC, hNullPen);

	SelectObject(lpDrawItemStruct->hDC, button_brush);

	if (RoundRect(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.left, lpDrawItemStruct->rcItem.top,
		lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom, corner_radius, corner_radius) == 0)
	{
		error_msg(L"Fehler: Konnte den Button nicht zeichnen!");

		goto ERR_CLEANUP_EXIT;
	}

	// Draw text
	if (SetBkMode(lpDrawItemStruct->hDC, TRANSPARENT) == 0)
	{
		error_msg(L"Fehler: Konnte den Button nicht zeichnen!");

		goto ERR_CLEANUP_EXIT;
	}

	if (SetTextColor(lpDrawItemStruct->hDC, font_color) == CLR_INVALID)
	{
		error_msg(L"Fehler: Konnte den Button nicht zeichnen!");

		goto ERR_CLEANUP_EXIT;
	}

	wchar_t button_text[128] = { 0 };

	if (GetWindowTextW(lpDrawItemStruct->hwndItem, button_text, 128) == 0)
	{
		error_msg(L"Fehler: Konnte den Button nicht zeichnen!");

		goto ERR_CLEANUP_EXIT;
	}

	if (DrawTextW(lpDrawItemStruct->hDC, button_text, -1, &button_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE) == 0)
	{
		error_msg(L"Fehler: Konnte den Button nicht zeichnen!");

		goto ERR_CLEANUP_EXIT; // For consistency
	}

ERR_CLEANUP_EXIT:

	// Cleanup
	DeleteObject(button_brush);
	DeleteObject(background_brush);
	DeleteObject(font_brush);
	DeleteObject(font);
}

static inline void page_draw_label(_In_ LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	HBRUSH background_brush = CreateSolidBrush(gSettings->accent);
	HBRUSH border_brush = CreateSolidBrush(adjust_brightness(gSettings->accent, 15, TRUE));

	// Border
	FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, border_brush);

	RECT inside_rect = adjust_rect(&lpDrawItemStruct->rcItem, 3, TRUE);

	// Background
	FillRect(lpDrawItemStruct->hDC, &inside_rect, background_brush);

	DeleteObject(background_brush);
	DeleteObject(border_brush);

	// Text
	int label_height = lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top;
	int text_size = label_height - (int)((float)label_height * 0.2f);
	wchar_t text[64] = { 0 };
	GetWindowTextW(lpDrawItemStruct->hwndItem, text, 64);

	HFONT font = CreateFontW(
		text_size,                        // nHeight
		0,                               // nWidth
		0,                               // nEscapement
		0,                               // nOrientation
		FW_NORMAL,                       // nWeight
		FALSE,                           // bItalic
		FALSE,                           // bUnderline
		FALSE,                           // cStrikeOut
		ANSI_CHARSET,                    // nCharSet
		OUT_DEFAULT_PRECIS,              // nOutPrecision
		CLIP_DEFAULT_PRECIS,             // nClipPrecision
		DEFAULT_QUALITY,                 // nQuality
		DEFAULT_PITCH | FF_DONTCARE,     // nPitchAndFamily
		gFont							 // lpszFaceName
	);

	HFONT hOldFont = SelectObject(lpDrawItemStruct->hDC, font);

	SetTextColor(lpDrawItemStruct->hDC, gSettings->foreground);
	SetBkMode(lpDrawItemStruct->hDC, TRANSPARENT);

	DrawTextW(lpDrawItemStruct->hDC, text, -1, &lpDrawItemStruct->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	SelectObject(lpDrawItemStruct->hDC, hOldFont);
	DeleteObject(font);
}

static inline void page_draw_caption(_In_ LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// Background
	HBRUSH background_brush = CreateSolidBrush(gSettings->background);
	FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, background_brush);
	DeleteObject(background_brush);

	// Text
	int label_height = lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top;
	int text_size = label_height - (int)((float)label_height * 0.2f);

	HFONT font = create_font(gFont, text_size);
	wchar_t text[64] = { 0 };
	GetWindowTextW(lpDrawItemStruct->hwndItem, text, 64);

	HFONT hOldFont = SelectObject(lpDrawItemStruct->hDC, font);

	SetTextColor(lpDrawItemStruct->hDC, gSettings->foreground);
	SetBkMode(lpDrawItemStruct->hDC, TRANSPARENT);

	DrawTextW(lpDrawItemStruct->hDC, text, -1, &lpDrawItemStruct->rcItem, DT_VCENTER | DT_LEFT | DT_SINGLELINE);

	// Calculate the position of the underline
	int underlineY = lpDrawItemStruct->rcItem.bottom - 1;

	// Draw the underline manually
	HPEN hUnderlinePen = CreatePen(PS_SOLID, 1, gSettings->foreground);  // Blue underline color
	HPEN hOldPen = SelectObject(lpDrawItemStruct->hDC, hUnderlinePen);

	MoveToEx(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.left, underlineY, NULL);
	LineTo(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.right, underlineY);

	SelectObject(lpDrawItemStruct->hDC, hOldPen);
	SelectObject(lpDrawItemStruct->hDC, hOldFont);
	DeleteObject(font);
	DeleteObject(hUnderlinePen);
}

static inline void page_draw_items(_In_ HWND page_handle, _In_ LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	UICtrlType type = get_type_from_hwnd(lpDrawItemStruct->hwndItem);

	switch (type)
	{
		case UICT_BUTTON:
		{
			page_draw_button(lpDrawItemStruct);
			break;
		}
		case UICT_LABEL:
		{
			if (lpDrawItemStruct->CtlID == IDC_NEW_CAPTION || lpDrawItemStruct->CtlID == IDC_WELCOME_CAPTION || lpDrawItemStruct->CtlID == IDC_EDIT_PROFILES_CAPTION || lpDrawItemStruct->CtlID == IDC_PASSWORDS_CAPTION || lpDrawItemStruct->CtlID == IDC_PROFILE_LABEL)
				page_draw_caption(lpDrawItemStruct);
			else
				page_draw_label(lpDrawItemStruct);

			break;
		}
	}
}

LRESULT CALLBACK page_wnd_proc(HWND page_handle, UINT message, WPARAM wparam, LPARAM lparam)
{
	static HBRUSH checkbox_background = NULL;
	static HBRUSH listbox_background = NULL;

	switch (message)
	{
		case WM_INITDIALOG:
		{
			// Create the background brushes
			checkbox_background = CreateSolidBrush(gSettings->background);
			listbox_background = CreateSolidBrush(gSettings->accent);

			break;
		}
		case WM_INITIALIZED:
		{
			// Position the page
			SetWindowPos(page_handle, NULL, 0, 0, RECT_WIDTH(gMainWindow->cur_client_rect), RECT_HEIGHT(gMainWindow->cur_client_rect), 0);

			for (int i = 0; i < cur_program_page.num_controls; i++)
			{
				force_redraw_window(cur_program_page.controls[i]->handle);
			}

			if (cur_program_page.id == IDD_PAGE_3)
			{
				HWND name_textbox = GetDlgItem(page_handle, IDC_TEXTBOX_NAME);

				SetFocus(name_textbox);
			}
			else if (cur_program_page.id == IDD_PAGE_2)
			{
				// Load passwords from the currently selected profile

			}

			break;
		}
		case WM_CTLCOLORDLG:
		{
			return (LRESULT)checkbox_background;
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

			unsigned int id = GetDlgCtrlID(hWndStatic);

			UICtrlType type = get_type_from_hwnd(hWndStatic);

			if (type == UICT_CHECKBOX)
			{
				SetBkMode(hdcStatic, TRANSPARENT);
				SetTextColor(hdcStatic, gSettings->foreground);

				return (LRESULT)checkbox_background;
			}
			else if (type == UICT_SLIDER)
			{
				SetBkMode(hdcStatic, TRANSPARENT);
				
				return (LRESULT)checkbox_background;
			}
			else if (id == IDC_GROUPBOX)
			{
				SetBkMode(hdcStatic, TRANSPARENT);
				SetTextColor(hdcStatic, gSettings->foreground);  // Red font color

				// Return a handle to the background brush
				return (LRESULT)checkbox_background;

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
		case WM_CTLCOLORLISTBOX:
		{
			return listbox_background;
		}
		case WM_COMMAND:
		{
			page_on_command(page_handle, wparam, lparam);
			break;
		}
		case WM_KEYDOWN:
		{
			if (cur_program_page.id == IDD_PAGE_3)
			{
				if (wparam == VK_ESCAPE)
				{
					abort_button_click(NULL, cur_program_page.handle);
				}

				if (wparam == VK_RETURN)
				{
					save_button_click(NULL, cur_program_page.handle);
				}
			}

			break;
		}
		case WM_HSCROLL:
		{
			length_slider_on_change((HWND)lparam, page_handle);
			break;
		}
		case WM_CLOSE:
		{
			DestroyWindow(page_handle);
			break;
		}
		case WM_DRAWITEM:
		{
			page_draw_items(page_handle, (LPDRAWITEMSTRUCT)lparam);
			break;
		}
		case WM_DESTROY:
		{
			DeleteObject(checkbox_background);
			DeleteObject(listbox_background);

			checkbox_background = NULL;
			listbox_background = NULL;

			break;
		}
	}

	return DefWindowProcW(page_handle, message, wparam, lparam);
}

LRESULT CALLBACK textbox_wnd_proc(HWND window_handle, UINT message, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (message)
	{
		case WM_CHAR:
		{
			int ctrl_id = GetDlgCtrlID(window_handle);

			if (wparam == VK_RETURN)
			{
				// Enter Key pressed

				if (ctrl_id == IDC_EDIT1)
				{
					login_button_click(NULL, cur_program_page.handle);
				}
				else if (ctrl_id == IDC_TEXTBOX_PASSWORD)
				{
					save_button_click(NULL, cur_program_page.handle);
				}
				else if (ctrl_id == IDC_WELCOME_PASSWORD)
				{
					welcome_create_button_click(NULL, cur_program_page.handle);
				}
				else
				{
					HWND currentFocus = GetFocus();
					HWND nextControl = GetNextDlgTabItem(cur_program_page.handle, currentFocus, FALSE);

					// Set the next control as the focus
					SetFocus(nextControl);

					return 0;
				}
			}
			else if (wparam == VK_ESCAPE)
			{
				if (cur_program_page.id == IDD_PAGE_3)
				{
					abort_button_click(NULL, cur_program_page.handle);
				}
			}
			else if (wparam == VK_TAB)
			{
				HWND currentFocus = GetFocus();
				HWND nextControl = GetNextDlgTabItem(cur_program_page.handle, currentFocus, FALSE);

				// Set the next control as the focus
				SetFocus(nextControl);

				return 0;
			}

			break;
		}
	}

	return DefSubclassProc(window_handle, message, wparam, lparam);
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
#ifdef _WIN64
			SetClassLongPtr(ui_handle, GCLP_HCURSOR, LoadCursorW(NULL, IDC_HAND));
#else
			SetClassLongPtr(ui_handle, GCL_HCURSOR, LoadCursorW(NULL, IDC_HAND));
#endif
			break;
		}
		case UICT_TEXTBOX:
		{
			SendMessage(ui_handle, EM_LIMITTEXT, (ctrl->cmd_id == IDC_WELCOME_NAME) ? 15 : 30, 0);

			if (ctrl->cmd_id == IDC_EDIT1)
			{
				SetFocus(ui_handle);

				if (gSettings->config & CFG_ASTERISK_PASSWORD)
				{
					SendMessage(ui_handle, EM_SETPASSWORDCHAR, L'•', 0);
				}
			}
			else if (ctrl->cmd_id == IDC_WELCOME_NAME)
			{
				SetFocus(ui_handle);
			}
			else if (ctrl->cmd_id == IDC_TEXTBOX_PASSWORD || ctrl->cmd_id == IDC_WELCOME_PASSWORD)
			{
				if (gSettings->config & CFG_ASTERISK_PASSWORD)
				{
					SendMessage(ui_handle, EM_SETPASSWORDCHAR, L'•', 0);
				}
			}

			// Subclass it to handle enter key pressed and escape key pressed events
			SetWindowSubclass(ui_handle, textbox_wnd_proc, 0, 0);

			break;
		}
		case UICT_LABEL:
		{
			if (cur_program_page.id == IDD_PAGE_3)
			{
				if (ctrl->cmd_id != IDC_LABEL_LENGTH && ctrl->cmd_id != IDC_LABEL_LENGTH_NUM)
				{
					reset_window_theme(ui_handle);

					set_window_style(ui_handle, (WS_CHILD | WS_VISIBLE | SS_OWNERDRAW));
				}
			}
			else if (ctrl->cmd_id == IDC_PASSWORDS_CAPTION || ctrl->cmd_id == IDC_WELCOME_CAPTION || ctrl->cmd_id == IDC_EDIT_PROFILES_CAPTION || ctrl->cmd_id == IDC_PROFILE_LABEL || cur_program_page.id == IDD_PAGE_0)
			{
				set_window_style(ui_handle, (WS_CHILD | WS_VISIBLE | SS_OWNERDRAW));
			}			

			size = (int)(((float)ui_height / 3.0f) * 2.5f);
			break;
		}
		case UICT_CHECKBOX:
		{
			reset_window_theme(ui_handle);

			if (ctrl->cmd_id == IDC_CHECK1 || ctrl->cmd_id == IDC_NEW_PASSWORD_VISIBILITY || ctrl->cmd_id == IDC_WELCOME_CHECK_PASSWORD_VISIBILITY)
			{
				if (gSettings->config & CFG_ASTERISK_PASSWORD)
				{
					set_checkbox(ui_handle, BST_CHECKED);
				}
			}
			else if (ctrl->cmd_id == IDC_CHECK_CHARS_BIG)
			{
				if (gSettings->config & CFG_USE_BIG_CHARS)
				{
					set_checkbox(ui_handle, BST_CHECKED);
				}
			}
			else if (ctrl->cmd_id == IDC_CHECK_CHARS_SMALL)
			{
				if (gSettings->config & CFG_USE_SMALL_CHARS)
				{
					set_checkbox(ui_handle, BST_CHECKED);
				}
			}
			else if (ctrl->cmd_id == IDC_CHECK_NUMBERS)
			{
				if (gSettings->config & CFG_USE_NUMBERS)
				{
					set_checkbox(ui_handle, BST_CHECKED);
				}
			}
			else if (ctrl->cmd_id == IDC_CHECK_SYMBOLS)
			{
				if (gSettings->config & CFG_USE_SYMBOLS)
				{
					set_checkbox(ui_handle, BST_CHECKED);
				}
			}

			size = ui_height;
			break;
		}
		case UICT_GROUPBOX:
		{
			reset_window_theme(ui_handle);
			break;
		}
		case UICT_SLIDER:
		{
			SendMessageW(ui_handle, TBM_SETRANGE, TRUE, MAKELPARAM(4, 30));
			break;
		}
		case UICT_COMBOBOX:
		{
			for (int i = 0; i < gProfiles->num_profiles; i++)
			{
				SendMessageW(ui_handle, CB_ADDSTRING, 0, (LPARAM)gProfiles->profiles[i]->name);
			}

			// Set cursel to default profile
			SendMessageW(ui_handle, CB_SETCURSEL, (WPARAM)gProfiles->default_profile_index, 0);

			break;
		}
		default:
		{
			size = 16;
			break;
		}
	}

	if (ctrl->cmd_id != IDC_LABEL_LENGTH && ctrl->cmd_id != IDC_LABEL_LENGTH_NUM && ctrl->type != UICT_COMBOBOX)
	{
		HFONT font = create_font(gFont, size);

		if (change_ctrl_font(ctrl->handle, font) == FALSE)
		{
			// TODO: Handle error
		}
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
		DWORD button_type = GetWindowLongPtr(ui_handle, GWL_STYLE) & BS_TYPEMASK;

		if (button_type == BS_CHECKBOX || button_type == BS_AUTOCHECKBOX || button_type == BS_3STATE)
			return UICT_CHECKBOX;
		else if (button_type == BS_GROUPBOX)
			return UICT_GROUPBOX;

		return UICT_BUTTON;
	}
	else if (WSTR_EQUALS(buffer, L"EDIT") || WSTR_EQUALS(buffer, L"Edit"))
	{
		return UICT_TEXTBOX;
	}
	else if (WSTR_EQUALS(buffer, L"msctls_trackbar32"))
	{
		return UICT_SLIDER;
	}
	else if (WSTR_EQUALS(buffer, WC_COMBOBOXW))
	{
		return UICT_COMBOBOX;
	}

	return UICT_NONE;
}