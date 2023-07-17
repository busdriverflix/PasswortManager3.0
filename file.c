#include <windows.h>
#include <shlwapi.h>
#include <stdio.h>
#include <wchar.h>

#include "file.h"
#include "util.h"

BOOL write_settings_to_file(_In_ Settings* settings, _In_ const wchar_t* settings_path)
{
	if (settings == NULL)
		return FALSE;

	FILE* fp = NULL;

	if (_wfopen_s(&fp, settings_path, L"w") != 0)
		return FALSE;

	if (fp == NULL)	// Redundant
		return FALSE;

	BOOL asterisk_password = (settings->config & CFG_ASTERISK_PASSWORD) ? TRUE : FALSE;
	BOOL use_big_chars = (settings->config & CFG_USE_BIG_CHARS) ? TRUE : FALSE;
	BOOL use_small_chars = (settings->config & CFG_USE_SMALL_CHARS) ? TRUE : FALSE;
	BOOL use_nums = (settings->config & CFG_USE_NUMBERS) ? TRUE : FALSE;
	BOOL use_symbols = (settings->config & CFG_USE_SYMBOLS) ? TRUE : FALSE;

	if (fwprintf_s(fp, L"background=#%06X\naccent=#%06X\nforeground=#%06X\nasterisk=%d\nuse_big_chars=%d\nuse_small_chars=%d\nuse_nums=%d\nuse_symbols=%d", settings->background,
		settings->accent, settings->foreground, asterisk_password, use_big_chars, use_small_chars, use_nums, use_symbols) < 0)
	{
		fclose(fp);
		return FALSE;
	}

	fclose(fp);
	return TRUE;
}

BOOL load_settings_from_file(_In_ Settings* settings, _In_ const wchar_t* settings_path)
{
	if (settings == NULL)
		return FALSE;

	FILE* fp = NULL;

	if (_wfopen_s(&fp, settings_path, L"r") != 0)
		return FALSE;

	if (fp == NULL)	// Redundant
		return FALSE;

	BOOL asterisk_password = FALSE, use_big_chars = FALSE, use_small_chars = FALSE, use_numbers = FALSE, use_symbols = FALSE;

	if (fwscanf_s(fp, L"background=#%06X\naccent=#%06X\nforeground=#%06X\nasterisk=%d\nuse_big_chars=%d\nuse_small_chars=%d\nuse_nums=%d\nuse_symbols=%d", &settings->background, &settings->accent, &settings->foreground, &asterisk_password, &use_big_chars, &use_small_chars, &use_numbers, &use_symbols) != NUM_SETTINGS)
	{
		fclose(fp);
		return FALSE;
	}

	settings->config |= (asterisk_password ? CFG_ASTERISK_PASSWORD : 0) | (use_big_chars ? CFG_USE_BIG_CHARS : 0) | (use_small_chars ? CFG_USE_SMALL_CHARS : 0) | (use_numbers ? CFG_USE_NUMBERS : 0) | (use_symbols ? CFG_USE_SYMBOLS : 0);

	fclose(fp);
	return TRUE;
}

void read_profiles_from_file(_In_ const wchar_t* file_path)
{
	if (file_path == NULL || !PathFileExistsW(file_path))
	{
		error_exit();
	}

	const wchar_t* profile_file_format_string = L"Profile:Name:\"%s\",PW:\"%s\"Default:%d;";

	// Open file
	FILE* fp = NULL;
	_wfopen_s(&fp, file_path, L"r");

	if (fp == NULL)
		error_exit();


}