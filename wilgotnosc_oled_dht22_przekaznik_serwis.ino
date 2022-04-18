/*  Humidity controller based on DHT22 + oled 128x32 + 230V relay
 *   
 *  relay steering on pin 10
 *  DHT22 data on pin7
 *  oled on pins 3 and 4 (clock, data)
 *  power supply for dht pin 9 (and GND)
 *  power supply for oled pin 2 (and GND)
 * 
 */
#include "dhtnew.h"
#include "U8g2lib.h"
#include "SPI.h"
#include "Wire.h"

#define MAX_FAN 1200000 // max time fan can run

unsigned long fan_start; //time when the fan started
DHTNEW dht(7);
U8X8_SSD1306_128X32_UNIVISION_SW_I2C u8x8(3, 4); //choose your constructor, set pins accordingly

void setup() {

  pinMode(10, OUTPUT);//relay control
  digitalWrite(10, LOW);
  pinMode(2, OUTPUT);//power supply oled
  pinMode(9, OUTPUT);//power supply dht
  digitalWrite(2, HIGH);
  digitalWrite(9, HIGH);

  dht.setHumOffset(17.00); //offset to make several sensor giving similar data

  Serial.begin(115200);                    //serial monitor

  u8x8.begin();
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_f);
  u8x8.drawString(0,0,"Wilg:    Temp:");
  u8x8.setFont(u8x8_font_8x13B_1x2_f );

  fan_start = 0;
}

void spin_time_start() {
  if ( fan_start == 0) {
    fan_start = millis();
  } 
}
void spin_time_stop() {
    //if anything is needed on fan stop
}

void loop() {
  // put your main code here, to run repeatedly:
  dht.read();
  float h = max(-50,min(150, dht.getHumidity())); //filtering out crazy results
  float t = max(-10,min(60, dht.getTemperature())); //filtering out crazy results
 
  if (!isnan(h) && !isnan(t)) {
      Serial.print("Humidity (%): ");
      Serial.print((float)h, 2);
      Serial.print("   ");
      Serial.print("Temperature (C): ");
      Serial.print((float)t, 2);
      Serial.print("   ");
      Serial.print("Time fan started: ");
      Serial.println(fan_start);
      
      u8x8.setCursor(1, 2);
      u8x8.print((float)h, 2);
      u8x8.print("  ");
      u8x8.setCursor(9, 2);
      u8x8.print((float)t, 2);
      u8x8.print("  ");
      
      if (h > 72 && (millis() - fan_start) < MAX_FAN) {
        //turn on the fan
        digitalWrite(10, HIGH);
        spin_time_start();
      } else if (h < 67 || (millis() - fan_start) > MAX_FAN  || millis()  < fan_start ) {
        //turn off the fan
        digitalWrite(10, LOW);
        spin_time_stop();
      } else {

      }
  }
  
  delay(2000);

}
