Let’s walk through the complete process of setting up an ESP32 client with certificate-based authentication for Azure Event Grid’s MQTT broker. This will cover everything from generating certificates, configuring Azure Event Grid, updating your ESP32 code, and testing the connection. I’ll assume you’re starting fresh and want a secure, scalable setup with an intermediate CA signing unique client certificates for multiple ESP32 devices.

---

### **Overview of the Process**
1. **Generate Certificates**: Create a root CA, an intermediate CA, and a client certificate for your ESP32.
2. **Configure Azure Event Grid**: Set up an Event Grid namespace, enable MQTT, and register the intermediate CA.
3. **Update ESP32 Code**: Modify your code to load the certificates and connect to Azure.
4. **Test the Connection**: Verify the ESP32 can publish and subscribe to topics.

---

### **Step 1: Generate Certificates**
We’ll use OpenSSL to create a root CA, an intermediate CA, and a client certificate for one ESP32 device. You can repeat the client certificate step for additional devices.

#### **1.1 Install OpenSSL**
- On Windows: Install via Git Bash, WSL, or a standalone OpenSSL package.
- On Linux/Mac: `sudo apt install openssl` or `brew install openssl`.

#### **1.2 Create a Root CA**
This is your top-level authority (keep it offline for security in production).
```bash
# Generate root CA private key
openssl genrsa -out root-ca.key 2048

# Create root CA certificate (valid for 10 years)
openssl req -x509 -new -nodes -key root-ca.key -sha256 -days 3650 -out root-ca.pem -subj "/CN=MyRootCA"
```

#### **1.3 Create an Intermediate CA**
This signs client certificates and gets uploaded to Azure.
```bash
# Generate intermediate CA private key
openssl genrsa -out intermediate-ca.key 2048

# Create a certificate signing request (CSR) for the intermediate CA
openssl req -new -key intermediate-ca.key -out intermediate-ca.csr -subj "/CN=MyIntermediateCA"

# Sign the intermediate CA with the root CA (valid for 5 years)
openssl x509 -req -in intermediate-ca.csr -CA root-ca.pem -CAkey root-ca.key -CAcreateserial -out intermediate-ca.pem -days 1825 -sha256

# Clean up CSR
rm intermediate-ca.csr
```

#### **1.4 Create a Client Certificate for ESP32**
This is specific to one device (e.g., `esp32-1`).
```bash
# Generate client private key
openssl genrsa -out client-key-esp32-1.pem 2048

# Create a CSR for the client
openssl req -new -key client-key-esp32-1.pem -out esp32-1.csr -subj "/CN=esp32-1"

# Sign the client certificate with the intermediate CA (valid for 1 year)
openssl x509 -req -in esp32-1.csr -CA intermediate-ca.pem -CAkey intermediate-ca.key -CAcreateserial -out client-cert-esp32-1.pem -days 365 -sha256

# Clean up CSR
rm esp32-1.csr
```

#### **1.5 Download Azure’s Root CA**
Azure Event Grid’s server certificate is signed by DigiCert Global Root G2:
- Download from [DigiCert](https://www.digicert.com/kb/digicert-root-certificates.htm) (select "DigiCert Global Root G2" in PEM format).
- Save as `digicert_global_root_g2.pem`.

#### **1.6 Verify Files**
You should now have:
- `root-ca.pem` and `root-ca.key` (keep secure, don’t upload to Azure or ESP32).
- `intermediate-ca.pem` and `intermediate-ca.key` (upload `.pem` to Azure).
- `client-cert-esp32-1.pem` and `client-key-esp32-1.pem` (load onto ESP32).
- `digicert_global_root_g2.pem` (load onto ESP32 for server verification).

---

### **Step 2: Configure Azure Event Grid**
You’ll need an Azure subscription and the Azure CLI installed (`az login` to authenticate).

#### **2.1 Create a Resource Group**
```bash
az group create --name MyIoTResourceGroup --location westus
```

#### **2.2 Create an Event Grid Namespace**
```bash
az eventgrid namespace create \
  --name MyEventGridNamespace \
  --resource-group MyIoTResourceGroup \
  --location westus \
  --topic-type mqtt \
  --sku Standard
```

#### **2.3 Enable MQTT**
Ensure MQTT is enabled (it’s on by default for new namespaces with `--topic-type mqtt`):
```bash
az eventgrid namespace update \
  --name MyEventGridNamespace \
  --resource-group MyIoTResourceGroup \
  --is-mqtt-broker-enabled true
```

#### **2.4 Register the Intermediate CA**
Upload your intermediate CA certificate to trust all client certificates it signs:
```bash
az eventgrid namespace ca-certificate create \
  --resource-group MyIoTResourceGroup \
  --namespace-name MyEventGridNamespace \
  --name MyIntermediateCA \
  --certificate intermediate-ca.pem
```

#### **2.5 Create a Client Group**
Group your ESP32 devices for permissions:
```bash
az eventgrid namespace client-group create \
  --resource-group MyIoTResourceGroup \
  --namespace-name MyEventGridNamespace \
  --name Esp32Devices \
  --description "Group for ESP32 IoT devices" \
  --query-expression "client.certificate.subject.CN LIKE 'esp32-*'"
```
- This matches clients with CNs like `esp32-1`, `esp32-2`, etc.

#### **2.6 Create a Topic Space**
Define topics your devices can use:
```bash
az eventgrid namespace topic-space create \
  --resource-group MyIoTResourceGroup \
  --namespace-name MyEventGridNamespace \
  --name Esp32TopicSpace \
  --topic-templates "devices/esp32-*/messages"
```
- `devices/esp32-*/messages` allows topics like `devices/esp32-1/messages`.

#### **2.7 Assign Permissions**
Grant the client group publish and subscribe rights:
```bash
az eventgrid namespace permission-binding create \
  --resource-group MyIoTResourceGroup \
  --namespace-name MyEventGridNamespace \
  --name Esp32Permission \
  --client-group-name Esp32Devices \
  --topic-space-name Esp32TopicSpace \
  --permission publisher \
  --permission subscriber
```

#### **2.8 Get the MQTT Endpoint**
Retrieve your namespace’s hostname:
```bash
az eventgrid namespace show \
  --resource-group MyIoTResourceGroup \
  --name MyEventGridNamespace \
  --query "topicSpacesConfiguration.hostname"
```
- Output: e.g., `myeventgridnamespace.westus-1.ts.eventgrid.azure.net`.

---

### **Step 3: Update ESP32 Code**
Modify your code to load the certificates and connect to Azure.

#### **3.1 Upload Certificates to LittleFS**
- Use the Arduino IDE’s "ESP32 Sketch Data Upload" tool or PlatformIO’s `data` folder.
- Upload:
  - `digicert_global_root_g2.pem`
  - `client-cert-esp32-1.pem`
  - `client-key-esp32-1.pem`

#### **3.2 Update `MqttHandler` Code**
Replace your certificate loading and connection logic:
```cpp
bool MqttHandler::loadCertificates(String& caCert, String& clientCert, String& clientKey) {
  if (!LittleFS.begin()) {
    debugE("MQTT: Failed to initialize LittleFS");
    return false;
  }

  String certPath = "/client-cert-" + settings.device.name + ".pem";
  String keyPath = "/client-key-" + settings.device.name + ".pem";

  File caFile = LittleFS.open("/digicert_global_root_g2.pem", "r");
  File certFile = LittleFS.open(certPath.c_str(), "r");
  File keyFile = LittleFS.open(keyPath.c_str(), "r");

  if (!caFile || !certFile || !keyFile) {
    debugE("MQTT: Failed to open certificate files");
    caFile.close(); certFile.close(); keyFile.close();
    return false;
  }

  caCert = caFile.readString();
  clientCert = certFile.readString();
  clientKey = keyFile.readString();

  caFile.close();
  certFile.close();
  keyFile.close();

  debugV("MQTT: Loaded CA (%d bytes), Cert (%d bytes), Key (%d bytes)", 
         caCert.length(), clientCert.length(), clientKey.length());
  return true;
}

void MqttHandler::connectToMqtt() {
  debugI("MQTT: Configuring secure connection");
  String caCert, clientCert, clientKey;
  if (!loadCertificates(caCert, clientCert, clientKey)) {
    return;
  }

  wifiClientSecure.setCACert(caCert.c_str());
  wifiClientSecure.setCertificate(clientCert.c_str());
  wifiClientSecure.setPrivateKey(clientKey.c_str());
  mqttClient.setClient(wifiClientSecure);

  // Set Azure Event Grid server and port
  settings.mqtt.server = "myeventgridnamespace.westus-1.ts.eventgrid.azure.net"; // Update with your hostname
  settings.mqtt.port = 8883;
  settings.mqtt.subTopic = "devices/" + settings.device.name + "/messages";
  settings.mqtt.pubTopic = "devices/" + settings.device.name + "/messages";

  mqttClient.setServer(settings.mqtt.server.c_str(), settings.mqtt.port);
  mqttClient.setCallback(handleMqttCallback);

  debugI("MQTT: Connecting to %s:%d", settings.mqtt.server.c_str(), settings.mqtt.port);

  // Connect with client ID (certificate handles auth)
  bool success = mqttClient.connect(settings.device.name.c_str());
  if (success) {
    debugI("MQTT: Connected as [%s]", settings.device.name.c_str());
    mqttClient.subscribe(settings.mqtt.subTopic.c_str());
    debugI("MQTT: Subscribed to [%s]", settings.mqtt.subTopic.c_str());

    String helloMessage = "Device: " + settings.device.name + " connected.";
    publish(settings.mqtt.pubTopic.c_str(), helloMessage.c_str());
    settings.mqtt.isConnected = true;

    currentBackoffMs = DEFAULT_TIMEOUT_MS;
    mqttReconnectTimer.setInterval(currentBackoffMs);
  } else {
    debugE("MQTT: Connection failed, state=%d", mqttClient.state());
    currentBackoffMs = min(currentBackoffMs * BACKOFF_FACTOR, MAX_BACKOFF_MS);
    mqttReconnectTimer.setInterval(currentBackoffMs);
  }
}
```

#### **3.3 Set Device Name**
Ensure `settings.device.name` matches the certificate’s CN (e.g., `esp32-1`).

---

### **Step 4: Test the Connection**
1. **Upload Code**: Flash your updated sketch to the ESP32.
2. **Monitor Serial Output**: Check for connection success or errors (e.g., `state=-2` means TLS failure).
3. **Test with MQTT Explorer**:
   - Connect to `myeventgridnamespace.westus-1.ts.eventgrid.azure.net:8883` with another client cert signed by the same intermediate CA.
   - Publish to `devices/esp32-1/messages` and verify the ESP32 receives it.

---

### **Scaling to Multiple Clients**
- For each new ESP32 (e.g., `esp32-2`):
  1. Generate a new client cert/key pair signed by `intermediate-ca.pem`.
  2. Upload to LittleFS with unique names (e.g., `client-cert-esp32-2.pem`).
  3. Update `settings.device.name` to `esp32-2`.
- The intermediate CA in Azure trusts all these certs, so no additional Azure config is needed.

---

### **Troubleshooting**
- **TLS Errors**: Verify certificate formats (PEM, no extra spaces), and ensure the ESP32 has enough memory (~10KB free for certs + TLS stack).
- **Permission Denied**: Check topic names match the topic space and client group permissions.
- **Connection Refused**: Confirm the intermediate CA is correctly uploaded and active in Azure.

---

### **Summary**
You’ve now set up a secure MQTT client with certificate auth for Azure Event Grid. The intermediate CA approach scales well, keeping Azure config simple while allowing unique identities per ESP32. Let me know if you hit any snags during implementation!