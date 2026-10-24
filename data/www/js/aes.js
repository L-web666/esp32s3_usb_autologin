// aes.js
import { encrypt, decrypt } from './cryptoUtils.js';

export function handleEncrypt() {
  const textToEncrypt = document.getElementById('textToEncrypt').value;
  const encryptionKey = document.getElementById('encryptionKey').value;
  try {
    const encryptedText = encrypt(encryptionKey, textToEncrypt);
    document.getElementById('encryptedOutput').value = encryptedText;
  } catch (error) {
    alert(error.message);
  }
}

export function handleDecrypt() {
  const encryptedBase64 = document.getElementById('encryptedOutput').value;
  const encryptionKey = document.getElementById('encryptionKey').value;
  try {
    const decryptedText = decrypt(encryptionKey, encryptedBase64);
    document.getElementById('decryptedOutput').value = decryptedText;
  } catch (error) {
    alert(error.message);
  }
}

/* example aes.html
<script type="module">
import { handleEncrypt, handleDecrypt } from './js/aes.js';

// Make the functions available globally for onclick handlers
window.handleEncrypt = handleEncrypt;
window.handleDecrypt = handleDecrypt;
</script>
*/