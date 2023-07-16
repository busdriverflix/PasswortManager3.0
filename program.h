#ifndef PROGRAM_H
#define PROGRAM_H

#include <windows.h>

// Generates a password with the specified length and stores it in the password_buffer.
// The mode parameter decides what characters to use (characters, numbers, symbols).
// Use CFG_... for the mode parameter (or them together)
void generate_password(_In_ wchar_t* password_buffer, _In_ int length, _In_ unsigned int mode);

#endif // PROGRAM_H