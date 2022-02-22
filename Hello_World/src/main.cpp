#include <Arduino.h>
#include <U8g2lib.h>

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
}

void loop() {
  u8x8.drawString(0,1,"Hello World");
  Serial.println("Hello_World");
  delay(5000);
}