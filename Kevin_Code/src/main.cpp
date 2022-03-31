#include <Arduino.h>
/*********
  Rui Santos
  Complete instructions at https://RandomNerdTutorials.com/esp32-ble-server-client/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <U8x8lib.h>
#include <EEPROM.h>

#define DHTPIN 4
#define LED_PIN 22

uint8_t led_toggle = 0;

#define DHTTYPE DHT22

// Stable DHT
DHT dht(DHTPIN, DHTTYPE);

// DHT dht(DHTPIN, DHTTYPE);
// Default Temperature is in Celsius
// Comment the next line for Temperature in Fahrenheit
#define temperatureCelsius

// BLE Device Name
#define DEVICE_MODEL "NuvIoT Temperature "

//BLE Server Name - Without MAC
#define bleServerName "NuvIoT Temp Sensor"

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/15, /* data=*/4, /* reset=*/16);

float temp;
float tempF;
float hum;

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 1000;

bool deviceConnected = false;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID "91bad492-b950-4226-aa2b-4ede9fa42f59"

// Temperature Characteristic and Descriptor
#ifdef temperatureCelsius
BLECharacteristic dhtTemperatureCelsiusCharacteristics("cba1d466-344c-4be3-ab3f-189f80dd7518", BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor dhtTemperatureCelsiusDescriptor(BLEUUID((uint16_t)0x2902));
#else
BLECharacteristic dhtTemperatureFahrenheitCharacteristics("f78ebbff-c8b7-4107-93de-889a6a06d408", BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor dhtTemperatureFahrenheitDescriptor(BLEUUID((uint16_t)0x2902));
#endif

/*
// Humidity Characteristic and Descriptor
BLECharacteristic dhtHumidityCharacteristics("ca73b3ba-39f6-4ab3-91ae-186dc9577d99", BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor dhtHumidityDescriptor(BLEUUID((uint16_t)0x2903));
*/

// Setup callbacks onConnect and onDisconnect
class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    deviceConnected = true;
  };
  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;
  }
};


String getUniqueId() {
    uint8_t baseMac[6];
    // Get MAC address for WiFi station
    esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
    char baseMacChr[18] = {0};
    sprintf(baseMacChr, "%02X%02X%02X", baseMac[3], baseMac[4], baseMac[5]);
    return String(baseMacChr);
}

void setup()
{
  // Start serial communication
  Serial.begin(115200);

//Optional LCD Screen
#ifdef LCD
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
#endif

  // Create the BLE Device
  //BLEDevice::init((String(DEVICE_MODEL) + getUniqueId()).c_str());
  BLEDevice::init(bleServerName);

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *dhtService = pServer->createService(SERVICE_UUID);

// Create BLE Characteristics and Create a BLE Descriptor
// Temperature
#ifdef temperatureCelsius
  dhtService->addCharacteristic(&dhtTemperatureCelsiusCharacteristics);
  dhtTemperatureCelsiusDescriptor.setValue("DHT temperature Celsius");
  dhtTemperatureCelsiusCharacteristics.addDescriptor(&dhtTemperatureCelsiusDescriptor);
#else
  dhtService->addCharacteristic(&dhtTemperatureFahrenheitCharacteristics);
  dhtTemperatureFahrenheitDescriptor.setValue("DHT temperature Fahrenheit");
  dhtTemperatureFahrenheitCharacteristics.addDescriptor(&dhtTemperatureFahrenheitDescriptor);
#endif
/*
  // Humidity
  dhtService->addCharacteristic(&dhtHumidityCharacteristics);
  dhtHumidityDescriptor.setValue("DHT humidity");
  dhtHumidityCharacteristics.addDescriptor(new BLE2902());
*/
  pinMode(LED_PIN, OUTPUT);

  // Start the service
  dhtService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pServer->getAdvertising()->start();
  Serial.println("Temperature Sensor Startup");

  dht.begin();
}

int delayMS = 1000;

void loop()
{
  delay(delayMS);

  led_toggle = led_toggle == 0 ? 1 : 0;
  digitalWrite(22, led_toggle);
  static char temperatureCTemp[6];

  temp = dht.readTemperature();

  // Fahrenheit
  tempF = dht.readTemperature(false);
  // Read humidity
  hum = dht.readHumidity();

  if (isnan(hum) || isnan(temp) || isnan(tempF))
  {
    Serial.print("No Temperature Available");
    strcpy(temperatureCTemp,"N/A");
  }
  else
  {

    dtostrf(temp, 6, 2, temperatureCTemp);
    Serial.print("Temperature Celsius: ");
    Serial.print(temp);
    Serial.print(" ÂºC\n");
  }

  if (deviceConnected)
  {
    delayMS = 500;
    Serial.println(F("BT client connected."));

    if ((millis() - lastTime) > timerDelay)
    {
      // Notify temperature reading from BME sensor
      // Set temperature Characteristic value and notify connected client
      dhtTemperatureCelsiusCharacteristics.setValue(temperatureCTemp);
      dhtTemperatureCelsiusCharacteristics.notify();

#ifdef LCD
      u8x8.drawString(0, 1, "Temperature:");
      u8x8.drawString(0, 2, temperatureCTemp);
#endif
    }
    else
    {
      delayMS = 2000;
      Serial.println(F("No BT client connected."));
#ifdef LCD
      u8x8.drawString(0, 1, "Waiting for connection");
#endif
    }
  }
}