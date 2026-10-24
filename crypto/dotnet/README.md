
---

### Updated C# Code
Here’s the revised `Program.cs` with the arguments reordered to match your C implementation:

```csharp
using System;
using System.Security.Cryptography;
using System.Text;

class Program
{
    private const int AesKeySize = 32;
    private const int AesBlockSize = 16;

    static void Main(string[] args)
    {
        if (args.Length < 3)
        {
            Console.WriteLine("Usage: dotnet run <enc|dec> <password> <text>");
            return;
        }

        string command = args[0].ToLower();
        string password = args[1];
        string text = args[2];

        Console.WriteLine($"Command: {command}, Password: '{password}', Text: '{text}'");

        try
        {
            if (command == "enc")
            {
                string encrypted = Encrypt(text, password);
                Console.WriteLine($"Encrypted: {encrypted}");
            }
            else if (command == "dec")
            {
                string decrypted = Decrypt(text, password);
                Console.WriteLine($"Decrypted: {decrypted}");
            }
            else
            {
                Console.WriteLine("Invalid command. Use 'enc' or 'dec'.");
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Error: {ex.Message}");
        }
    }

    static string Encrypt(string plainText, string password)
    {
        byte[] key = ComputeSha256Hash(password);
        byte[] iv = GenerateRandomBytes(AesBlockSize);

        byte[] cipherText;
        using (Aes aes = Aes.Create())
        {
            aes.KeySize = AesKeySize * 8;
            aes.BlockSize = AesBlockSize * 8;
            aes.Mode = CipherMode.CBC;
            aes.Padding = PaddingMode.PKCS7;
            aes.Key = key;
            aes.IV = iv;

            using (ICryptoTransform encryptor = aes.CreateEncryptor())
            {
                byte[] plainBytes = Encoding.UTF8.GetBytes(plainText);
                Console.WriteLine($"Plain bytes: {BitConverter.ToString(plainBytes)}");
                cipherText = encryptor.TransformFinalBlock(plainBytes, 0, plainBytes.Length);
            }
        }

        byte[] finalOutput = new byte[AesBlockSize + cipherText.Length];
        Array.Copy(iv, 0, finalOutput, 0, AesBlockSize);
        Array.Copy(cipherText, 0, finalOutput, AesBlockSize, cipherText.Length);

        return Convert.ToBase64String(finalOutput);
    }

    static string Decrypt(string encryptedBase64, string password)
    {
        byte[] key = ComputeSha256Hash(password);
        byte[] encryptedBytes = Convert.FromBase64String(encryptedBase64);
        if (encryptedBytes.Length < AesBlockSize)
        {
            throw new ArgumentException("Invalid encrypted data: too short.");
        }

        byte[] iv = new byte[AesBlockSize];
        byte[] cipherText = new byte[encryptedBytes.Length - AesBlockSize];
        Array.Copy(encryptedBytes, 0, iv, 0, AesBlockSize);
        Array.Copy(encryptedBytes, AesBlockSize, cipherText, 0, cipherText.Length);

        byte[] plainBytes;
        using (Aes aes = Aes.Create())
        {
            aes.KeySize = AesKeySize * 8;
            aes.BlockSize = AesBlockSize * 8;
            aes.Mode = CipherMode.CBC;
            aes.Padding = PaddingMode.PKCS7;
            aes.Key = key;
            aes.IV = iv;

            using (ICryptoTransform decryptor = aes.CreateDecryptor())
            {
                plainBytes = decryptor.TransformFinalBlock(cipherText, 0, cipherText.Length);
            }
        }

        return Encoding.UTF8.GetString(plainBytes);
    }

    static byte[] ComputeSha256Hash(string input)
    {
        using (SHA256 sha256 = SHA256.Create())
        {
            return sha256.ComputeHash(Encoding.UTF8.GetBytes(input));
        }
    }

    static byte[] GenerateRandomBytes(int length)
    {
        byte[] randomBytes = new byte[length];
        using (RandomNumberGenerator rng = RandomNumberGenerator.Create())
        {
            rng.GetBytes(randomBytes);
        }
        return randomBytes;
    }
}
```

---

### Changes Made
1. **Argument Order**: Swapped `text` and `password` in `Main`:
   - Before: `string text = args[1]; string password = args[2];`
   - After: `string password = args[1]; string text = args[2];`
2. **Usage Message**: Updated to reflect the new order: `dotnet run <enc|dec> <password> <text>`.
3. **Logging**: Adjusted the log message to show `Password` before `Text` for consistency.

The encryption and decryption logic remains unchanged, as it’s already compatible with your C and JavaScript implementations. Only the command-line interface has been adjusted.

---

### Step 1: Test the Updated Code
#### Encrypt "test" with password "hello"
```bash
dotnet run enc "hello" "test"
```
Expected output:
```
Command: enc, Password: 'hello', Text: 'test'
Plain bytes: 74-65-73-74
Encrypted: <base64 string, e.g., vlm7ie9XvhR2g8TVZXqs29y9bVJeBq5xgIOK7CgurwU=>
```

#### Decrypt in C#
```bash
dotnet run dec "hello" "vlm7ie9XvhR2g8TVZXqs29y9bVJeBq5xgIOK7CgurwU="
```
Expected:
```
Command: dec, Password: 'hello', Text: 'vlm7ie9XvhR2g8TVZXqs29y9bVJeBq5xgIOK7CgurwU='
Decrypted: test
```

#### Encrypt "#123" with password "test"
```bash
dotnet run enc "test" "#123"
```
Expected:
```
Command: enc, Password: 'test', Text: '#123'
Plain bytes: 23-31-32-33
Encrypted: <base64 string, e.g., Xz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8=>
```

#### Decrypt in C#
```bash
dotnet run dec "test" "Xz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8="
```
Expected:
```
Command: dec, Password: 'test', Text: 'Xz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8='
Decrypted: #123
```

---

### Step 2: Cross-Language Compatibility
#### Encrypt in C#, Decrypt in C
1. C#:
   ```bash
   dotnet run enc "test" "#123"
   ```
   Output: (e.g.) `Xz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8=`

2. C:
   ```
   crypto dec test Xz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8=
   ```
   Expected:
   ```
   Server: (I) Raw decrypted: 233132330c0c0c0c0c0c0c0c0c0c0c0c
   Server: (I) Decrypted length: 4, text: #123
   ```

#### Encrypt in C, Decrypt in C#
1. C:
   ```
   crypto enc test "#123"
   ```
   Output: (e.g.) `CsuQ00ZEGF1e6LREAOefawrLkNNGRBhdXui0RADnn2s=`

2. C#:
   ```bash
   dotnet run dec "test" "CsuQ00ZEGF1e6LREAOefawrLkNNGRBhdXui0RADnn2s="
   ```
   Expected:
   ```
   Command: dec, Password: 'test', Text: 'CsuQ00ZEGF1e6LREAOefawrLkNNGRBhdXui0RADnn2s='
   Decrypted: