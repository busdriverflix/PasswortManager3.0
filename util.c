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

void error_msg(_In_ const wchar_t* const message)
{
    messageboxf(MB_OK | MB_ICONWARNING, L"Fehler!", message);
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

BOOL change_ctrl_fontsize(_In_ HWND control_handle, _In_ int new_size)
{
    // Get the current font of the control
    HFONT hFont = (HFONT)SendMessage(control_handle, WM_GETFONT, 0, 0);
    if (hFont == NULL)
    {
        // Error retrieving the current font
        return FALSE;
    }

    // Create a new font with the desired size
    LOGFONT lf = { 0 };
    if (GetObject(hFont, sizeof(LOGFONT), &lf) == 0)
    {
        // Error retrieving information about the current font
        return FALSE;
    }
    lf.lfHeight = new_size; // Specify the desired font size

    HFONT hNewFont = CreateFontIndirect(&lf);
    if (hNewFont == NULL)
    {
        // Error creating the new font
        return FALSE;
    }

    // Set the new font for the control
    if (SendMessage(control_handle, WM_SETFONT, (WPARAM)hNewFont, TRUE) == 0)
    {
        // Error setting the new font for the control
        DeleteObject(hNewFont);
        return FALSE;
    }

    // Delete the previous font
    if (!DeleteObject(hFont))
    {
        // Error deleting the previous font
        return FALSE;
    }

    // Font size change successful
    return TRUE;
}

BOOL change_ctrl_font(_In_ HWND control_handle, _In_ HFONT new_font)
{
    // Set the new font for the control
    if (SendMessageW(control_handle, WM_SETFONT, (WPARAM)new_font, MAKELPARAM(TRUE, 0)) == 0)
        return FALSE;

    // Font change successful
    return TRUE;
}

HFONT create_font(_In_ const wchar_t* fontname, _In_ int font_size)
{
    HFONT font = CreateFontW(
        font_size,                        // nHeight
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
        fontname                         // lpszFaceName
    );

    return font;
}

HFONT get_font_from_ctrl(_In_ HWND ctrl_handle)
{
    return (HFONT)SendMessageW(ctrl_handle, WM_GETFONT, 0, 0);
}

void redraw_window(_In_ HWND window_handle)
{
    RedrawWindow(window_handle, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}

void force_redraw_window(_In_ HWND window_handle)
{
    ShowWindow(window_handle, SW_HIDE);
    ShowWindow(window_handle, SW_SHOW);
}