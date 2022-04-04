/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-two-way-communication-esp32/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

// SENDER
#include <esp_now.h>
#include <WiFi.h>

#include <Wire.h>
#include <DHT.h>

//#define SCREEN_WIDTH 128  // OLED display width, in pixels
//#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define DHTTYPE DHT22
#define DHTPIN 4

#define SLEEP_SECS 5 // 5 secs
#define SEND_TIMEOUT 245  // 245 millis seconds timeout 

DHT dht(DHTPIN, DHTTYPE);


// REPLACE WITH THE MAC Address of your receiver 
uint8_t broadcastAddress[] = {0x7C, 0x9E, 0xBD, 0x5B, 0x33, 0x20};

// Define variables to store DHT readings to be sent
float temperature;
float humidity;
// Define variables to store incoming readings


// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
    float temp;
    float hum;
} struct_message;

volatile boolean callbackCalled;

// Create a struct_message called DHTReadings to hold sensor readings
struct_message dhtReadings;

// Create a struct_message to hold incoming sensor readings
//struct_message incomingReadings;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

// Callback when data is received

 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  Serial.println(F("DHTxx test!"));
  // Init DHT sensor
  dht.begin();

  // Init OLED display

 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  //Deep sleep
  WiFi.disconnect();
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    gotoSleep(); //Deep Sleep
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  //esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {
  //Go to sleep
  if (callbackCalled || (millis() > SEND_TIMEOUT)) {
    gotoSleep();
  }
  // Wait a few seconds between measurements.
  delay(2000);
  getReadings();
 
  // Set values to send
  dhtReadings.temp=temperature;
  dhtReadings.hum=humidity;
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &dhtReadings, sizeof(dhtReadings));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  updateDisplay();
  delay(10000);
  }
  
void getReadings(){
  humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  temperature = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
}

void updateDisplay(){
  // Display Readings on OLED Display

  
  // Display Readings in Serial Monitor
  Serial.print(F("Humidity: "));
  Serial.print(humidity);
  Serial.print(F("%  Temperature: "));
  Serial.print(temperature);
  Serial.print(F("°C "));
  //Serial.print(f);
}

void gotoSleep(){
  Serial.printf("Up for %i ms, going to sleep for %i secs...\n", millis(), SLEEP_SECS); 
  ESP.deepSleep(SLEEP_SECS * 1000000);
}
