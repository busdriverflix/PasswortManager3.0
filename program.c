#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <stdint.h>
//#include <stdbool.h>
#include <wchar.h>
#include <windows.h>
//#include <wctype.h>
//#include <wincrypt.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include "global.h"
#include "program.h"

#define KEYSIZE 32

inline int random_num(_In_ int min, _In_ int max);
char* encode_openssl(_In_ const char* unencoded_str, _In_ const char* passed_password);
char* decode_openssl(_In_ const char* encoded_str, _In_ const char* passed_password);

static BOOL openssl_initialized = FALSE;

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

//static wchar_t* Encode(const wchar_t* unencodedString, const wchar_t* passedPassword)
//{
//    HCRYPTPROV hCryptProv = NULL;
//    if (!CryptAcquireContextW(&hCryptProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT))
//    {
//        // Error handling
//        return NULL;
//    }
//
//    // Salt and IV is randomly generated each time, but is preprended to encrypted cipher text
//    // so that the same Salt and IV values can be used when decrypting.
//    BYTE saltStringBytes[32];
//    if (!CryptGenRandom(hCryptProv, sizeof(saltStringBytes), saltStringBytes))
//    {
//        // Error handling
//        CryptReleaseContext(hCryptProv, 0);
//        return NULL;
//    }
//
//    BYTE ivStringBytes[32];
//    if (!CryptGenRandom(hCryptProv, sizeof(ivStringBytes), ivStringBytes))
//    {
//        // Error handling
//        CryptReleaseContext(hCryptProv, 0);
//        return NULL;
//    }
//
//    DWORD plainTextSize = (DWORD)wcslen(unencodedString) * sizeof(wchar_t);
//    BYTE* plainTextBytes = (BYTE*)unencodedString;
//
//    HCRYPTHASH hHash = NULL;
//    if (!CryptCreateHash(hCryptProv, CALG_SHA_256, 0, 0, &hHash))
//    {
//        // Error handling
//        CryptReleaseContext(hCryptProv, 0);
//        return NULL;
//    }
//
//    if (!CryptDeriveKey(hCryptProv, CALG_AES_256, hHash, KEYSIZE * 8, NULL))
//    {
//        // Error handling
//        CryptDestroyHash(hHash);
//        CryptReleaseContext(hCryptProv, 0);
//        return NULL;
//    }
//
//    HCRYPTKEY hKey = NULL;
//    if (!CryptGenKey(hCryptProv, CALG_AES_256, 0, 0, &hKey))
//    {
//        // Error handling
//        CryptDestroyHash(hHash);
//        CryptReleaseContext(hCryptProv, 0);
//        return NULL;
//    }
//
//    if (!CryptSetKeyParam(hKey, KP_IV, ivStringBytes, 0))
//    {
//        // Error handling
//        CryptDestroyKey(hKey);
//        CryptDestroyHash(hHash);
//        CryptReleaseContext(hCryptProv, 0);
//        return NULL;
//    }
//
//    DWORD cipherTextSize = plainTextSize + KEYSIZE / 8;
//    BYTE* cipherTextBytes = (BYTE*)malloc(cipherTextSize);
//    if (!cipherTextBytes)
//    {
//        // Error handling
//        CryptDestroyKey(hKey);
//        CryptDestroyHash(hHash);
//        CryptReleaseContext(hCryptProv, 0);
//        return NULL;
//    }
//
//    memcpy(cipherTextBytes, saltStringBytes, sizeof(saltStringBytes));
//    memcpy(cipherTextBytes + sizeof(saltStringBytes), ivStringBytes, sizeof(ivStringBytes));
//
//    if (!CryptEncrypt(hKey, NULL, TRUE, 0, cipherTextBytes + sizeof(saltStringBytes) + sizeof(ivStringBytes), &plainTextSize, cipherTextSize - sizeof(saltStringBytes) - sizeof(ivStringBytes)))
//    {
//        // Error handling
//        free(cipherTextBytes);
//        CryptDestroyKey(hKey);
//        CryptDestroyHash(hHash);
//        CryptReleaseContext(hCryptProv, 0);
//        return NULL;
//    }
//
//    CryptDestroyKey(hKey);
//    CryptDestroyHash(hHash);
//    CryptReleaseContext(hCryptProv, 0);
//
//    DWORD finalDataSize = sizeof(saltStringBytes) + sizeof(ivStringBytes) + plainTextSize;
//    BYTE* finalDataBytes = (BYTE*)malloc(finalDataSize);
//    if (!finalDataBytes)
//    {
//        // Error handling
//        free(cipherTextBytes);
//        return NULL;
//    }
//
//    memcpy(finalDataBytes, cipherTextBytes, sizeof(saltStringBytes) + sizeof(ivStringBytes));
//    memcpy(finalDataBytes + sizeof(saltStringBytes) + sizeof(ivStringBytes), cipherTextBytes + sizeof(saltStringBytes) + sizeof(ivStringBytes), plainTextSize);
//
//    free(cipherTextBytes);
//
//    wchar_t* encodedString = (wchar_t*)malloc(finalDataSize * 2 + 1);
//    if (!encodedString)
//    {
//        // Error handling
//        free(finalDataBytes);
//        return NULL;
//    }
//
//    for (DWORD i = 0; i < finalDataSize; i++)
//    {
//        swprintf(encodedString + i * 2, 3, L"%02X", finalDataBytes[i]);
//    }
//
//    encodedString[finalDataSize * 2] = L'\0';
//
//    free(finalDataBytes);
//
//    return encodedString;
//}

