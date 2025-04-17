#include <Arduino.h>
#include <BLEDevice.h>

#define LED_PIN 27
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
 
class MyCallbacks: public BLECharacteristicCallbacks {
   void onWrite(BLECharacteristic *pCharacteristic) {
     std::string value = pCharacteristic->getValue();

     if (value.length() > 0) {
       Serial.println("*********");
       Serial.print("New value: ");
       String utf8Value(value.c_str());
       utf8Value.trim();
       
       if (utf8Value == "on") {
         Serial.println("Turning LED ON");
         digitalWrite(LED_PIN, HIGH);

       }

       else if (utf8Value == "off") {
         Serial.println("Turning LED OFF");
         digitalWrite(LED_PIN, LOW);

       }

       else {
         Serial.println(utf8Value);

       }
      
     }

   }

};
 
void setup() {
  Serial.begin(115200);
  
  pinMode(LED_PIN, OUTPUT);
  
  Serial.println("1- Download and install a BLE scanner app on your phone");
  Serial.println("2- Scan for BLE devices in the app");
  Serial.println("3- Connect to SDSUCS");
  Serial.println("4- Go to CUSTOM CHARACTERISTIC in CUSTOM SERVICE and write 'on' or 'off'");
  Serial.println("5- See the magic =)");

  // Initialize BLE and create a BLE server
  BLEDevice::init("SDSUCS");
  BLEServer *pServer = BLEDevice::createServer();
  
  // Create BLE service
  BLEService *pService = pServer->createService(SERVICE_UUID);
  
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                        CHARACTERISTIC_UUID,
                                        BLECharacteristic::PROPERTY_WRITE
                                      );
  
  // Set callback to handle write events
  pCharacteristic->setCallbacks(new MyCallbacks());
  
  // Start the service
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  
}
 
void loop() {
  delay(2000);

}
