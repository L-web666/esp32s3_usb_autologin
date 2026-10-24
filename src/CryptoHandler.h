#pragma once

#ifdef ENABLE_CRYPTO_HANDLER

#include <Arduino.h>
#include "mbedtls/aes.h"

class CryptoHandler
{
public:
    static void init();
    static String encryptAES(const String &plainText, const String &key);
    static String decryptAES(const String &cipherText, const String &key);
};

#else

class CryptoHandler
{
public:
    static void init() {}
    static String encryptAES(const String &plainText, const String &key) { return plainText; }
    static String decryptAES(const String &cipherText, const String &key) { return cipherText; }
};

#endif // ENABLE_CRYPTO_HANDLER
