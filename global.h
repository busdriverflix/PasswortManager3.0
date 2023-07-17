#ifndef GLOBAL_H
#define GLOBAL_H

#include <windows.h>

#define DBB() __debugbreak()

#define CFG_ASTERISK_PASSWORD	0x0001
#define CFG_USE_BIG_CHARS		0x0002
#define CFG_USE_SMALL_CHARS     0x0004
#define CFG_USE_NUMBERS			0x0008
#define CFG_USE_SYMBOLS			0x0010

typedef struct Settings
{
	COLORREF background, accent, foreground;

	unsigned int config;

} Settings;

typedef struct Profile
{
	wchar_t* name;
	wchar_t* encrypted_password;
} Profile;

typedef struct Profiles
{
	Profile* profiles;
	short num_profiles;
	short default_profile_index;

} Profiles;

#endif

extern Settings* gSettings;
extern wchar_t master_password[];

extern wchar_t gFont[];
extern const wchar_t app_encrypt_decrypt_password[];
extern Profiles* gProfiles;