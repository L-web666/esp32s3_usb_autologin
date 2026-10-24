#ifdef ENABLE_AES_HANDLER
// AesHandler.h
#pragma once

#include <Arduino.h>
#include <vector>

/**
 * @class AesHandler
 * @brief Handles AES encryption and decryption using ESP32 hardware acceleration.
 */
class AesHandler {
public:
    static String encrypt(const String &plainText, const String &password);
    static String decrypt(const String &cipherText, const String &password);
    static void init();

private:
    static const size_t saltSize;
    static const size_t iterations;
    static const size_t keySize;
    static const size_t blockSize;

    static std::vector<uint8_t> generateRandomBytes(size_t length);
    static size_t getPaddedLength(size_t inputLen, size_t blockSize);
    static void applyPKCS7Padding(uint8_t *buffer, size_t inputLen, size_t blockSize);
    static size_t removePKCS7Padding(const uint8_t *buffer, size_t length);
    static void deriveKeyAndIV(const char *password, const uint8_t *salt, uint8_t *key, uint8_t *iv);
    static void encryptData(const uint8_t *plainText, size_t length, const uint8_t *key, const uint8_t *iv, uint8_t *output);
    static void decryptData(const uint8_t *cipherText, size_t length, const uint8_t *key, const uint8_t *iv, uint8_t *output);
};

#else

/**
 * No-op implementation if ENABLE_AES_HANDLER is not defined.
 * This allows other code to compile without changing references.
 */
class AesHandler
{
public:
    static String encrypt(const String &plainText, const String &password) { return ""; }
    static String decrypt(const String &cipherText, const String &password) { return ""; }
    static void init() {}
};

#endif //ENABLE_AES_HANDLER
