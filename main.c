#pragma warning(push, 0)
#pragma warning(disable: 4668)
#include <windows.h>
#pragma warning(pop)
#include <commctrl.h>
#include <shlwapi.h>
#include <wchar.h>
#include <stdio.h>
#include "global.h"
#include "util.h"
#include "file.h"
#include "window.h"

Settings gSettings_s = { 0 };
Settings* gSettings = &gSettings_s;

wchar_t gFont[] = L"Calibri";
const char app_encrypt_decrypt_password[] = "<g88U-96n4T$9e<6w)o(o7aq§0h9hd";
Profiles gProfiles_s = { 0 };
Profiles* gProfiles = &gProfiles_s;

// Function prototypes
void setup(void);
inline void load_settings(void);
inline void check_profiles_file(void);

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

	check_profiles_file();

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

	wchar_t settings_path[MAX_PATH];

	swprintf_s(settings_path, MAX_PATH, L"%s\\settings.ini", exepath);

	if (!PathFileExistsW(settings_path))
	{
		// If the settings file doesn't exist, create one
		Settings default_settings =
		{
			.background = RGB(0x18, 0x18, 0x18),
			.accent = RGB(0x22, 0x22, 0x22),
			.foreground = RGB(255, 255, 255),
			.config = (CFG_ASTERISK_PASSWORD | CFG_USE_BIG_CHARS | CFG_USE_SMALL_CHARS | CFG_USE_NUMBERS | CFG_USE_SYMBOLS)
		};

		if (write_settings_to_file(&default_settings, settings_path) == FALSE)
			error_exit();

		*gSettings = default_settings;

		return;
	}

	if (load_settings_from_file(gSettings, settings_path) == FALSE)
		error_exit();
}

inline void check_profiles_file(void)
{
	wchar_t* appdata_roaming_path = get_roaming_folder_path_wstr();

	if (appdata_roaming_path == NULL)
		error_exit();

	wchar_t profiles_path[MAX_PATH];

	swprintf_s(profiles_path, MAX_PATH, L"%s\\PasswortManager\\", appdata_roaming_path);

	if (create_folder(profiles_path) == FALSE)
		error_exit();

	swprintf_s(profiles_path, MAX_PATH, L"%s\\PasswortManager\\ProfileData.profiles", appdata_roaming_path);

	if (!PathFileExistsW(profiles_path))
	{
		// If the profiles file doesn't exist, create one
		FILE* fp = NULL;
		_wfopen_s(&fp, profiles_path, L"w");
		
		if (fp == NULL)
		{
			fclose(fp);
			error_exit();
		}
		else
			fclose(fp);

		return;
	}

	// Read profile data
	read_profiles_from_file(profiles_path);
}