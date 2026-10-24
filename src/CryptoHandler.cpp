#ifdef ENABLE_CRYPTO_HANDLER

#include "CryptoHandler.h"
#include "Globals.h"
#include "mbedtls/base64.h"
#include "mbedtls/md.h"
#include "mbedtls/aes.h"
#include "CommandHandler.h"

#define AES_KEY_SIZE 32   // AES-256 key size (256 bits)
#define AES_BLOCK_SIZE 16 // Block size for AES CBC mode

// Derive a 32-byte key from a password using SHA-256
void deriveKey(const String &password, uint8_t *keyOut) {
    mbedtls_md_context_t ctx;
    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 0);
    mbedtls_md_starts(&ctx);
    mbedtls_md_update(&ctx, (const uint8_t *)password.c_str(), password.length());
    mbedtls_md_finish(&ctx, keyOut);
    mbedtls_md_free(&ctx);
}
String CryptoHandler::encryptAES(const String &plainText, const String &password) {
    uint8_t aesKey[AES_KEY_SIZE];
    deriveKey(password, aesKey);

    uint8_t iv[AES_BLOCK_SIZE];
    esp_fill_random(iv, AES_BLOCK_SIZE);
    uint8_t iv_copy[AES_BLOCK_SIZE]; // Preserve original IV
    memcpy(iv_copy, iv, AES_BLOCK_SIZE);

    size_t inputLen = plainText.length();
    size_t paddedLen = ((inputLen / AES_BLOCK_SIZE) + 1) * AES_BLOCK_SIZE;
    uint8_t *input = (uint8_t *)calloc(paddedLen, 1);
    memcpy(input, plainText.c_str(), inputLen);

    uint8_t padVal = paddedLen - inputLen;
    for (size_t i = inputLen; i < paddedLen; i++) {
        input[i] = padVal;
    }

    uint8_t *output = (uint8_t *)calloc(paddedLen, 1);

    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    int ret = mbedtls_aes_setkey_enc(&aes, aesKey, AES_KEY_SIZE * 8);
    if (ret != 0) {
        debugE("Key setup failed: %d", ret);
        free(input);
        free(output);
        mbedtls_aes_free(&aes);
        return "";
    }
    ret = mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, paddedLen, iv, input, output);
    if (ret != 0) {
        debugE("Encryption failed: %d", ret);
        free(input);
        free(output);
        mbedtls_aes_free(&aes);
        return "";
    }
    mbedtls_aes_free(&aes);

    // Log raw encrypted output
    char rawHex[33];
    for (int i = 0; i < paddedLen; i++) {
        sprintf(rawHex + i * 2, "%02x", output[i]);
    }
    debugV("Raw encrypted: %s", rawHex);

    size_t finalLen = AES_BLOCK_SIZE + paddedLen;
    uint8_t *finalOutput = (uint8_t *)malloc(finalLen);
    memcpy(finalOutput, iv_copy, AES_BLOCK_SIZE); // Use original IV
    memcpy(finalOutput + AES_BLOCK_SIZE, output, paddedLen);

    size_t base64Len = 0;
    size_t maxBase64Len = ((finalLen + 2) / 3) * 4 + 1;
    uint8_t *base64Output = (uint8_t *)malloc(maxBase64Len);
    ret = mbedtls_base64_encode(base64Output, maxBase64Len, &base64Len, finalOutput, finalLen);
    if (ret != 0) {
        debugE("Base64 encode failed: %d", ret);
        free(input);
        free(output);
        free(finalOutput);
        free(base64Output);
        return "";
    }

    String result = String((char *)base64Output);

    free(input);
    free(output);
    free(finalOutput);
    free(base64Output);

    return result;
}

