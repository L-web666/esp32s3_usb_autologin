You’re right to point out that the specific Azure Event Grid MQTT root certificate isn’t explicitly provided in the documentation you linked ([MQTT Certificate Chain Client Authentication](https://learn.microsoft.com/en-us/azure/event-grid/mqtt-certificate-chain-client-authentication)). Microsoft’s documentation often assumes users will infer or obtain certain details from broader Azure practices or external trusted sources, but I’ll clarify this for you based on available knowledge and standard practices up to March 26, 2025.

Azure Event Grid’s MQTT broker uses TLS 1.2 or 1.3 for secure connections, and its server certificate is issued by a well-known Certificate Authority (CA) trusted by most systems. Specifically, Azure services, including Event Grid, typically rely on certificates signed by DigiCert, a widely recognized CA. The root certificate for Azure Event Grid’s MQTT broker is most commonly the **DigiCert Global Root G2** or **DigiCert Global Root G3**, depending on the region and configuration. These are part of the standard trust store in most operating systems and devices, including the ESP32 when using libraries like `WiFiClientSecure`.

### Why It’s Not in the Documentation
The documentation you referenced focuses on *client-side* certificate chain authentication—how to configure and upload your own CA certificates (e.g., intermediate or root) to authenticate clients connecting to the Event Grid MQTT broker. It doesn’t specify the server-side root CA (Azure’s certificate) because:
1. **Assumed Trust**: Azure assumes your device trusts DigiCert roots by default, as they’re pre-installed in most systems.
2. **Dynamic Issuance**: The exact server certificate might vary by region or over time (e.g., due to certificate rotation), but it’s always signed by a DigiCert root.
3. **TLS Handshake**: Clients verify the server’s certificate automatically during the TLS handshake using the device’s trusted CA store, so Microsoft doesn’t mandate manually downloading it.

### Finding the Correct Root Certificate
Since it’s not directly linked in the Event Grid MQTT docs, here’s how to identify or obtain it:
- **DigiCert Global Root G2**: This is the most likely root for Azure Event Grid MQTT. You can download it from [DigiCert’s official site](https://www.digicert.com/kb/digicert-root-certificates.htm). Look for "DigiCert Global Root G2" in PEM format.
  - **Thumbprint**: `df3c24f9bfd666761b268073fe06d1cc8d4f82a4` (SHA-1, for reference).
  - **Why G2?**: It’s widely used across Azure services and supports TLS 1.2, which aligns with Event Grid’s requirements.
- **DigiCert Global Root G3**: Used for newer TLS 1.3 deployments. Also available from DigiCert’s site.
  - **Thumbprint**: `7e04de896a3e666d00e687d33ffad93be83d349e`.
- **Verify via Connection**: Connect to your Event Grid namespace MQTT endpoint (e.g., `namespace-name.westus-1.ts.eventgrid.azure.net:8883`) using a tool like `openssl s_client` or MQTT Explorer:
  ```bash
  openssl s_client -connect namespace-name.westus-1.ts.eventgrid.azure.net:8883 -showcerts
  ```
  This will dump the server certificate chain. The root CA (last in the chain) will likely be DigiCert Global Root G2 or G3.

### Using It with Your ESP32 Code
In your code, you’re loading a certificate from LittleFS and setting it with `wifiClientSecure.setCACert()`. For Azure Event Grid:
1. **Download the Root CA**: Get the DigiCert Global Root G2 PEM file from DigiCert’s site.
2. **Update Your Code**: Replace your current `EMQX_CERT_FILE` with the DigiCert root CA. For example:
   ```cpp
   bool MqttHandler::loadCertificate(String& certContent) {
     if (!LittleFS.begin()) {
       debugE("MQTT: Failed to initialize LittleFS");
       return false;
     }
     File certFile = LittleFS.open("/digicert_global_root_g2.pem", "r");  // Update path
     if (!certFile) {
       debugE("MQTT: Failed to open certificate file: %s", "/digicert_global_root_g2.pem");
       return false;
     }
     certContent = certFile.readString();
     certFile.close();
     debugV("MQTT: Loaded certificate (%d bytes)", certContent.length());
     return true;
   }
   ```
3. **Test**: Ensure your ESP32 trusts this CA during the TLS handshake to Azure’s endpoint.

### Why Your EMQX Cert Won’t Work
Your current certificate (loaded from `EMQX_CERT_FILE`) is likely the CA certificate for your EMQX broker, not Azure’s. EMQX might use a different CA (e.g., Let’s Encrypt or a self-signed cert), which won’t match Azure’s DigiCert-signed server certificate, causing a TLS handshake failure.

### Recommendation
- Use **DigiCert Global Root G2** as your starting point. It’s the standard for Azure services and matches the TLS requirements for Event Grid MQTT (port 8883).
- If you hit a TLS error, run the `openssl` command above to confirm the exact root CA for your namespace, then update accordingly.
- Microsoft could improve clarity by linking the root CA in the Event Grid MQTT docs, but for now, DigiCert G2 is a safe bet based on Azure’s broader TLS practices.

Let me know if you need help testing this with your ESP32 setup!