#ifndef UTIL_H
#define UTIL_H

#include <windows.h>

#define HEAP_ALLOC(size) (HeapAlloc(GetProcessHeap(), 0, size))
#define HEAP_ALLOCZ(size) (HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size))
#define HEAP_REALLOC(memory, newsize) (HeapReAlloc(GetProcessHeap(), 0, memory, newsize))
#define HEAP_FREE(memory) (HeapFree(GetProcessHeap(), 0, memory))

#define RECT_WIDTH(rect) (rect.right - rect.left)
#define RECT_HEIGHT(rect) (rect.bottom - rect.top)

// Functions
int messageboxf(_In_ UINT type, _In_ const wchar_t* const title, _In_ const wchar_t* const _format, ...);
void error_exit(void);

RECT get_window_rect(_In_ HWND window_handle);
RECT get_client_rect(_In_ HWND window_handle);

#endif