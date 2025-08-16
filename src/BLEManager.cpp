#include "BLEManager.h"
#include "LEDController.h"
#include "WorkoutDevice.h"

BLEManager::BLEManager() 
    : pServer(nullptr), pCommandCharacteristic(nullptr), 
      pStatusCharacteristic(nullptr), deviceConnected(false),
      workoutDevice(nullptr), ledController(nullptr) {
}

void BLEManager::begin(const String& deviceName, WorkoutDevice* device, LEDController* ledCtrl) {
    Serial.println("Initializing BLE...");
    
    workoutDevice = device;
    ledController = ledCtrl;
    
    BLEDevice::init(deviceName.c_str());
    
    // Create BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks(this));
    
    // Create BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);
    
    // Create Command Characteristic (for receiving commands)
    pCommandCharacteristic = pService->createCharacteristic(
                                         COMMAND_CHAR_UUID,
                                         BLECharacteristic::PROPERTY_WRITE |
                                         BLECharacteristic::PROPERTY_WRITE_NR
                                       );
    pCommandCharacteristic->setCallbacks(new MyCommandCallbacks(this, ledController, device));
    
    // Create Status Characteristic (for sending status updates)
    pStatusCharacteristic = pService->createCharacteristic(
                                        STATUS_CHAR_UUID,
                                        BLECharacteristic::PROPERTY_READ |
                                        BLECharacteristic::PROPERTY_NOTIFY
                                      );
    pStatusCharacteristic->addDescriptor(new BLE2902());
    
    // Start the service
    pService->start();
    
    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0);  // Set value to 0x00 to not advertise this parameter
    BLEDevice::startAdvertising();
    
    Serial.printf("BLE Device '%s' initialized and advertising\n", deviceName.c_str());
    Serial.printf("Service UUID: %s\n", SERVICE_UUID);
}

void BLEManager::sendStatus(const JsonDocument& statusDoc) {
    if (pStatusCharacteristic && deviceConnected) {
        String response;
        serializeJson(statusDoc, response);
        pStatusCharacteristic->setValue(response.c_str());
        pStatusCharacteristic->notify();
    }
}

void BLEManager::sendDeviceInfo(uint8_t deviceId, const String& deviceName, uint8_t batteryLevel) {
    JsonDocument doc;
    doc["type"] = "device_info";
    doc["id"] = deviceId;
    doc["name"] = deviceName;
    doc["battery"] = batteryLevel;
    doc["connected"] = true;
    
    sendStatus(doc);
}

// Server Callbacks Implementation
void MyServerCallbacks::onConnect(BLEServer* pServer) {
    bleManager->deviceConnected = true;
    Serial.println("BLE Client connected");
    
    // Send device info on connection
    if (bleManager->workoutDevice) {
        bleManager->sendDeviceInfo(
            bleManager->workoutDevice->getDeviceId(),
            bleManager->workoutDevice->getDeviceName(),
            bleManager->workoutDevice->getBatteryLevel()
        );
    }
}

void MyServerCallbacks::onDisconnect(BLEServer* pServer) {
    bleManager->deviceConnected = false;
    Serial.println("BLE Client disconnected");
    
    // Restart advertising
    delay(500);
    pServer->startAdvertising();
    Serial.println("Restarted BLE advertising");
}

// Command Callbacks Implementation
void MyCommandCallbacks::onWrite(BLECharacteristic *pCharacteristic) {
    String command = pCharacteristic->getValue().c_str();
    Serial.printf("Received command: %s\n", command.c_str());
    
    // Parse JSON command
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, command);
    
    if (error) {
        Serial.printf("JSON parse error: %s\n", error.c_str());
        return;
    }
    
    String type = doc["type"];
    
    if (type == "led_pattern") {
        handleLEDPattern(doc);
    } else if (type == "workout_command") {
        handleWorkoutCommand(doc);
    } else if (type == "get_status") {
        sendStatus();
    } else if (type == "get_workout_status") {
        sendWorkoutStatus();
    } else if (type == "ping") {
        sendPong();
    }
}

void MyCommandCallbacks::handleLEDPattern(const JsonDocument& doc) {
    if (ledController) {
        ledController->handleLEDPattern(doc);
        
        // Send acknowledgment
        JsonDocument response;
        response["type"] = "led_ack";
        response["pattern"] = doc["pattern"];
        response["device_id"] = workoutDevice ? workoutDevice->getDeviceId() : 0;
        
        bleManager->sendStatus(response);
    }
}

void MyCommandCallbacks::sendStatus() {
    if (workoutDevice) {
        JsonDocument doc;
        doc["type"] = "status";
        doc["id"] = workoutDevice->getDeviceId();
        doc["name"] = workoutDevice->getDeviceName();
        doc["battery"] = workoutDevice->getBatteryLevel();
        doc["connected"] = bleManager->isConnected();
        doc["uptime"] = millis();
        
        bleManager->sendStatus(doc);
    }
}

void MyCommandCallbacks::sendPong() {
    if (workoutDevice) {
        JsonDocument doc;
        doc["type"] = "pong";
        doc["device_id"] = workoutDevice->getDeviceId();
        doc["timestamp"] = millis();
        
        bleManager->sendStatus(doc);
    }
}

void MyCommandCallbacks::handleWorkoutCommand(const JsonDocument& doc) {
    String action = doc["action"];
    
    if (action == "start") {
        workoutDevice->startWorkout();
    } else if (action == "stop") {
        workoutDevice->stopWorkout();
    } else if (action == "set_state") {
        int state = doc["state"];
        workoutDevice->setWorkoutState((WorkoutState)state);
    } else if (action == "sync_pattern") {
        // Coordinated pattern across devices
        int deviceCount = doc["device_count"] | 1;
        unsigned long timestamp = doc["timestamp"] | millis();
        ledController->showSyncPattern(workoutDevice->getDeviceId(), deviceCount, timestamp);
    } else if (action == "timer") {
        int seconds = doc["seconds"] | 0;
        int totalSeconds = doc["total_seconds"] | 60;
        ledController->showTimer(seconds, totalSeconds);
    } else if (action == "intensity") {
        int level = doc["level"] | 5;
        ledController->showIntensity(level);
    }
    
    // Send acknowledgment
    JsonDocument response;
    response["type"] = "workout_ack";
    response["action"] = action;
    response["device_id"] = workoutDevice->getDeviceId();
    
    bleManager->sendStatus(response);
}

void MyCommandCallbacks::sendWorkoutStatus() {
    if (workoutDevice) {
        JsonDocument doc;
        doc["type"] = "workout_status";
        doc["device_id"] = workoutDevice->getDeviceId();
        doc["state"] = workoutDevice->getWorkoutState();
        doc["workout_time"] = workoutDevice->getWorkoutTime();
        doc["calories"] = workoutDevice->getCaloriesBurned();
        doc["battery"] = workoutDevice->getBatteryLevel();
        
        bleManager->sendStatus(doc);
    }
}
