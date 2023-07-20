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

		wchar_t encrypted_password_wstr[31];

		if (swscanf_s(line_buffer, L"Profile:Name:'%[^']',PW:'%[^']',Default:%d;", new_profile->name, _countof(new_profile->name), encrypted_password_wstr, _countof(encrypted_password_wstr), &default_profile) == 3)
		{
			if (default_profile)
				gProfiles->default_profile_index = gProfiles->num_profiles;

			gProfiles->profiles[gProfiles->num_profiles++] = new_profile;

			size_t num_converted = 0;
			wcstombs_s(&num_converted, new_profile->encrypted_password, 31, encrypted_password_wstr, sizeof(encrypted_password_wstr));
		}
		else
		{
			HEAP_FREE(new_profile);
			messageboxf(MB_OK | MB_ICONERROR, L"Fehler!", L"Konnte ein Profil nicht aus der Datei lesen!");
		}
	}

	gProfiles->current_profile = gProfiles->current_profile;

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

	fseek(fp, 0, SEEK_END);

	fwprintf_s(fp, L"Profile:Name:'%s',PW:'%hs',Default:%d;", profile->name, profile->encrypted_password, is_default);

	fclose(fp);

	return TRUE;
}

BOOL append_password_entry_to_file(_In_ const char* file_path, _In_ PasswordEntry* entry)
{
	FILE* fp = NULL;

	fopen_s(&fp, file_path, "a");

	if (fp == NULL)
	{
		return FALSE;
	}

	fseek(fp, 0, SEEK_END);

	fprintf_s(fp, "%s\n%s\n%s\n%s", entry->encrypted_name, entry->encrypted_user_name, entry->encrypted_email, entry->encrypted_password);

	fclose(fp);
}

void save_passwords_from_profile(_In_ Profile* profile)
{
	// Get the path to the password file of the profile
	char* appdata_roaming_path = get_roaming_folder_path();
	char file_path[MAX_PATH];
	sprintf_s(file_path, MAX_PATH, "%s\\PasswortManager\%S.DATA", appdata_roaming_path, profile->name);

	FILE* fp = NULL;
	fopen_s(&fp, file_path, "w");

	if (fp == NULL)
	{
		// TODO: Handle error
		return;
	}

	// Number of passwords
	fprintf_s(fp, "%d\n", profile->num_passwords);

	// Write each password entry to file
	for (int i = 0; i < profile->num_passwords; i++)
	{
		fprintf_s(fp, "%s\n%s\n%s\n%s\n", profile->passwords[i].encrypted_name, profile->passwords[i].encrypted_user_name, profile->passwords[i].encrypted_email, profile->passwords[i].encrypted_password);
	}

	fclose(fp);
}

void read_passwords_from_profile(_In_ Profile* profile)
{
	// Get file name of the profile
	char* appdata_roaming_path = get_roaming_folder_path();

	char file_path[MAX_PATH];
	sprintf_s(file_path, MAX_PATH, "%s\\PasswortManager\\%S.DATA", appdata_roaming_path, profile->name);

	FILE* fp = NULL;
	fopen_s(&fp, file_path, "r");

	if (fp == NULL)
	{
		// TODO: Handle error
		return;
	}

	char line[8];
	fgets(line, sizeof(line), fp);
	sscanf_s(line, "%d", &profile->num_passwords);

	if (profile->passwords == 0)
	{
		fclose(fp);
		return;
	}

	profile->passwords = HEAP_ALLOCZ(sizeof(PasswordEntry) * profile->num_passwords);

	if (profile->passwords == NULL)
	{
		// TODO: Handle error
		__debugbreak();
		return;
	}

	for (int i = 0; i < profile->num_passwords; i++) 
	{
		fgets(profile->passwords[i].encrypted_name, 30, fp);
		fgets(profile->passwords[i].encrypted_user_name, 30, fp);
		fgets(profile->passwords[i].encrypted_email, 30, fp);
		fgets(profile->passwords[i].encrypted_password, 30, fp);
	}

	fclose(fp);
}

void read_line_from_file(_In_ FILE* opened_file, _In_ char* buffer, _In_ int max_count, _In_ int line_number)
{
	// Put cursor back to start
	rewind(opened_file);

	// Read each line until the desired line number is reached
	int current_line = 0;
	while (current_line < line_number && fgets(buffer, max_count, opened_file) != NULL)
	{
		current_line++;
	}

	// If the desired line number is found, the buffer will contain the line
	// Otherwise, buffer will be empty
}

void save_password_entry_to_profile(_In_ PasswordEntry* entry, _In_ Profile* profile)
{
	if (profile->passwords == NULL)
	{
		profile->passwords = HEAP_ALLOCZ(sizeof(PasswordEntry));
		profile->passwords[0] = *entry;
		profile->num_passwords = 1;
	}
	else
	{
		profile->passwords = HEAP_REALLOC(profile->passwords, (++profile->num_passwords) * sizeof(PasswordEntry));
		profile->passwords[profile->num_passwords - 1] = *entry;
	}
}