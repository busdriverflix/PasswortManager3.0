#include <windows.h>
#include <stdio.h>
#include <wchar.h>

#include "file.h"
#include "util.h"

void write_settings_to_file(_In_ Settings* settings, _In_ const wchar_t* settings_path)
{
	FILE* fp = NULL;

	_wfopen_s(&fp, settings_path, L"r");
}