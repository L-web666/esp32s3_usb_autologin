
---

### Option 1: Go (Golang) - Simple and Portable
**Why Go?**
- **Built-in Crypto**: Go’s standard library (`crypto/aes`, `crypto/sha256`, `encoding/base64`) provides everything you need without external dependencies.
- **Simplicity**: No manual memory management, concise syntax, and built-in error handling.
- **Portability**: Compiles to standalone binaries for any platform (Linux, Windows, macOS, etc.).
- **Performance**: Fast and efficient, comparable to C/C++.

#### Go Implementation
```go
package main

import (
	"crypto/aes"
	"crypto/cipher"
	"crypto/rand"
	"crypto/sha256"
	"encoding/base64"
	"fmt"
	"os"
	"strings"
)

const blockSize = 16 // AES block size

func encrypt(plainText, password string) (string, error) {
	key := sha256.Sum256([]byte(password)) // SHA-256 key derivation

	// Generate random IV
	iv := make([]byte, blockSize)
	if _, err := rand.Read(iv); err != nil {
		return "", fmt.Errorf("failed to generate IV: %v", err)
	}

	// Pad plaintext with PKCS7
	padLen := blockSize - (len(plainText) % blockSize)
	padText := make([]byte, len(plainText)+padLen)
	copy(padText, plainText)
	for i := len(plainText); i < len(padText); i++ {
		padText[i] = byte(padLen)
	}

	// Encrypt
	block, err := aes.NewCipher(key[:])
	if err != nil {
		return "", fmt.Errorf("failed to create cipher: %v", err)
	}
	cipherText := make([]byte, len(padText))
	mode := cipher.NewCBCEncrypter(block, iv)
	mode.CryptBlocks(cipherText, padText)

	// Prepend IV and encode
	finalOutput := append(iv, cipherText...)
	return base64.StdEncoding.EncodeToString(finalOutput), nil
}

func decrypt(encryptedBase64, password string) (string, error) {
	key := sha256.Sum256([]byte(password))

	// Decode Base64
	encryptedBytes, err := base64.StdEncoding.DecodeString(encryptedBase64)
	if err != nil {
		return "", fmt.Errorf("base64 decode failed: %v", err)
	}
	if len(encryptedBytes) < blockSize {
		return "", fmt.Errorf("invalid encrypted data: too short")
	}

	// Extract IV and ciphertext
	iv := encryptedBytes[:blockSize]
	cipherText := encryptedBytes[blockSize:]

	// Decrypt
	block, err := aes.NewCipher(key[:])
	if err != nil {
		return "", fmt.Errorf("failed to create cipher: %v", err)
	}
	plainText := make([]byte, len(cipherText))
	mode := cipher.NewCBCDecrypter(block, iv)
	mode.CryptBlocks(plainText, cipherText)

	// Remove PKCS7 padding
	padLen := int(plainText[len(plainText)-1])
	if padLen > blockSize || padLen <= 0 {
		return "", fmt.Errorf("invalid padding value: %d", padLen)
	}
	for i := len(plainText) - padLen; i < len(plainText); i++ {
		if plainText[i] != byte(padLen) {
			return "", fmt.Errorf("invalid PKCS7 padding")
		}
	}
	return string(plainText[:len(plainText)-padLen]), nil
}

func main() {
	if len(os.Args) < 4 {
		fmt.Printf("Usage: %s <enc|dec> <password> <text>\n", os.Args[0])
		os.Exit(1)
	}

	command := strings.ToLower(os.Args[1])
	password := os.Args[2]
	text := os.Args[3]

	switch command {
	case "enc":
		encrypted, err := encrypt(text, password)
		if err != nil {
			fmt.Fprintf(os.Stderr, "Error: %v\n", err)
			os.Exit(1)
		}
		fmt.Println("Encrypted:", encrypted)
	case "dec":
		decrypted, err := decrypt(text, password)
		if err != nil {
			fmt.Fprintf(os.Stderr, "Error: %v\n", err)
			os.Exit(1)
		}
		fmt.Println("Decrypted:", decrypted)
	default:
		fmt.Println("Invalid command. Use 'enc' or 'dec'.")
		os.Exit(1)
	}
}
```

