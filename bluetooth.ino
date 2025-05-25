#include <ArduinoBLE.h>

BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // LED Service

// Generic BLE characteristic that can hold float (4 bytes)
BLECharacteristic ledCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLEWrite | BLERead, sizeof(float));

const int LEDpin = 3;
int LEDIntensity;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  pinMode(LEDpin, OUTPUT);

  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("LED");
  BLE.setAdvertisedService(ledService);

  ledService.addCharacteristic(ledCharacteristic);
  BLE.addService(ledService);

  BLE.advertise();
  Serial.println("BLE Buzzer Peripheral");
}

void loop() {
  BLEDevice central = BLE.central();
  LEDIntensity = 0;
  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    while (central.connected()) {
      if (ledCharacteristic.written()) {
        float received;
        float receivedrange;
        int LEDInter;
        ledCharacteristic.readValue((byte*)&received, sizeof(received));
        receivedrange = constrain(received, 0 , 50);
        LEDInter =  map(receivedrange, 0, 50, 255, 0);
        LEDIntensity = constrain(LEDInter, 0, 255);  // convert to ms
        Serial.println(LEDIntensity);
        analogWrite(LEDpin, LEDIntensity);
      }
    }
    analogWrite(LEDpin, 0);
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}