//static wchar_t* Decode(const wchar_t* encodedString, const wchar_t* passedPassword)
//{
//    HCRYPTPROV hCryptProv = NULL;
//    if (!CryptAcquireContextW(&hCryptProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT))
//    {
//        // Error handling
//        return NULL;
//    }
//
//    DWORD encodedDataSize = (DWORD)wcslen(encodedString) / 2;
//    BYTE* encodedDataBytes = (BYTE*)malloc(encodedDataSize);
//    if (!encodedDataBytes)
//    {
//        // Error handling
//        CryptReleaseContext(hCryptProv, 0);
//        return NULL;
//    }
//
//    for (DWORD i = 0; i < encodedDataSize; i++)
//    {
//        swscanf_s(encodedString + i * 2, L"%02hhX", &encodedDataBytes[i]);
//    }
//
//    BYTE saltStringBytes[32];
//    memcpy(saltStringBytes, encodedDataBytes, sizeof(saltStringBytes));
//
//    BYTE ivStringBytes[32];
//    memcpy(ivStringBytes, encodedDataBytes + sizeof(saltStringBytes), sizeof(ivStringBytes));
//
//    DWORD cipherTextSize = encodedDataSize - sizeof(saltStringBytes) - sizeof(ivStringBytes);
//    BYTE* cipherTextBytes = encodedDataBytes + sizeof(saltStringBytes) + sizeof(ivStringBytes);
//
//    HCRYPTHASH hHash = NULL;
//    if (!CryptCreateHash(hCryptProv, CALG_SHA_256, 0, 0, &hHash))
//    {
//        // Error handling
//        free(encodedDataBytes);
//        CryptReleaseContext(hCryptProv, 0);
//        return NULL;
//    }
//
//    if (!CryptDeriveKey(hCryptProv, CALG_AES_256, hHash, 0, NULL, KEYSIZE * 8, NULL))
//    {
//        // Error handling
//        CryptDestroyHash(hHash);
//        free(encodedDataBytes);
//        CryptReleaseContext(hCryptProv, 0);
//        return NULL;
//    }
//
//    HCRYPTKEY hKey = NULL;
//    if (!CryptGenKey(hCryptProv, CALG_AES_256, 0, 0, &hKey))
//    {
//        // Error handling
//        CryptDestroyHash(hHash);
//        free(encodedDataBytes);
//        CryptReleaseContext(hCryptProv, 0);
//        return NULL;
//    }
//
//    if (!CryptSetKeyParam(hKey, KP_IV, ivStringBytes, 0))
//    {
//        // Error handling
//        CryptDestroyKey(hKey);
//        CryptDestroyHash(hHash);
//        free(encodedDataBytes);
//        CryptReleaseContext(hCryptProv, 0);
//        return NULL;
//    }
//
//    DWORD plainTextSize = cipherTextSize;
//    BYTE* plainTextBytes = (BYTE*)malloc(plainTextSize);
//    if (!plainTextBytes)
//    {
//        // Error handling
//        CryptDestroyKey(hKey);
//        CryptDestroyHash(hHash);
//        free(encodedDataBytes);
//        CryptReleaseContext(hCryptProv, 0);
//        return NULL;
//    }
//
//    memcpy(plainTextBytes, cipherTextBytes, plainTextSize);
//
//    if (!CryptDecrypt(hKey, NULL, TRUE, 0, plainTextBytes, &plainTextSize))
//    {
//        // Error handling
//        free(plainTextBytes);
//        CryptDestroyKey(hKey);
//        CryptDestroyHash(hHash);
//        free(encodedDataBytes);
//        CryptReleaseContext(hCryptProv, 0);
//        return NULL;
//    }
//
//    CryptDestroyKey(hKey);
//    CryptDestroyHash(hHash);
//    free(encodedDataBytes);
//    CryptReleaseContext(hCryptProv, 0);
//
//    wchar_t* decodedString = (wchar_t*)malloc(plainTextSize + sizeof(wchar_t));
//    if (!decodedString)
//    {
//        // Error handling
//        free(plainTextBytes);
//        return NULL;
//    }
//
//    memcpy(decodedString, plainTextBytes, plainTextSize);
//    decodedString[plainTextSize / sizeof(wchar_t)] = L'\0';
//
//    free(plainTextBytes);
//
//    return decodedString;
//}