#### Setup and Run
1. **Install Go**: Download from [go.dev](https://go.dev/dl/) (e.g., `go1.23.0`).
2. **Save as `crypto.go`**:
   ```bash
   mkdir crypto-go
   cd crypto-go
   # Save code in crypto.go
   ```
3. **Run**:
   ```bash
   go run crypto.go enc "test" "#123"
   ```
   Output: `Encrypted: <base64 string>`
   ```bash
   go run crypto.go dec "test" "<base64 string>"
   ```
   Output: `Decrypted: #123`
4. **Build Binary**:
   ```bash
   go build -o crypto
   ./crypto enc "test" "#123"
   ```

#### Pros
- ~70 lines of code (vs. 130+ in C/C++).
- No external dependencies (pure standard library).
- Cross-platform binary with `GOOS`/`GOARCH` (e.g., `GOOS=windows go build`).

#### Cons
- Learning Go if you’re unfamiliar (though it’s simple).

---

### Option 2: Rust - Safe and Modern
**Why Rust?**
- **Crypto Crates**: `aes`, `sha2`, `base64` crates are well-maintained.
- **Safety**: Memory safety without garbage collection.
- **Portability**: Compiles to any platform.

#### Rust Implementation
```rust
use std::env;
use aes::Aes256;
use block_modes::{BlockMode, Cbc};
use block_modes::block_padding::Pkcs7;
use sha2::{Sha256, Digest};
use base64;
use rand::Rng;

type Aes256Cbc = Cbc<Aes256, Pkcs7>;

fn encrypt(plain_text: &str, password: &str) -> Result<String, Box<dyn std::error::Error>> {
    let key = Sha256::digest(password.as_bytes());
    let iv: [u8; 16] = rand::thread_rng().gen();

    let cipher = Aes256Cbc::new_from_slices(&key, &iv)?;
    let cipher_text = cipher.encrypt_vec(plain_text.as_bytes());

    let final_output = [iv.as_slice(), cipher_text.as_slice()].concat();
    Ok(base64::encode(final_output))
}

fn decrypt(encrypted_base64: &str, password: &str) -> Result<String, Box<dyn std::error::Error>> {
    let key = Sha256::digest(password.as_bytes());
    let encrypted_bytes = base64::decode(encrypted_base64)?;
    if encrypted_bytes.len() < 16 {
        return Err("Invalid encrypted data: too short".into());
    }

    let iv = &encrypted_bytes[..16];
    let cipher_text = &encrypted_bytes[16..];

    let cipher = Aes256Cbc::new_from_slices(&key, iv)?;
    let plain_bytes = cipher.decrypt_vec(cipher_text)?;
    Ok(String::from_utf8(plain_bytes)?)
}

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 4 {
        eprintln!("Usage: {} <enc|dec> <password> <text>", args[0]);
        std::process::exit(1);
    }

    let command = args[1].to_lowercase();
    let password = &args[2];
    let text = &args[3];

    match command.as_str() {
        "enc" => {
            match encrypt(text, password) {
                Ok(encrypted) => println!("Encrypted: {}", encrypted),
                Err(e) => eprintln!("Error: {}", e),
            }
        }
        "dec" => {
            match decrypt(text, password) {
                Ok(decrypted) => println!("Decrypted: {}", decrypted),
                Err(e) => eprintln!("Error: {}", e),
            }
        }
        _ => eprintln!("Invalid command. Use 'enc' or 'dec'."),
    }
}
```

#### Setup and Run
1. **Install Rust**: [rustup.rs](https://rustup.rs/)
2. **Create Project**:
   ```bash
   cargo new crypto-rust
   cd crypto-rust
   ```
3. **Edit `Cargo.toml`**:
   ```toml
   [package]
   name = "crypto-rust"
   version = "0.1.0"
   edition = "2021"

   [dependencies]
   aes = "0.8"
   block-modes = "0.9"
   sha2 = "0.10"
   base64 = "0.22"
   rand = "0.8"
   ```
4. **Replace `src/main.rs`** with the code above.
5. **Run**:
   ```bash
   cargo run -- enc "test" "#123"
   ```
   ```bash
   cargo run -- dec "test" "<base64 string>"
   ```

#### Pros
- ~60 lines, concise and safe.
- Modern language with strong community support.

#### Cons
- Requires external crates (though minimal).
- Learning Rust if new to it.

---

### Option 3: Node.js (JavaScript) - Easiest with Built-ins
Since you already have a JavaScript version with CryptoJS, Node.js offers a simpler alternative using its built-in `crypto` module, avoiding external dependencies.

#### Node.js Implementation
```javascript
const crypto = require('crypto');

function encrypt(plainText, password) {
    const key = crypto.createHash('sha256').update(password).digest();
    const iv = crypto.randomBytes(16);
    const cipher = crypto.createCipheriv('aes-256-cbc', key, iv);
    let encrypted = cipher.update(plainText, 'utf8', 'binary');
    encrypted += cipher.final('binary');
    return Buffer.concat([iv, Buffer.from(encrypted, 'binary')]).toString('base64');
}

function decrypt(encryptedBase64, password) {
    const key = crypto.createHash('sha256').update(password).digest();
    const encryptedBytes = Buffer.from(encryptedBase64, 'base64');
    if (encryptedBytes.length < 16) throw new Error('Invalid encrypted data: too short');
    const iv = encryptedBytes.slice(0, 16);
    const cipherText = encryptedBytes.slice(16);
    const decipher = crypto.createDecipheriv('aes-256-cbc', key, iv);
    let decrypted = decipher.update(cipherText, 'binary', 'utf8');
    decrypted += decipher.final('utf8');
    return decrypted;
}

const [,, command, password, text] = process.argv;
if (!command || !password || !text) {
    console.error(`Usage: node ${process.argv[1]} <enc|dec> <password> <text>`);
    process.exit(1);
}

try {
    if (command === 'enc') {
        console.log('Encrypted:', encrypt(text, password));
    } else if (command === 'dec') {
        console.log('Decrypted:', decrypt(text, password));
    } else {
        console.error("Invalid command. Use 'enc' or 'dec'.");
    }
} catch (e) {
    console.error('Error:', e.message);
}
```

#### Run
```bash
node crypto.js enc "test" "#123"
node crypto.js dec "test" "<base64 string>"
```

#### Pros
- ~30 lines, simplest yet.
- No external deps (Node.js built-in).
- Familiar if you know JavaScript.

#### Cons
- Requires Node.js runtime, not a standalone binary.

---

### Recommendation: Go
- **Why**: Go strikes the best balance of simplicity (~70 lines), portability (standalone binary), and no external dependencies beyond the standard library. It’s easier than C/C++ (no memory management) and more self-contained than Python/Rust (no package manager needed post-compilation).
- **Ease**: Similar to Python but with better performance and no virtual environment setup.
- **Compatibility**: Matches your exact scheme, interoperable with all your versions.

#### Full Test
```bash
go run crypto.go enc "test" "#123"  # e.g., Xz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8=
./crypto dec test Xz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8=  # C version
python crypto.py dec "test" "Xz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8="  # Python
```
All should output `#123`.

If you want even simpler (and don’t mind a runtime), Node.js is the easiest. Let me know which you’d like to pursue or if you have another language in mind!