#include <windows.h>
#include <Uxtheme.h>

#pragma comment(lib, "uxtheme.lib")

void change_window_theme(_In_ HWND handle)
{
	SetWindowTheme(handle, L"", L"");
}