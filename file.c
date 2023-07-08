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

	if (fwprintf_s(fp, L"background=#%06X\naccent=#%06X\nforeground=#%06X\nfont=%s", settings->background,
		settings->accent, settings->foreground, settings->font) > 0)
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

	if (fwscanf_s(fp, L"background=#%06lx\naccent=#%06lx\nforeground=#%06lx\nfont=%ls", &settings->background,
		&settings->accent, &settings->foreground, settings->font, 32) != NUM_SETTINGS)
	{
		fclose(fp);
		return FALSE;
	}

	fclose(fp);
	return TRUE;
}