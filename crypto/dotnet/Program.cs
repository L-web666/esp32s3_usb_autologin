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
            Console.WriteLine("Examples:");
            Console.WriteLine("  dotnet run enc \"mypassword\" \"Hello World\"");
            //Console.WriteLine("  dotnet run dec \"mypassword\" \"<encryptedtext>\"");
            Console.WriteLine("  dotnet run dec \"mypassword\" \"FDNGl0K7cGFQQF6AmN/67PWq3Iouxd1bKPrLcmNFyTE=\"");
            return;
        }

        string command = args[0].ToLower();
        string password = args[1];
        string text = args[2];

        //Console.WriteLine($"Command: {command}, Password: '{password}', Text: '{text}'");

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
                //Console.WriteLine($"Plain bytes: {BitConverter.ToString(plainBytes)}");
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