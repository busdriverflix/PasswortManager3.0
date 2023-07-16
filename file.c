#include <windows.h>
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
	BOOL use_chars = (settings->config & CFG_USE_CHARACTERS) ? TRUE : FALSE;
	BOOL use_nums = (settings->config & CFG_USE_NUMBERS) ? TRUE : FALSE;
	BOOL use_symbols = (settings->config & CFG_USE_SYMBOLS) ? TRUE : FALSE;

	if (fwprintf_s(fp, L"background=#%06X\naccent=#%06X\nforeground=#%06X\nasterisk=%d\nuse_chars=%d\nuse_nums=%d\nuse_symbols=%d", settings->background,
		settings->accent, settings->foreground, asterisk_password, use_chars, use_nums, use_symbols) < 0)
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

	BOOL asterisk_password = FALSE, use_characters = FALSE, use_numbers = FALSE, use_symbols = FALSE;

	if (fwscanf_s(fp, L"background=#%06X\naccent=#%06X\nforeground=#%06X\nasterisk=%d\nuse_chars=%d\nuse_nums=%d\nuse_symbols=%d", &settings->background, &settings->accent, &settings->foreground, &asterisk_password, &use_characters, &use_numbers, &use_symbols) != NUM_SETTINGS)
	{
		fclose(fp);
		return FALSE;
	}

	settings->config |= (asterisk_password ? CFG_ASTERISK_PASSWORD : 0) | (use_characters ? CFG_USE_CHARACTERS : 0) | (use_numbers ? CFG_USE_NUMBERS : 0) | (use_symbols ? CFG_USE_SYMBOLS : 0);

	fclose(fp);
	return TRUE;
}