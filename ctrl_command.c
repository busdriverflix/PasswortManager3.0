#pragma warning(push, 0)
#pragma warning(disable: 4668)
#include <windows.h>
#pragma warning(pop)
#include <CommCtrl.h>
#include <stdio.h>

#include "util.h"
#include "resource.h"
#include "window.h"
#include "global.h"
#include "program.h"
#include "file.h"

void welcome_create_button_click(_In_ HWND button_handle, _In_ HWND parent_handle)
{
	// Get the name and password from the text boxes, check for any invalid inputs and then save them in the profiles file
	wchar_t name[16];
	char password[31];

	HWND name_textbox_handle = GetDlgItem(parent_handle, IDC_WELCOME_NAME);
	HWND password_box_handle = GetDlgItem(parent_handle, IDC_WELCOME_PASSWORD);

	// Retrieve the text from the textboxes
	/*SendMessage(name_textbox_handle, WM_GETTEXT, sizeof(name), (LPARAM)name);
	SendMessageA(password_box_handle, WM_GETTEXT, sizeof(password), (LPARAM)password);*/
	GetWindowTextW(name_textbox_handle, name, 15);
	GetWindowTextA(password_box_handle, password, 30);

	// Create a profile from the inputs
	Profile* profile = HEAP_ALLOCZ(sizeof(Profile));

	if (profile == NULL)
	{
		// TODO: Handle error
		return;
	}

	// Copy name
	wcscpy_s(profile->name, 15, name);

	// Encode the password and save it to the profile
	encrypt_str(password, app_encrypt_decrypt_password, profile->encrypted_password);

	// Create the password file for the profile
	wchar_t* appdata_path = get_roaming_folder_path_wstr();
	wchar_t file_path[MAX_PATH];
	swprintf_s(file_path, MAX_PATH, L"%s\\PasswortManager\\%s.DATA", appdata_path, profile->name);

	// Write the number of passwords (0 at this point)
	FILE* fp = NULL;
	_wfopen_s(&fp, file_path, L"w");
	fwprintf_s(fp, "0");
	fclose(fp);

	// Write the profile to the profiles file
	swprintf_s(file_path, MAX_PATH, L"%s\\PasswortManager\\ProfileData.profiles", appdata_path);
	append_profile_to_file(file_path, profile, TRUE);

	// Add the profile to the global profiles struct
	gProfiles->profiles[0] = profile;
	gProfiles->current_profile = 0;
	gProfiles->default_profile_index = 0;
	gProfiles->num_profiles = 1;

	unload_current_page();

	switch_window_position_and_style(WND_POS_TYPE_RESIZE);

	load_page(IDD_PAGE_2);
}

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

void show_password_checkbox_welcome_click(_In_ HWND checkbox_handle, _In_ HWND parent_handle)
{
	BOOL checked = (SendMessage(checkbox_handle, BM_GETCHECK, 0, 0) == BST_CHECKED);

	HWND passwordbox_handle = GetDlgItem(parent_handle, IDC_WELCOME_PASSWORD);

	if (checked)
	{
		SendMessage(passwordbox_handle, EM_SETPASSWORDCHAR, L'•', 0);
	}
	else
	{
		SendMessage(passwordbox_handle, EM_SETPASSWORDCHAR, 0, 0);
	}

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
	char password_text[31];
	SendMessageA(textbox_handle, WM_GETTEXT, sizeof(wchar_t) * 30, (LPARAM)password_text);

	// Encrypt and compare the encrypted password to the profiles encrypted password
	char encrypted_password[31];
	encrypt_str(password_text, app_encrypt_decrypt_password, encrypted_password);

	if (!STR_EQUALS(encrypted_password, gProfiles->profiles[gProfiles->current_profile]->encrypted_password))
	{
		messageboxf(MB_OK | MB_ICONWARNING, L"Fehler!", L"Das eingegebene Passwort stimmt nicht mit dem Passwort des Profils überein!");
		return;
	}

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

	wchar_t buffer[31]; // Don't forget null terminator (30 characters plus the null character)

	generate_password(buffer, length, gSettings->config);

	SetWindowTextW(password_box_handle, buffer);
}

void combo_box_select(_In_ HWND combo_handle, _In_ HWND parent_handle)
{
	gProfiles->current_profile = SendMessageW(combo_handle, CB_GETCURSEL, 0, 0);

	if (gProfiles->current_profile == -1)
	{
		// TODO: Handle error
	}
}