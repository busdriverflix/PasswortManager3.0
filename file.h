#ifndef FILE_H
#define FILE_H

#define NUM_SETTINGS 8

#include "global.h"

// Writes the settings to the specified, returns FALSE when it fails at some point
BOOL write_settings_to_file(_In_ Settings* settings, _In_ const wchar_t* settings_path);

// Loads the settings from the specified settings.ini file (saves it into the settings ptr), returns FALSE on failure
BOOL load_settings_from_file(_In_ Settings* settings, _In_ const wchar_t* settings_path);

#endif