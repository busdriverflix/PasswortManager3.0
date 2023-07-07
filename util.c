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

void error_exit(void)
{
    MessageBoxW(NULL, L"Ein Fehler ist aufgetreten.", L"Fehler", MB_OK);
    exit(EXIT_FAILURE);
}