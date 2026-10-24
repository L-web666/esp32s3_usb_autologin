#ifdef ENABLE_BLUETOOTH_HANDLER

#include "BluetoothHandler.h"
#include "Globals.h"

NimBLEServer *BluetoothHandler::pServer = nullptr;
NimBLECharacteristic *BluetoothHandler::pTxCharacteristic = nullptr;
NimBLECharacteristic *BluetoothHandler::pRxCharacteristic = nullptr;

// Server callbacks implementation
void BluetoothHandler::ServerCallbacks::onConnect(NimBLEServer *pServer)
{
    debugI("Client connected. Updating connection state...");
    // Add custom logic for when a client connects
}

void BluetoothHandler::ServerCallbacks::onDisconnect(NimBLEServer *pServer)
{
    debugI("Client disconnected. Restarting advertising...");
    // Restart advertising to allow new connections
    NimBLEDevice::startAdvertising();
}

// RX characteristic callback implementation
void BluetoothHandler::RxCallback::onWrite(NimBLECharacteristic *pCharacteristic)
{
    std::string receivedValue = pCharacteristic->getValue();
    if (receivedValue.length() > 0)
    {
        debugI("Received: %s", receivedValue.c_str());

        CommandHandler::handleCommand(receivedValue.c_str());

        // Send the response back using the TX characteristic
        if (pTxCharacteristic)
        {
            pTxCharacteristic->setValue(receivedValue);
            pTxCharacteristic->notify();
            debugI("Sent response: %s", receivedValue.c_str());
        }
    }
    else
    {
        debugW("Received an empty message.");
    }
}

// TX characteristic callback implementation
void BluetoothHandler::TxCallback::onRead(NimBLECharacteristic *pCharacteristic)
{
    debugI("TX characteristic read requested. Current value: %s", pCharacteristic->getValue().c_str());
    // Add any custom logic for when the TX characteristic is read
}

// Initialize the Nordic UART Service
void BluetoothHandler::init()
{
    debugI("Initializing BluetoothHandler...");

    // Initialize BLE
    NimBLEDevice::init(settings.deviceName.c_str());
    pServer = NimBLEDevice::createServer();

    // Attach server callbacks
    pServer->setCallbacks(new ServerCallbacks());

    // Create the Nordic UART Service
    NimBLEService *pService = pServer->createService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");

    // Create TX characteristic (Notify)
    pTxCharacteristic = pService->createCharacteristic(
        "6E400003-B5A3-F393-E0A9-E50E24DCCA9E",
        NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ);

    // Attach TX callback
    pTxCharacteristic->setCallbacks(new TxCallback());

    // Create RX characteristic (Write)
    pRxCharacteristic = pService->createCharacteristic(
        "6E400002-B5A3-F393-E0A9-E50E24DCCA9E",
        NIMBLE_PROPERTY::WRITE);

    // Attach the RX callback
    pRxCharacteristic->setCallbacks(new RxCallback());

    // Start the service and advertising
    pService->start();
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
    pAdvertising->start();

    debugI("BluetoothHandler initialized and advertising.");
}

// BLE loop
void BluetoothHandler::loop()
{
    // BLE is event-driven, so this loop can remain empty
}

#endif // ENABLE_BLUETOOTH_HANDLER
