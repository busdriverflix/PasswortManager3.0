#pragma warning(push, 0)
#pragma warning(disable: 4668)
#include <windows.h>
#pragma warning(pop)
#include <shlwapi.h>
#include <stdio.h>
#include <wchar.h>

#include "file.h"
#include "util.h"

#define MAX_LINE_LENGTH MAX_PATH

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

	const wchar_t* profile_file_format_string = L"Profile:Name:'%[^']',PW:'%[^']',Default:%d;";

	// Open file
	FILE* fp = NULL;
	_wfopen_s(&fp, file_path, L"r");

	if (fp == NULL)
	{
		error_exit();
		return; // To make Visual Studio shut the fuck up about dErEfErEnCiNg NuLl pOintEr
	}

	wchar_t line_buffer[MAX_LINE_LENGTH];

	while (gProfiles->num_profiles < MAX_PROFILES && fgetws(line_buffer, MAX_LINE_LENGTH, fp) != NULL)
	{
		Profile* new_profile = HEAP_ALLOCZ(sizeof(Profile));

		if (new_profile == NULL)
		{
			for (int i = 0; i < gProfiles->num_profiles; i++)
			{
				HEAP_FREE(gProfiles->profiles[i]);
			}

			fclose(fp);

			error_exit();
			return;
		}

		BOOL default_profile = FALSE;

		if (swscanf_s(line_buffer, profile_file_format_string, new_profile->name, _countof(new_profile->name), new_profile->encrypted_password, _countof(new_profile->encrypted_password), &default_profile) == 3)
		{
			if (default_profile)
				gProfiles->default_profile_index = gProfiles->num_profiles;

			gProfiles->profiles[gProfiles->num_profiles++] = new_profile;
		}
		else
		{
			HEAP_FREE(new_profile);
			messageboxf(MB_OK | MB_ICONERROR, L"Fehler!", L"Konnte ein Profil nicht aus der Datei lesen!");
		}
	}

	// Cleanup
	fclose(fp);
}

BOOL append_profile_to_file(_In_ const wchar_t* file_path, _In_ Profile* profile, _In_ BOOL is_default)
{
	if (file_path == NULL || profile == NULL)
		return FALSE;

	FILE* fp = NULL;

	_wfopen_s(&fp, file_path, L"a");

	if (fp == NULL)
		return FALSE;

	fwprintf_s(fp, L"Profile:Name:'%s',PW:'%s',Default:%d;", profile->name, profile->encrypted_password, is_default);

	fclose(fp);

	return TRUE;
}

void read_passwords_from_profile(_In_ Profile* profile)
{

}