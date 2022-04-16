#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include "U8x8lib.h"

//ESP NOW Communication protocol 

//OLED Display on the development board
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

// REPLACE WITH THE MAC Address of your receiver 
uint8_t broadcastAddress[] = {0x78, 0xE3, 0x6D, 0x13, 0x89, 0xF0};

// Define variables to store incoming readings
float incomingTemp;
float incomingHum;
char tempchar[6];

// Variable to store if sending data was successful
String success;

//Temp alert value 
float temp_alert = 55;

//Must match the receiver structure
typedef struct struct_message {
    float temp=0;
    float hum=0;
} struct_message;

// Create a struct_message to hold incoming sensor readings
struct_message incomingReadings;

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  Serial.print("Bytes received: ");
  Serial.println(len);
  incomingTemp = incomingReadings.temp;
  incomingHum = incomingReadings.hum;
}

void updateDisplay(){

  //static char tempchar[6];
  static char humchar[6];
  // Display Readings on OLED Display
  dtostrf(incomingReadings.temp,6,1,tempchar);
  dtostrf(incomingReadings.hum,6,2,humchar);
  // Display Readings in Serial Monitor
  Serial.println("INCOMING READINGS");
  Serial.print("Temperature: ");
  u8x8.drawString(0,0,"Temperature: ");
  Serial.print(incomingReadings.hum);
  u8x8.drawString(0,1,tempchar);
  Serial.println(" ºC");
  u8x8.drawString(7,1, "C");
  Serial.print("Humidity: ");
  u8x8.drawString(0,2,"Humidity:");
  Serial.print(incomingReadings.hum);
  u8x8.drawString(0,3,humchar);  
  Serial.println(" %");
  u8x8.drawString(7,3,"%");
}

void setup() {

  // Init Serial Monitor
  Serial.begin(115200);

  // Init OLED display
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    u8x8.drawString(0,0,"error");
    return;
  }

  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {

  if (incomingReadings.temp>temp_alert){
    u8x8.drawString(0,5,"HIGH TEMP!!!");
  }
  
  u8x8.drawString(0,7,"Hi :)");
  updateDisplay();
  delay(10000);
}

