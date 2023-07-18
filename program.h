#ifndef PROGRAM_H
#define PROGRAM_H

#pragma warning(push, 0)
#pragma warning(disable: 4668)
#include <windows.h>
#pragma warning(pop)

// Generates a password with the specified length and stores it in the password_buffer.
// The mode parameter decides what characters to use (characters, numbers, symbols).
// Use CFG_... for the mode parameter (or them together)
void generate_password(_In_ wchar_t* password_buffer, _In_ int length, _In_ unsigned int mode);

// Encrypts a string using the password parameter and saves it to the output str
BOOL encrypt_str(_In_ const char* src, _In_ const char* password, _Inout_ char* output);

// Decrypts a string using the password parameter and saves it to the output str
BOOL decrypt_str(_In_ const char* src, _In_ const char* password, _Inout_ char* output);

#endif // PROGRAM_H