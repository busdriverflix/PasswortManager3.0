#include <math.h>
#include <time.h>

#include "global.h"
#include "program.h"

inline int random_num(_In_ int min, _In_ int max);

// Decode, encode and generate password

void generate_password(_In_ wchar_t* password_buffer, _In_ int length, _In_ unsigned int mode)
{
	const wchar_t abc[] = L"abcdefghijklmnopqrstuvvxyzäöü";
	const wchar_t ABC[] = L"ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÜ";
	const wchar_t num[] = L"0123456789";
	const wchar_t sym[] = L"^!°\"§$%&/()=?ß´`\\*+-~#'_<>|{[]}²³";

    const wchar_t* selectedChars[] = { NULL, NULL, NULL, NULL };
    int selectedCharsLengths[] = { 0, 0, 0, 0 };
    int selectedCharsCount = 0;

    // Determine available character sets based on mode configuration
    if (mode & CFG_USE_BIG_CHARS)
    {
        selectedChars[selectedCharsCount] = ABC;
        selectedCharsLengths[selectedCharsCount] = wcslen(ABC);
        selectedCharsCount++;
    }
    if (mode & CFG_USE_SMALL_CHARS)
    {
        selectedChars[selectedCharsCount] = abc;
        selectedCharsLengths[selectedCharsCount] = wcslen(abc);
        selectedCharsCount++;
    }
    if (mode & CFG_USE_NUMBERS)
    {
        selectedChars[selectedCharsCount] = num;
        selectedCharsLengths[selectedCharsCount] = wcslen(num);
        selectedCharsCount++;
    }
    if (mode & CFG_USE_SYMBOLS)
    {
        selectedChars[selectedCharsCount] = sym;
        selectedCharsLengths[selectedCharsCount] = wcslen(sym);
        selectedCharsCount++;
    }

    if (selectedCharsCount < 1)
        return;

    // Set random seed
    srand((unsigned int)time(NULL));

    // Generate random string
    for (int i = 0; i < length; i++)
    {
        int selectedSetIndex = rand() % selectedCharsCount;
        int selectedSetLength = selectedCharsLengths[selectedSetIndex];
        int randomIndex = rand() % selectedSetLength;
        password_buffer[i] = selectedChars[selectedSetIndex][randomIndex];
    }

    // Null-terminate the string
    password_buffer[length] = L'\0';
}

inline int random_num(_In_ int min, _In_ int max)
{
	return ((rand() % (max - min)) + min);
}