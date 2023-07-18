#pragma warning(push, 0)
#pragma warning(disable: 4668)
#include <windows.h>
#pragma warning(pop)
#include <Uxtheme.h>

#pragma comment(lib, "uxtheme.lib")

void reset_window_theme(_In_ HWND handle)
{
	SetWindowTheme(handle, L"", L"");
}