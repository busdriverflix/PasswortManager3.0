#include <stdarg.h>
#include <wchar.h>

#include "util.h"

int messageboxf(_In_ UINT type, _In_ const wchar_t* const title, _In_ const wchar_t* const _format, ...)
{
    va_list arglist;
    va_start(arglist, _format);

    // Determine the length of the formatted string
    int len = vswprintf(NULL, 0, _format, arglist) + 1; // +1 for the null terminator

    // Allocate memory for the formatted string
    wchar_t* message = (wchar_t*)HEAP_ALLOC(len * sizeof(wchar_t));

    // Format the message using vswprintf and the variable arguments
    vswprintf(message, len, _format, arglist);

    va_end(arglist);

    // Display the message box with the formatted message
    int result = MessageBoxW(NULL, message, title, type);

    // Free the memory allocated for the message buffer
    HEAP_FREE(message);

    return result;
}

void messageboxfOK(_In_ const wchar_t* const title, _In_ const wchar_t* const _format, ...)
{
    va_list arglist;
    va_start(arglist, _format);

    // Determine the length of the formatted string
    int len = vswprintf(NULL, 0, _format, arglist) + 1; // +1 for the null terminator

    // Allocate memory for the formatted string
    wchar_t* message = (wchar_t*)HEAP_ALLOC(len * sizeof(wchar_t));

    // Format the message using vswprintf and the variable arguments
    vswprintf(message, len, _format, arglist);

    va_end(arglist);

    // Display the message box with the formatted message
    MessageBoxW(NULL, message, title, MB_OK);

    // Free the memory allocated for the message buffer
    HEAP_FREE(message);
}

void error_exit(void)
{
    MessageBoxW(NULL, L"Ein Fehler ist aufgetreten.", L"Fehler", MB_OK);
    exit(EXIT_FAILURE);
}

RECT get_window_rect(_In_ HWND window_handle)
{
    RECT wr = { 0 };
    GetWindowRect(window_handle, &wr);
    return wr;
}

RECT get_client_rect(_In_ HWND window_handle)
{
    RECT cr = { 0 };
    GetClientRect(window_handle, &cr);
    return cr;
}