#ifndef WINDOW_H
#define WINDOW_H

#include <windows.h>

#define APP_VERSION L"v3.0"

#ifdef _DEBUG
	#ifdef _WIN64
		#define WINDOW_TITLE L"PasswortManager - Beta "APP_VERSION L" Debug x64"
	#else
		#define WINDOW_TITLE L"PasswortManager - Beta "APP_VERSION L" Debug x86"
	#endif
#else
	#ifdef _WIN64
		#define WINDOW_TITLE L"PasswortManager - Beta "APP_VERSION L" Release x64"
	#else
		#define WINDOW_TITLE L"PasswortManager - Beta "APP_VERSION L" Release x86"
	#endif
#endif

#define WNDCLASSNAME L"PasswortManagerWndClass"
#define WINDOW_STYLE (WS_VISIBLE | WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX)

#define WM_INITIALIZED (WM_USER + 1)

#define MAX_PASSWORD_COUNT 32

// Trackbar defines (Slider control)
#define TBM_GETPOS				1024
#define TBM_GETRANGEMIN			1026
#define TBM_GETRANGEMAX			1027
#define TBM_SETPOS				1029
#define TBM_SETRANGE			1030
#define TBM_SETRANGEMIN			1031
#define TBM_SETRANGEMAX			1032
#define TBM_SETTICFREQ			1033
#define TBM_SETSEL				1034
#define TBM_SETSELSTART			1035
#define TBM_SETSELEND			1036
#define TBM_CLEARSEL			1040
#define TBM_SETTOOLTIPS			1041
#define TBM_SETTIPSIDE			1042
#define TBM_SETBUDDY			1043
#define TBM_GETBUDDY			1044
#define TBM_SETUNICODEFORMAT	1091

typedef struct Window
{
	HWND handle;
	RECT cur_client_rect;
	RECT cur_window_rect;
} Window;

// UI controls
typedef enum UICtrlType
{
	UICT_NONE,
	UICT_LABEL,
	UICT_TEXTBOX,
	UICT_BUTTON,
	UICT_CHECKBOX,
	UICT_GROUPBOX,
	UICT_SLIDER

} UICtrlType;

typedef struct UIControl
{
	HWND handle;
	UICtrlType type;
	unsigned int cmd_id;

	// Normalized size and position
	float x, y, width, height;

} UIControl;

void init_window(_In_ HINSTANCE hinstance);
void load_page(_In_ int id);
void unload_current_page(void);

UICtrlType get_type_from_hwnd(_In_ HWND ui_handle);

#endif // WINDOW_H

extern Window* gMainWindow;