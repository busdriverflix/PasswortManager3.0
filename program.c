#include <math.h>
#include <time.h>

#include "program.h"

// Decode, encode and generate password

void generate_password(_In_ wchar_t* password_buffer, _In_ int length, _In_ unsigned int mode)
{
	const wchar_t abc[] = L"abcdefghijklmnopqrstuvvxyzäöü";
	const wchar_t ABC[] = L"ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÜ";
	const wchar_t num[] = L"0123456789";
	const wchar_t sym[] = L"^!°\"§$%&/()=?ß´`\\*+-~#'_<>|{[]}²³";

	// Set random seed
	srand(time(NULL));


}