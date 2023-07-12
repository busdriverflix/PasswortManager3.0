#include <windows.h>
#include <commctrl.h>
#include <shlwapi.h>
#include <wchar.h>
#include "global.h"
#include "util.h"
#include "file.h"
#include "window.h"

Settings gSettings_s = { 0 };
Settings* gSettings = &gSettings_s;

wchar_t master_password[32] = { 0 };

// Function prototypes
void setup(void);
inline void load_settings(void);

int WINAPI WinMain(_In_ HINSTANCE hinstance, _In_opt_ HINSTANCE hprevinstance, _In_ LPWSTR commandline, _In_ int showcmd)
{
	UNREFERENCED_PARAMETER(hprevinstance);
	UNREFERENCED_PARAMETER(commandline);
	UNREFERENCED_PARAMETER(showcmd);

	setup();

	init_window(hinstance);

	MSG message = { 0 };

	while (GetMessageW(&message, gMainWindow->handle, 0, 0) > 0)
	{
		TranslateMessage(&message);
		DispatchMessageW(&message);
	}

	UnregisterClassW(WNDCLASSNAME, NULL);

	return 0;
}

void setup(void)
{
	load_settings();

	// Init common controls
	INITCOMMONCONTROLSEX icex = { 0 };
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_STANDARD_CLASSES | ICC_BAR_CLASSES | ICC_COOL_CLASSES;
	InitCommonControlsEx(&icex);
}

inline void load_settings(void)
{
	// Get file path of executable
	wchar_t exepath[MAX_PATH];
	GetModuleFileNameW(NULL, exepath, MAX_PATH);
	PathRemoveFileSpecW(exepath);

	size_t settings_path_count = wcslen(exepath) + wcslen(L"\\settings.ini") + 1;

	wchar_t* settings_path = (wchar_t*)HEAP_ALLOC(settings_path_count * sizeof(wchar_t));

	if (settings_path == NULL)
		error_exit();

	swprintf_s(settings_path, settings_path_count, L"%s\\settings.ini", exepath);

	if (!PathFileExistsW(settings_path))
	{
		// If the settings file doesn't exist, create one
		Settings default_settings =
		{
			.background = RGB(36, 42, 64),
			.accent = RGB(51, 59, 90),
			.foreground = RGB(255, 255, 255),
			.font = L"Calibri",
			.asterisk_password = TRUE
		};

		if (write_settings_to_file(&default_settings, settings_path) == FALSE)
			error_exit();

		*gSettings = default_settings;

		HEAP_FREE(settings_path);

		return;
	}

	if (load_settings_from_file(gSettings, settings_path) == FALSE)
		error_exit();

	HEAP_FREE(settings_path);
}