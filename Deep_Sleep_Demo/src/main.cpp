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
#include <EEPROM.h>
#include <U8x8lib.h>

//Time definition for Deep Sleep
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5        /* Time ESP32 will go to sleep (in seconds) */

//RTC Storage
RTC_DATA_ATTR int bootCount = 0;

//Default Temperature is in Celsius
//Comment the next line for Temperature in Fahrenheit
#define temperatureCelsius

//BLE server name
#define bleServerName "NuvIoT Temperature"

#define DHTPIN 26

//LED Def
#define LED_PIN 22
uint8_t led_toggle = 0;

//DHT Def
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

//Screen def 
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/15, /* data=*/4, /* reset=*/16);
#define LCD

//Var Def
float temp;
float tempF;
int i;
int delayMS=1000;

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

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

/*
String getUniqueId() {
    uint8_t baseMac[6];
    // Get MAC address for WiFi station
    esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
    char baseMacChr[18] = {0};
    sprintf(baseMacChr, "%02X%02X%02X", baseMac[3], baseMac[4], baseMac[5]);
    return String(baseMacChr);
}
*/
void setup() {

  //Initialize the LCD screen in LCD defined
  #ifdef LCD
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  #endif

  Serial.begin(9600);
  Serial.println(F("DHT22 test!"));
  dht.begin();

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

  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  //Print the wakeup reason for ESP32
  //print_wakeup_reason();

  //First we configure the wake up source We set our ESP32 to wake up every 5 seconds
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
  " Seconds");

  //Show on screen the process it is in
  #ifdef LCD
    u8x8.drawString(0,0,"Waiting ...");
  #endif

  //Main code that will repeat after start up and will repeat 5 times, after completed will sleep
  for (i=0; i<30; ++i){
    if (deviceConnected) {
      #ifdef LCD
        u8x8.drawString(0,0,"Connected!");
      #endif
      if ((millis() - lastTime) > timerDelay) {
        // Read temperature as Celsius (the default)
        temp = dht.readTemperature();
        // Fahrenheit
        tempF = dht.readTemperature(false);

          delay(delayMS);
          led_toggle = led_toggle == 0 ? 1 : 0;
          digitalWrite(22, led_toggle);
          //static char temperatureCTemp[6];
    
        //Notify temperature reading from DHT sensor
        #ifdef temperatureCelsius
          static char temperatureCTemp[6];
          //Set temperature Characteristic value and notify connected client
          dtostrf(temp, 6, 2, temperatureCTemp);
          bmeTemperatureCelsiusCharacteristics.setValue(temperatureCTemp);
          bmeTemperatureCelsiusCharacteristics.notify();
          Serial.print("Temperature Celsius: ");
          Serial.print(temp); 
          Serial.print(" ºC\n");
            #ifdef LCD
              u8x8.drawString(0,0,temperatureCTemp);
            #endif

        #else
          static char temperatureFTemp[6];
          dtostrf(tempF, 6, 2, temperatureFTemp);
          //Set temperature Characteristic value and notify connected client
          bmeTemperatureFahrenheitCharacteristics.setValue(temperatureFTemp);
          bmeTemperatureFahrenheitCharacteristics.notify();
          Serial.print("Temperature Fahrenheit: ");
          Serial.print(tempF);
          Serial.print(" ºF");
           #ifdef LCD
             u8x8.drawString(0,0,temperatureFTemp);
           #endif
        #endif
         //Sleep process begins
        Serial.println("Going to sleep now");
        #ifdef LCD
          u8x8.drawString(0,0,"Sleeping ...");
        #endif
        delay(1000);
        Serial.flush(); 
        esp_deep_sleep_start();
        Serial.println("This will never be printed");
        lastTime = millis();
      }
    }
  }
}

void loop(){ 
}