#ifndef CTRL_CMD_H
#define CTRL_CMD_H

#pragma warning(push, 0)
#pragma warning(disable: 4668)
#include <windows.h>
#pragma warning(pop)

void welcome_create_button_click(_In_ HWND button_handle, _In_ HWND parent_handle);
void show_password_checkbox_click(_In_ HWND checkbox_handle, _In_ HWND parent_handle);
void show_password_checkbox_new_click(_In_ HWND checkbox_handle, _In_ HWND parent_handle);
void show_password_checkbox_welcome_click(_In_ HWND checkbox_handle, _In_ HWND parent_handle);
void checkbox_cfg_click(_In_ HWND checkbox_handle, _In_ HWND parent_handle, _In_ int cmd_id);
void login_button_click(_In_ HWND button_handle, _In_ HWND parent_handle);
void new_button_click(_In_ HWND button_handle, _In_ HWND parent_handle);
void length_slider_on_change(_In_ HWND slider_handle, _In_ HWND parent_handle);
void save_button_click(_In_ HWND button_handle, _In_ HWND parent_handle);
void abort_button_click(_In_ HWND button_handle, _In_ HWND parent_handle);
void generate_button_click(_In_ HWND button_handle, _In_ HWND parent_handle);
void combo_box_select(_In_ HWND combo_handle, _In_ HWND parent_handle);

#endif // CTRL_CMD_H