import sys
import base64
import hashlib
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.primitives import padding
from cryptography.hazmat.backends import default_backend
import os

# Constants
AES_KEY_SIZE = 32  # 256 bits
AES_BLOCK_SIZE = 16  # 128 bits (16 bytes)

def derive_key(password: str) -> bytes:
    """Derive a 32-byte key from the password using SHA-256."""
    return hashlib.sha256(password.encode('utf-8')).digest()

def encrypt(plain_text: str, password: str) -> str:
    """Encrypt the plain text using AES-256-CBC with PKCS7 padding."""
    # Derive key
    key = derive_key(password)

    # Generate random 16-byte IV
    iv = os.urandom(AES_BLOCK_SIZE)

    # Pad the plaintext with PKCS7
    padder = padding.PKCS7(AES_BLOCK_SIZE * 8).padder()
    plain_bytes = plain_text.encode('utf-8')
    padded_data = padder.update(plain_bytes) + padder.finalize()

    # Encrypt using AES-CBC
    cipher = Cipher(algorithms.AES(key), modes.CBC(iv), backend=default_backend())
    encryptor = cipher.encryptor()
    cipher_text = encryptor.update(padded_data) + encryptor.finalize()

    # Prepend IV to ciphertext and encode in Base64
    final_output = iv + cipher_text
    return base64.b64encode(final_output).decode('utf-8')

def decrypt(encrypted_base64: str, password: str) -> str:
    """Decrypt the Base64-encoded encrypted text."""
    # Derive key
    key = derive_key(password)

    # Decode Base64
    encrypted_bytes = base64.b64decode(encrypted_base64)
    if len(encrypted_bytes) < AES_BLOCK_SIZE:
        raise ValueError("Invalid encrypted data: too short.")

    # Extract IV (first 16 bytes) and ciphertext
    iv = encrypted_bytes[:AES_BLOCK_SIZE]
    cipher_text = encrypted_bytes[AES_BLOCK_SIZE:]

    # Decrypt using AES-CBC
    cipher = Cipher(algorithms.AES(key), modes.CBC(iv), backend=default_backend())
    decryptor = cipher.decryptor()
    padded_data = decryptor.update(cipher_text) + decryptor.finalize()

    # Remove PKCS7 padding
    unpadder = padding.PKCS7(AES_BLOCK_SIZE * 8).unpadder()
    plain_bytes = unpadder.update(padded_data) + unpadder.finalize()

    return plain_bytes.decode('utf-8')

def main():
    if len(sys.argv) < 4:
        print("Usage: python crypto.py <enc|dec> <password> <text>")
        sys.exit(1)

    command = sys.argv[1].lower()
    password = sys.argv[2]
    text = sys.argv[3]

    print(f"Command: {command}, Password: '{password}', Text: '{text}'")

    try:
        if command == "enc":
            encrypted = encrypt(text, password)
            print(f"Encrypted: {encrypted}")
        elif command == "dec":
            decrypted = decrypt(text, password)
            print(f"Decrypted: {decrypted}")
        else:
            print("Invalid command. Use 'enc' or 'dec'.")
    except Exception as e:
        print(f"Error: {str(e)}")

if __name__ == "__main__":
    main()