#ifndef CTRL_CMD_H
#define CTRL_CMD_H

#include <windows.h>

void show_password_checkbox_click(_In_ HWND checkbox_handle, _In_ HWND parent_handle);
void show_password_checkbox_new_click(_In_ HWND checkbox_handle, _In_ HWND parent_handle);
void login_button_click(_In_ HWND button_handle, _In_ HWND parent_handle);
void new_button_click(_In_ HWND button_handle, _In_ HWND parent_handle);
void length_slider_on_change(_In_ HWND slider_handle, _In_ HWND parent_handle);
void save_button_click(_In_ HWND button_handle, _In_ HWND parent_handle);
void abort_button_click(_In_ HWND button_handle, _In_ HWND parent_handle);

#endif // CTRL_CMD_H