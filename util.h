#ifndef UTIL_H
#define UTIL_H

#include <windows.h>
#include <wchar.h>

#define HEAP_ALLOC(size) (HeapAlloc(GetProcessHeap(), 0, size))
#define HEAP_ALLOCZ(size) (HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size))
#define HEAP_REALLOC(memory, newsize) (HeapReAlloc(GetProcessHeap(), 0, memory, newsize))
#define HEAP_FREE(memory) (HeapFree(GetProcessHeap(), 0, memory))

#define RECT_WIDTH(rect) (rect.right - rect.left)
#define RECT_HEIGHT(rect) (rect.bottom - rect.top)

#define WSTR_EQUALS(str1, str2) (wcscmp(str1, str2) == 0)

#define get_window_style(window_handle) (GetWindowLongPtr(window_handle, GWL_STYLE))
#define get_class_style(window_handle) (GetClassLongPtr(window_handle, GCL_STYLE))

#define set_window_style(window_handle, new_style) (SetWindowLongPtr(window_handle, GWL_STYLE, new_style))
#define set_class_style(window_handle, new_style) (SetClassLongPtr(window_handle, GCL_STYLE, new_style))

// Functions
int messageboxf(_In_ UINT type, _In_ const wchar_t* const title, _In_ const wchar_t* const _format, ...);
void messageboxfOK(_In_ const wchar_t* const title, _In_ const wchar_t* const _format, ...);
void error_exit(void);
void error_msg(_In_ const wchar_t* const message);

RECT get_window_rect(_In_ HWND window_handle);
RECT get_client_rect(_In_ HWND window_handle);

BOOL change_ctrl_fontsize(_In_ HWND control_handle, _In_ int new_size);
BOOL change_ctrl_font(_In_ HWND control_handle, _In_ HFONT new_font);
HFONT create_font(_In_ const wchar_t* fontname, _In_ int font_size);
HFONT get_font_from_ctrl(_In_ HWND ctrl_handle);

void redraw_window(_In_ HWND window_handle);
void invalidate_window(_In_ HWND window_handle);
void force_redraw_window(_In_ HWND window_handle);

RECT adjust_rect(_In_ RECT* input, _In_ int amount_in_pixels, _In_ BOOL inside);

// Color
COLORREF adjust_brightness(_In_ COLORREF color, _In_ int amount, _In_ BOOL increase);
#define INVERT_COLOR(color) ((~color) & 0xFFFFFFFF)

#endif