#ifndef FILE_H
#define FILE_H

#define NUM_SETTINGS 8

#include "global.h"

// Writes the settings to the specified, returns FALSE when it fails at some point
BOOL write_settings_to_file(_In_ Settings* settings, _In_ const wchar_t* settings_path);

// Loads the settings from the specified settings.ini file (saves it into the settings ptr), returns FALSE on failure
BOOL load_settings_from_file(_In_ Settings* settings, _In_ const wchar_t* settings_path);

// Reads the profiles stored in the specified file
void read_profiles_from_file(_In_ const wchar_t* file_path);

// Appends a profile to the profiles file
BOOL append_profile_to_file(_In_ const wchar_t* file_path, _In_ Profile* profile, _In_ BOOL is_default);

#endif