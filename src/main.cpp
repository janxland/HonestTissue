#include <NimBLEDevice.h>
#include <ESP32Servo.h>
Servo servo; // 创建舵机对象
int servoPin = 13;
class NimBLECallbacks : public NimBLECharacteristicCallbacks {
  void onRead(NimBLECharacteristic* pCharacteristic) {
    Serial.println("Read event");
    if (pCharacteristic->getUUID().equals(BLEUUID("5678"))) {
      std::string value = pCharacteristic->getValue();
      if (value.length() > 0) {
        Serial.print("Read value: ");
        Serial.print(value.c_str());
      }
    }
  }

  void onWrite(NimBLECharacteristic* pCharacteristic) {
    Serial.println("Write event");
    if (pCharacteristic->getUUID().equals(BLEUUID("5678"))) {
      // 读取手机发送的值并设置给读写特征
      std::string value = pCharacteristic->getValue();
      if (value.length() > 0) {
        Serial.print("Write value: ");
        Serial.println(value.c_str());
        if (value == "openservo") {
          pCharacteristic->setValue(value);
          pCharacteristic->notify();
            servo.write(0);
            delay(435);
            servo.write(90);
            delay(1000);
        } else {
          pCharacteristic->setValue("Not found");
          pCharacteristic->notify();
        }
      }
    }
  }
};

NimBLECharacteristic* pReadWriteCharacteristic; // 新增读写特征
NimBLEServer* pServer;
NimBLECharacteristic* pCharacteristic;
NimBLEService* pService;
void setup() {
  Serial.begin(115200);
  Serial.println("Starting NimBLE Server");
  NimBLEDevice::init("NimBLE");
  ESP32PWM::allocateTimer(0);
  servo.setPeriodHertz(50);
  servo.attach(servoPin,500,2500);
#ifdef ESP_PLATFORM
  NimBLEDevice::setPower(ESP_PWR_LVL_P9); /** +9db */
#else
  NimBLEDevice::setPower(9); /** +9db */
#endif

  NimBLEDevice::setSecurityAuth(true, true, true);
  NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);

  // 创建BLE服务器
  pServer = NimBLEDevice::createServer();

  // 创建特征值
  pCharacteristic = new NimBLECharacteristic("5678", NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::READ);

  // 设置写回调函数
  pCharacteristic->setCallbacks(new NimBLECallbacks());

  // 添加特征值到服务
  NimBLEService* pService = pServer->createService("ABCD");
  pService->addCharacteristic(pCharacteristic);
  pService->start();

  // 开始广播
  NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID("ABCD");
  pAdvertising->start();
}

void loop() {
}
