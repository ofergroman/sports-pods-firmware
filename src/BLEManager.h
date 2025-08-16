#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ArduinoJson.h>

// Forward declaration
class LEDController;
class WorkoutDevice;

// BLE UUIDs
#define SERVICE_UUID        "12345678-1234-5678-9abc-123456789abc"
#define COMMAND_CHAR_UUID   "87654321-4321-8765-cba9-876543210abc"
#define STATUS_CHAR_UUID    "11111111-2222-3333-4444-555555555555"

class BLEManager {
public:
    BLEManager();
    void begin(const String& deviceName, WorkoutDevice* device, LEDController* ledController);
    void sendStatus(const JsonDocument& statusDoc);
    void sendDeviceInfo(uint8_t deviceId, const String& deviceName, uint8_t batteryLevel);
    bool isConnected() const { return deviceConnected; }
    
private:
    BLEServer* pServer;
    BLECharacteristic* pCommandCharacteristic;
    BLECharacteristic* pStatusCharacteristic;
    bool deviceConnected;
    
    WorkoutDevice* workoutDevice;
    LEDController* ledController;
    
    friend class MyServerCallbacks;
    friend class MyCommandCallbacks;
};

class MyServerCallbacks: public BLEServerCallbacks {
public:
    MyServerCallbacks(BLEManager* manager) : bleManager(manager) {}
    void onConnect(BLEServer* pServer) override;
    void onDisconnect(BLEServer* pServer) override;
    
private:
    BLEManager* bleManager;
};

class MyCommandCallbacks: public BLECharacteristicCallbacks {
public:
    MyCommandCallbacks(BLEManager* manager, LEDController* ledCtrl, WorkoutDevice* device) 
        : bleManager(manager), ledController(ledCtrl), workoutDevice(device) {}
    void onWrite(BLECharacteristic *pCharacteristic) override;
    
private:
    void handleLEDPattern(const JsonDocument& doc);
    void handleWorkoutCommand(const JsonDocument& doc);
    void sendStatus();
    void sendPong();
    void sendWorkoutStatus();
    
    BLEManager* bleManager;
    LEDController* ledController;
    WorkoutDevice* workoutDevice;
};

#endif // BLE_MANAGER_H
