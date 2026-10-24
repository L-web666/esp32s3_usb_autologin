#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <mbedtls/aes.h>
#include <mbedtls/sha256.h>
#include <mbedtls/base64.h>
#include <mbedtls/platform.h> // For mbedtls_calloc/free
#include <ctime> // For seeding rand()

const size_t AES_KEY_SIZE = 32;   // AES-256 key size (256 bits)
const size_t AES_BLOCK_SIZE = 16; // AES block size (128 bits)

// Derive a 32-byte key from a password using SHA-256
std::vector<unsigned char> derive_key(const std::string& password) {
    std::vector<unsigned char> key(AES_KEY_SIZE);
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts_ret(&ctx, 0); // 0 = SHA-256
    mbedtls_sha256_update_ret(&ctx, reinterpret_cast<const unsigned char*>(password.c_str()), password.length());
    mbedtls_sha256_finish_ret(&ctx, key.data());
    mbedtls_sha256_free(&ctx);
    return key;
}

// Portable random bytes generator
std::vector<unsigned char> generate_random_bytes(size_t length) {
    std::vector<unsigned char> buffer(length);
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__) // Unix-like systems
    FILE* f = fopen("/dev/urandom", "rb");
    if (!f) {
        throw std::runtime_error("Failed to open /dev/urandom");
    }
    if (fread(buffer.data(), 1, length, f) != length) {
        fclose(f);
        throw std::runtime_error("Failed to read random bytes");
    }
    fclose(f);
#else // Fallback for other platforms (e.g., Windows)
    // Warning: This is less secure; replace with platform-specific RNG (e.g., CryptGenRandom on Windows)
    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned>(time(nullptr)));
        seeded = true;
    }
    for (size_t i = 0; i < length; ++i) {
        buffer[i] = static_cast<unsigned char>(rand() % 256);
    }
#endif
    return buffer;
}

std::string encrypt(const std::string& plain_text, const std::string& password) {
    auto key = derive_key(password);

    // Generate random IV
    auto iv = generate_random_bytes(AES_BLOCK_SIZE);
    std::vector<unsigned char> iv_copy = iv; // Preserve original IV

    // Calculate padded length and apply PKCS7 padding
    size_t input_len = plain_text.length();
    size_t padded_len = ((input_len / AES_BLOCK_SIZE) + 1) * AES_BLOCK_SIZE;
    std::vector<unsigned char> input(padded_len, 0);
    memcpy(input.data(), plain_text.c_str(), input_len);
    unsigned char pad_val = static_cast<unsigned char>(padded_len - input_len);
    for (size_t i = input_len; i < padded_len; ++i) {
        input[i] = pad_val;
    }

    // Encrypt
    std::vector<unsigned char> output(padded_len);
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    if (mbedtls_aes_setkey_enc(&aes, key.data(), AES_KEY_SIZE * 8) != 0) {
        mbedtls_aes_free(&aes);
        throw std::runtime_error("AES key setup failed");
    }
    if (mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, padded_len, iv.data(), input.data(), output.data()) != 0) {
        mbedtls_aes_free(&aes);
        throw std::runtime_error("AES encryption failed");
    }
    mbedtls_aes_free(&aes);

    // Combine IV and ciphertext
    std::vector<unsigned char> final_output;
    final_output.reserve(AES_BLOCK_SIZE + padded_len);
    final_output.insert(final_output.end(), iv_copy.begin(), iv_copy.end());
    final_output.insert(final_output.end(), output.begin(), output.end());

    // Base64 encode
    size_t base64_len = 0;
    size_t max_base64_len = ((final_output.size() + 2) / 3) * 4 + 1;
    std::vector<unsigned char> base64_output(max_base64_len);
    if (mbedtls_base64_encode(base64_output.data(), max_base64_len, &base64_len, final_output.data(), final_output.size()) != 0) {
        throw std::runtime_error("Base64 encoding failed");
    }
    return std::string(reinterpret_cast<char*>(base64_output.data()), base64_len);
}

std::string decrypt(const std::string& encrypted_base64, const std::string& password) {
    auto key = derive_key(password);

    // Base64 decode
    size_t decoded_len = 0;
    size_t max_decoded_len = ((encrypted_base64.length() + 3) / 4) * 3;
    std::vector<unsigned char> decoded(max_decoded_len);
    if (mbedtls_base64_decode(decoded.data(), max_decoded_len, &decoded_len, 
                              reinterpret_cast<const unsigned char*>(encrypted_base64.c_str()), encrypted_base64.length()) != 0) {
        throw std::runtime_error("Base64 decoding failed");
    }
    if (decoded_len < AES_BLOCK_SIZE) {
        throw std::runtime_error("Invalid encrypted data: too short");
    }

    // Extract IV and ciphertext
    std::vector<unsigned char> iv(decoded.begin(), decoded.begin() + AES_BLOCK_SIZE);
    std::vector<unsigned char> ciphertext(decoded.begin() + AES_BLOCK_SIZE, decoded.begin() + decoded_len);

    // Decrypt
    std::vector<unsigned char> output(ciphertext.size());
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    if (mbedtls_aes_setkey_dec(&aes, key.data(), AES_KEY_SIZE * 8) != 0) {
        mbedtls_aes_free(&aes);
        throw std::runtime_error("AES key setup failed");
    }
    if (mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, ciphertext.size(), iv.data(), ciphertext.data(), output.data()) != 0) {
        mbedtls_aes_free(&aes);
        throw std::runtime_error("AES decryption failed");
    }
    mbedtls_aes_free(&aes);

    // Remove PKCS7 padding
    unsigned char pad_val = output.back();
    if (pad_val == 0 || pad_val > AES_BLOCK_SIZE) {
        throw std::runtime_error("Invalid padding value: " + std::to_string(pad_val));
    }
    bool valid_padding = true;
    for (size_t i = output.size() - pad_val; i < output.size(); ++i) {
        if (output[i] != pad_val) {
            valid_padding = false;
            break;
        }
    }
    if (!valid_padding) {
        throw std::runtime_error("Invalid PKCS7 padding");
    }
    size_t unpadded_len = output.size() - pad_val;

    return std::string(output.begin(), output.begin() + unpadded_len);
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <enc|dec> <password> <text>" << std::endl;
        return 1;
    }

    std::string command = argv[1];
    std::string password = argv[2];
    std::string text = argv[3];

    try {
        if (command == "enc") {
            std::string encrypted = encrypt(text, password);
            std::cout << "Encrypted: " << encrypted << std::endl;
        } else if (command == "dec") {
            std::string decrypted = decrypt(text, password);
            std::cout << "Decrypted: " << decrypted << std::endl;
        } else {
            std::cerr << "Invalid command. Use 'enc' or 'dec'." << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}