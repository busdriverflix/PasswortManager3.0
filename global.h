#ifndef GLOBAL_H
#define GLOBAL_H

#pragma warning(push, 0)
#pragma warning(disable: 4668)
#include <windows.h>
#pragma warning(pop)

#define DBB() __debugbreak()

#define CFG_ASTERISK_PASSWORD	0x0001
#define CFG_USE_BIG_CHARS		0x0002
#define CFG_USE_SMALL_CHARS     0x0004
#define CFG_USE_NUMBERS			0x0008
#define CFG_USE_SYMBOLS			0x0010

#define MAX_PROFILES 5

typedef struct Settings
{
	COLORREF background, accent, foreground;

	unsigned int config;

} Settings;

typedef struct PasswordEntry
{
	char encrypted_name[31];
	char encrypted_user_name[31];
	char encrypted_email[31];
	char encrypted_password[31];

} PasswordEntry;

typedef struct Profile
{
	wchar_t name[16];
	char encrypted_password[31];
	int num_passwords;
	PasswordEntry* passwords;

} Profile;

typedef struct Profiles
{
	Profile* profiles[MAX_PROFILES];
	short num_profiles;
	short default_profile_index;
	short current_profile;

} Profiles;

#endif

extern Settings* gSettings;

extern wchar_t gFont[];
extern const char app_encrypt_decrypt_password[];
extern Profiles* gProfiles;