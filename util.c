#include <stdarg.h>
#include <wchar.h>
#include <shlobj.h>

#include "util.h"
#include "window.h"

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

void invalidate_window(_In_ HWND window_handle)
{
    RECT wnd_rect = get_window_rect(window_handle);

    InvalidateRect(window_handle, &wnd_rect, TRUE);
}

void force_redraw_window(_In_ HWND window_handle)
{
    ShowWindow(window_handle, SW_HIDE);
    ShowWindow(window_handle, SW_SHOW);
}

RECT adjust_rect(_In_ RECT* input, _In_ int amount_in_pixels, _In_ BOOL inside)
{
    RECT rect = *input;
    
    if (inside)
    {
        rect.left += amount_in_pixels;
        rect.top += amount_in_pixels;
        rect.right -= amount_in_pixels;
        rect.bottom -= amount_in_pixels;
    }
    else
    {
        rect.left -= amount_in_pixels;
        rect.top -= amount_in_pixels;
        rect.right += amount_in_pixels;
        rect.bottom += amount_in_pixels;
    }

    return rect;
}

// Color
COLORREF adjust_brightness(_In_ COLORREF color, _In_ int amount, _In_ BOOL increase)
{
    int red = GetRValue(color);
    int green = GetGValue(color);
    int blue = GetBValue(color);

    if (increase)
    {
        red = min(255, red + amount);
        green = min(255, green + amount);
        blue = min(255, blue + amount);
    }
    else
    {
        red = max(0, red - amount);
        green = max(0, green - amount);
        blue = max(0, blue - amount);
    }

    return RGB(red, green, blue);
}

void switch_window_position_and_style(_In_ unsigned int type)
{
    // Set the main window style to include maximize box and thickframe
    DWORD main_window_style = get_window_style(gMainWindow->handle);

    // Change position and size of the main window
    int screen_width = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);

    int width = 0;
    int height = 0;

    if (type == WND_POS_TYPE_RESIZE || (type != WND_POS_TYPE_RESIZE && type != WND_POS_TYPE_NORESIZE))
    {
        main_window_style |= (WS_MAXIMIZEBOX | WS_THICKFRAME);
        width = (int)(((float)screen_width / 3.0f) * 2.0f);
        height = (int)(((float)screen_height / 3.0f) * 2.0f);
    }
    else if (type == WND_POS_TYPE_NORESIZE)
    {
        main_window_style &= ~(WS_MAXIMIZEBOX | WS_THICKFRAME | WS_MAXIMIZE);
        width = (screen_width / 2) + 16;
        height = (screen_height / 2) + 39;
    }

    set_window_style(gMainWindow->handle, main_window_style);

    int x = (screen_width - width) / 2;
    int y = (screen_height - height) / 2;

    SetWindowPos(gMainWindow->handle, NULL, x, y, width, height, SWP_NOZORDER | SWP_NOACTIVATE);

    UpdateWindow(gMainWindow->handle);
}

wchar_t* get_roaming_folder_path(void)
{
    wchar_t* path = NULL;

    // Get the path to the Roaming folder
    HRESULT result = SHGetKnownFolderPath(&FOLDERID_RoamingAppData, 0, NULL, &path);
    if (SUCCEEDED(result))
    {
        return path;
    }

    return NULL;
}

BOOL create_folder(_In_ const wchar_t* destination_path)
{
    if (CreateDirectoryW(destination_path, NULL) == TRUE)
        return TRUE;
    else
    {
        if (GetLastError() == ERROR_ALREADY_EXISTS)
            return TRUE;
        else
            return FALSE;
    }
}

BOOL file_is_empty(_In_ const wchar_t* file_path)
{
    HANDLE file = CreateFileW(file_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (file != INVALID_HANDLE_VALUE)
    {
        DWORD file_size = GetFileSize(file, NULL);
        CloseHandle(file);

        return (file_size == 0);
    }

    // Failed to open the file
    return FALSE;
}