String CryptoHandler::decryptAES(const String &cipherText, const String &password) {
    uint8_t aesKey[AES_KEY_SIZE];
    deriveKey(password, aesKey);

    size_t decodedLen = 0;
    size_t maxDecodedLen = ((cipherText.length() + 3) / 4) * 3;
    uint8_t *decoded = (uint8_t *)malloc(maxDecodedLen);
    int ret = mbedtls_base64_decode(decoded, maxDecodedLen, &decodedLen, (const uint8_t *)cipherText.c_str(), cipherText.length());
    if (ret != 0 || decodedLen < AES_BLOCK_SIZE) {
        debugE("Base64 decode failed: %d, len: %d", ret, decodedLen);
        free(decoded);
        return "";
    }

    uint8_t iv[AES_BLOCK_SIZE];
    memcpy(iv, decoded, AES_BLOCK_SIZE);
    size_t cipherLen = decodedLen - AES_BLOCK_SIZE;
    uint8_t *ciphertext = decoded + AES_BLOCK_SIZE;

    if (cipherLen % AES_BLOCK_SIZE != 0) {
        debugE("Invalid ciphertext length: %d", cipherLen);
        free(decoded);
        return "";
    }

    uint8_t *output = (uint8_t *)malloc(cipherLen);

    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    ret = mbedtls_aes_setkey_dec(&aes, aesKey, AES_KEY_SIZE * 8);
    if (ret != 0) {
        debugE("Key setup failed: %d", ret);
        free(output);
        free(decoded);
        return "";
    }
    uint8_t iv_copy[AES_BLOCK_SIZE]; // Copy IV to avoid modification
    memcpy(iv_copy, iv, AES_BLOCK_SIZE);
    ret = mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, cipherLen, iv_copy, ciphertext, output);
    if (ret != 0) {
        debugE("Decryption failed: %d", ret);
        free(output);
        free(decoded);
        return "";
    }
    mbedtls_aes_free(&aes);

    // Log raw decrypted output
    char rawHex[33];
    for (int i = 0; i < cipherLen; i++) {
        sprintf(rawHex + i * 2, "%02x", output[i]);
    }
    debugV("Raw decrypted: %s", rawHex);

    uint8_t padVal = output[cipherLen - 1];
    size_t unpaddedLen = cipherLen;
    if (padVal > 0 && padVal <= AES_BLOCK_SIZE) {
        bool validPadding = true;
        for (size_t i = cipherLen - padVal; i < cipherLen; i++) {
            if (output[i] != padVal) {
                validPadding = false;
                break;
            }
        }
        if (validPadding) {
            unpaddedLen = cipherLen - padVal;
        } else {
            debugE("Invalid PKCS7 padding");
        }
    } else {
        debugE("Invalid padding value: %d", padVal);
    }

    String decryptedText((char *)output, unpaddedLen);
    free(output);
    free(decoded);

    debugV("Decrypted length: %d, text: %s", unpaddedLen, decryptedText.c_str());
    return decryptedText;
}

void CryptoHandler::init() {
    CommandHandler::registerCommand("CRYPTO", [](const String &command) {
        String cmd, args;
        CommandHandler::parseCommand(command, cmd, args);

        if (CommandHandler::equalsIgnoreCase(cmd, "ENC")) {
            int spaceIndex = args.indexOf(' ');
            if (spaceIndex > 0) {
                String key = args.substring(0, spaceIndex);
                String text = args.substring(spaceIndex + 1);
                String encrypted = encryptAES(text, key);
                debugI("Encrypted: %s", encrypted.c_str());
            } else {
                debugW("Usage: CRYPTO ENC <key> <text>");
            }
        } else if (CommandHandler::equalsIgnoreCase(cmd, "DEC")) {
            int spaceIndex = args.indexOf(' ');
            if (spaceIndex > 0) {
                String key = args.substring(0, spaceIndex);
                String text = args.substring(spaceIndex + 1);
                String decrypted = decryptAES(text, key);
                debugI("Decrypted: %s", decrypted.c_str());
            } else {
                debugW("Usage: CRYPTO DEC <key> <ciphertext>");
            }
        } else {
            debugW("Unknown CRYPTO subcommand: %s", cmd.c_str());
        }
    }, "Handles CRYPTO commands. Usage: CRYPTO <subcommand> <args>\n"
       "  Subcommands:\n"
       "  enc <key> <text> - Encrypts a string\n"
       "  dec <key> <ciphertext> - Decrypts a string");

    debugI("CryptoHandler initialized");
}

#endif // ENABLE_CRYPTO_HANDLER