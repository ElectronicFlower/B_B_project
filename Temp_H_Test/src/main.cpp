#include <Arduino.h>
#include <U8g2lib.h>

#include "DHT.h"

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

#define DHTPIN 26

#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

char temp[10];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println(F("DHT22 test!"));
  dht.begin();

  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  if (isnan(h) || isnan(t) || isnan(f)){
    Serial.println(F("Failed to read from the DHT sensor!"));
    return;
  }

  sprintf(temp, "%f", t);
  float hif = dht.computeHeatIndex(f,h);

  float hic = dht.computeHeatIndex(t,h,false);

  Serial.print(F("\n Hummidity: "));
  Serial.print(h);
  Serial.print(F("% Temperature: "));
  Serial.print(t);
  Serial.print(F("C "));
  Serial.print(f);
  Serial.print(F("F Heat Index: "));
  Serial.print(hic);
  Serial.print(F("C "));
  Serial.print(hif);
  Serial.print(F("F "));

  u8x8.drawString(0,0,"Temperature:");
  u8x8.drawString(0,1,temp);
  //u8x8.drawString(1,2," C");
}