BOOL encrypt_str(_In_ const char* src, _In_ const char* password, _Inout_ char* output)
{
    char* encoded_str = encode_openssl(src, password);

    if (encoded_str == NULL)
        return FALSE;

    strcpy_s(output, strlen(encoded_str) + 1, encoded_str);

    free(encoded_str);

    return TRUE;
}

BOOL decrypt_str(_In_ const char* src, _In_ const char* password, _Inout_ char* output)
{
    char* decoded_str = decode_openssl(src, password);

    if (decoded_str == NULL)
        return FALSE;

    strcpy_s(output, strlen(decoded_str) + 1, decoded_str);

    free(decoded_str);

    return TRUE;
}

static void handleErrors(void)
{
    __debugbreak();
}

static int encrypt(unsigned char* plaintext, int plaintext_len, unsigned char* key,
    unsigned char* iv, unsigned char* ciphertext)
{
    EVP_CIPHER_CTX* ctx;

    int len;

    int ciphertext_len;

    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    /*
     * Initialise the encryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;

    /*
     * Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleErrors();
    ciphertext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

static int decrypt(unsigned char* ciphertext, int ciphertext_len, unsigned char* key,
    unsigned char* iv, unsigned char* plaintext)
{
    EVP_CIPHER_CTX* ctx;

    int len;

    int plaintext_len;

    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    /*
     * Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary.
     */
    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;

    /*
     * Finalise the decryption. Further plaintext bytes may be written at
     * this stage.
     */
    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        handleErrors();
    plaintext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

char* encode_openssl(_In_ const char* unencoded_str, _In_ const char* passed_password)
{
    /* A 128 bit IV */
    const unsigned char iv[] = 
    {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35
    };

    /*
     * Buffer for ciphertext. Ensure the buffer is long enough for the
     * ciphertext which may be longer than the plaintext, depending on the
     * algorithm and mode.
     */
    unsigned char* ciphertext = malloc(128);

    int ciphertext_len = encrypt((unsigned char*)unencoded_str, strlen(unencoded_str), (unsigned char*)passed_password, iv,
        ciphertext);

    ciphertext[ciphertext_len] = '\0';

    return ciphertext;
}

char* decode_openssl(_In_ const char* encoded_str, _In_ const char* passed_password)
{
    /* A 128 bit IV */
    const unsigned char iv[] =
    {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35
    };

    unsigned char* decryptedtext = malloc(128);

    size_t length = strlen(encoded_str);

    int decryptedText_len = decrypt((unsigned char*)encoded_str, length, (unsigned char*)passed_password, iv, decryptedtext);

    decryptedtext[decryptedText_len] = '\0';

    return decryptedtext;
}