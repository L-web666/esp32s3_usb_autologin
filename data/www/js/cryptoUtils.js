// cryptoUtils.js
// Assumes CryptoJS is already loaded in the global scope via HTML <script> tag
// <script src="https://cdnjs.cloudflare.com/ajax/libs/crypto-js/4.2.0/crypto-js.min.js"></script>

export function encrypt(password, text) {
    if (!password || !text) {
      throw new Error("Password and text are required for encryption.");
    }
  
    const key = CryptoJS.SHA256(password);
    const iv = CryptoJS.lib.WordArray.random(16);
    const encrypted = CryptoJS.AES.encrypt(text, key, {
      iv: iv,
      mode: CryptoJS.mode.CBC,
      padding: CryptoJS.pad.Pkcs7
    });
  
    return CryptoJS.enc.Base64.stringify(iv.concat(encrypted.ciphertext));
  }
  
  export function decrypt(password, encryptedBase64) {
    if (!password || !encryptedBase64) {
      throw new Error("Password and encrypted text are required for decryption.");
    }
  
    const key = CryptoJS.SHA256(password);
    const encryptedRaw = CryptoJS.enc.Base64.parse(encryptedBase64);
    const iv = CryptoJS.lib.WordArray.create(encryptedRaw.words.slice(0, 4));
    const ciphertext = CryptoJS.lib.WordArray.create(encryptedRaw.words.slice(4));
    const decrypted = CryptoJS.AES.decrypt({ ciphertext: ciphertext }, key, {
      iv: iv,
      mode: CryptoJS.mode.CBC,
      padding: CryptoJS.pad.Pkcs7
    });
  
    return decrypted.toString(CryptoJS.enc.Utf8);
  }

// Example of using encrypt/decrypt elsewhere
// const myPassword = "secret123";
// const myText = "Hello, World!";
// const encrypted = encrypt(myPassword, myText);
// console.log("Encrypted:", encrypted);
// const decrypted = decrypt(myPassword, encrypted);
// console.log("Decrypted:", decrypted);