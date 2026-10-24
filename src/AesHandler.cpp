#ifdef ENABLE_AES_HANDLER
// AesHandler.cpp
#include "AesHandler.h"
#include "Globals.h"
#include <aes/esp_aes.h>
#include "mbedtls/md.h"
#include "mbedtls/pkcs5.h"
#include "arduino_base64.hpp"

//const size_t AesHandler::iterations = 10000; //Anything above 10000 starts to become really slow
const size_t AesHandler::iterations = 1000; //Fast but less secure
const size_t AesHandler::saltSize = 16;
const size_t AesHandler::keySize = 32;
const size_t AesHandler::blockSize = 16;

String AesHandler::encrypt(const String &plainText, const String &password)
{
    std::vector<uint8_t> salt = generateRandomBytes(saltSize);
    std::vector<uint8_t> key(keySize), iv(blockSize);
    deriveKeyAndIV(password.c_str(), salt.data(), key.data(), iv.data());

    size_t paddedLen = getPaddedLength(plainText.length(), blockSize);
    std::vector<uint8_t> inputBuffer(paddedLen);
    memcpy(inputBuffer.data(), plainText.c_str(), plainText.length());
    applyPKCS7Padding(inputBuffer.data(), plainText.length(), blockSize);

    std::vector<uint8_t> cipherText(paddedLen);
    encryptData(inputBuffer.data(), paddedLen, key.data(), iv.data(), cipherText.data());

    std::vector<uint8_t> saltedCipherText;
    saltedCipherText.reserve(saltSize + paddedLen);
    saltedCipherText.insert(saltedCipherText.end(), salt.begin(), salt.end());
    saltedCipherText.insert(saltedCipherText.end(), cipherText.begin(), cipherText.end());

    std::vector<char> encodedText(base64::encodeLength(saltedCipherText.size()));
    base64::encode(saltedCipherText.data(), saltedCipherText.size(), encodedText.data());
    return String(encodedText.data());
}

String AesHandler::decrypt(const String &cipherText, const String &password)
{
    std::vector<uint8_t> decodedBytes(base64::decodeLength(cipherText.c_str()));
    base64::decode(cipherText.c_str(), decodedBytes.data());

    std::vector<uint8_t> salt(saltSize);
    memcpy(salt.data(), decodedBytes.data(), saltSize);
    std::vector<uint8_t> key(keySize), iv(blockSize);
    deriveKeyAndIV(password.c_str(), salt.data(), key.data(), iv.data());

    uint8_t *cipherData = decodedBytes.data() + saltSize;
    size_t cipherLen = decodedBytes.size() - saltSize;

    std::vector<uint8_t> plainTextBuffer(cipherLen);
    decryptData(cipherData, cipherLen, key.data(), iv.data(), plainTextBuffer.data());

    size_t plainTextLen = removePKCS7Padding(plainTextBuffer.data(), cipherLen);
    return String(reinterpret_cast<const char *>(plainTextBuffer.data()), plainTextLen);
}

std::vector<uint8_t> AesHandler::generateRandomBytes(size_t length)
{
    std::vector<uint8_t> buffer(length);
    uint32_t randValue;
    for (size_t i = 0; i < length; i += 4)
    {
        randValue = esp_random();
        memcpy(buffer.data() + i, &randValue, (length - i < 4) ? length - i : 4);
    }
    return buffer;
}

size_t AesHandler::getPaddedLength(size_t inputLen, size_t blockSize)
{
    return ((inputLen / blockSize) + 1) * blockSize;
}

void AesHandler::applyPKCS7Padding(uint8_t *buffer, size_t inputLen, size_t blockSize)
{
    uint8_t padding = blockSize - (inputLen % blockSize);
    memset(buffer + inputLen, padding, blockSize - (inputLen % blockSize));
}

size_t AesHandler::removePKCS7Padding(const uint8_t *buffer, size_t length)
{
    return length - buffer[length - 1];
}

void AesHandler::deriveKeyAndIV(const char *password, const uint8_t *salt, uint8_t *key, uint8_t *iv)
{
    mbedtls_md_context_t ctx;
    mbedtls_md_init(&ctx);
    const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    mbedtls_md_setup(&ctx, md_info, 1);

    std::vector<uint8_t> keyIV(keySize + blockSize);
    mbedtls_pkcs5_pbkdf2_hmac(&ctx, reinterpret_cast<const uint8_t *>(password), strlen(password),
                              salt, saltSize, iterations, keyIV.size(), keyIV.data());
    memcpy(key, keyIV.data(), keySize);
    memcpy(iv, keyIV.data() + keySize, blockSize);
    mbedtls_md_free(&ctx);
}

void AesHandler::encryptData(const uint8_t *plainText, size_t length, const uint8_t *key, const uint8_t *iv, uint8_t *output)
{
    esp_aes_context aes;
    esp_aes_init(&aes);
    esp_aes_setkey(&aes, key, keySize * 8);
    esp_aes_crypt_cbc(&aes, ESP_AES_ENCRYPT, length, const_cast<uint8_t *>(iv), plainText, output);
    esp_aes_free(&aes);
}

void AesHandler::decryptData(const uint8_t *cipherText, size_t length, const uint8_t *key, const uint8_t *iv, uint8_t *output)
{
    esp_aes_context aes;
    esp_aes_init(&aes);
    esp_aes_setkey(&aes, key, keySize * 8);
    esp_aes_crypt_cbc(&aes, ESP_AES_DECRYPT, length, const_cast<uint8_t *>(iv), cipherText, output);
    esp_aes_free(&aes);
}

void AesHandler::init()
{
    CommandHandler::registerCommand("aes", [](const String &command) {
        String cmd, args;
        CommandHandler::parseCommand(command, cmd, args);

        if (cmd == "enc") {
            int delimiterPos = args.indexOf(' ');
            if (delimiterPos == -1) {
                debugW("Invalid arguments for enc. Usage: aes enc <text> <password>");
                return;
            }
            String text = args.substring(0, delimiterPos);
            String password = args.substring(delimiterPos + 1);

            String encryptedText = AesHandler::encrypt(text, password);
            debugI("Encrypted: %s", encryptedText.c_str());
        } else if (cmd == "dec") {
            int delimiterPos = args.indexOf(' ');
            if (delimiterPos == -1) {
                debugW("Invalid arguments for dec. Usage: aes dec <text> <password>");
                return;
            }
            String text = args.substring(0, delimiterPos);
            String password = args.substring(delimiterPos + 1);

            String decryptedText = AesHandler::decrypt(text, password);
            debugI("Decrypted: %s", decryptedText.c_str());
        } else {
            debugW("Unknown aes subcommand: %s", cmd.c_str());
        }
    }, "Handles aes commands. Usage: aes <subcommand> [args]\n"
       "Subcommands:\n"
       "  enc <text> <password> - Encrypts the given text using the password\n"
       "  dec <text> <password> - Decrypts the given text using the password");
}

#endif //ENABLE_AES_HANDLER