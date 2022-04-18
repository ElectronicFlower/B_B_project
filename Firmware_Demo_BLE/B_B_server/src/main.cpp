#include <Arduino.h>

/*********
 Wireless Board to Board connection -- Server side 

 Reference code: https://randomnerdtutorials.com/esp32-ble-server-client/

 Code created to work with the DHT22 sensor and the ESP32 board 
*********/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include "U8x8lib.h"

// Screen Setup 
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

//Default Temperature is in Celsius
//Comment the next line for Temperature in Fahrenheit
#define temperatureCelsius

//BLE server name
#define bleServerName "DHT22_ESP32"

#define DHTPIN 26

#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

float temp;
float tempF;
float hum;
char tempchar[10];

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 4000;

bool deviceConnected = false;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID "91bad492-b950-4226-aa2b-4ede9fa42f59"

// Temperature Characteristic and Descriptor
#ifdef temperatureCelsius
  BLECharacteristic bmeTemperatureCelsiusCharacteristics("cba1d466-344c-4be3-ab3f-189f80dd7518", BLECharacteristic::PROPERTY_NOTIFY);
  BLEDescriptor bmeTemperatureCelsiusDescriptor(BLEUUID((uint16_t)0x2902));
#else
  BLECharacteristic bmeTemperatureFahrenheitCharacteristics("f78ebbff-c8b7-4107-93de-889a6a06d408", BLECharacteristic::PROPERTY_NOTIFY);
  BLEDescriptor bmeTemperatureFahrenheitDescriptor(BLEUUID((uint16_t)0x2902));
#endif

//Setup callbacks onConnect and onDisconnect
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    //u8x8.drawString(0,3,"Disconnected");
  }
};

void setup() {

  Serial.begin(9600);
  Serial.println(F("DHT22 test!"));
  dht.begin();

  //Setup the screen
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);

  // Create the BLE Device
  BLEDevice::init(bleServerName);

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *bmeService = pServer->createService(SERVICE_UUID);

  // Create BLE Characteristics and Create a BLE Descriptor
  // Temperature
  #ifdef temperatureCelsius
    bmeService->addCharacteristic(&bmeTemperatureCelsiusCharacteristics);
    bmeTemperatureCelsiusDescriptor.setValue("BME temperature Celsius");
    bmeTemperatureCelsiusCharacteristics.addDescriptor(&bmeTemperatureCelsiusDescriptor);
  #else
    bmeService->addCharacteristic(&bmeTemperatureFahrenheitCharacteristics);
    bmeTemperatureFahrenheitDescriptor.setValue("BME temperature Fahrenheit");
    bmeTemperatureFahrenheitCharacteristics.addDescriptor(&bmeTemperatureFahrenheitDescriptor);
  #endif  

  // Start the service
  bmeService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
  u8x8.drawString(0,0,"Wait.. Server");
}

void loop() {
  if (deviceConnected) {
    if ((millis() - lastTime) > timerDelay) {
      // Read temperature as Celsius (the default)
      temp = dht.readTemperature();
      // Fahrenheit
      tempF = dht.readTemperature(false);
  
      //Notify temperature reading from BME sensor
      #ifdef temperatureCelsius
        static char temperatureCTemp[6];
        //Set temperature Characteristic value and notify connected client
        dtostrf(temp, 6, 2, temperatureCTemp);
        bmeTemperatureCelsiusCharacteristics.setValue(temperatureCTemp);
        bmeTemperatureCelsiusCharacteristics.notify();
        Serial.print("Temperature Celsius: ");
        Serial.print(temp); 
        Serial.print(" ºC\n");
        u8x8.drawString(0,0,"Temperature:");
        u8x8.drawString(1,1,temperatureCTemp);
        u8x8.drawString(7,1," C");
      #else
        static char temperatureFTemp[6];
        dtostrf(tempF, 6, 2, temperatureFTemp);
        //Set temperature Characteristic value and notify connected client
        bmeTemperatureFahrenheitCharacteristics.setValue(temperatureFTemp);
        bmeTemperatureFahrenheitCharacteristics.notify();
        Serial.print("Temperature Fahrenheit: ");
        Serial.print(tempF);
        Serial.print(" ºF");
        u8x8.drawString(0,0,"Temperature:");
        u8x8.drawString(1,1,temperatureCTemp);
        u8x8.drawString(7,1," F");
      #endif

      lastTime = millis();
    }
  }
}