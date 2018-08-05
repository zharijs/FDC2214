// This is example code to demonstrate the functionality of FDC2214 library
//
// There is no warranty to the code. There is no support, don't ask for it.
// Use or skip on your own responsibility.
// NOT ALL FEATURES ARE TESTED! 
//
// The code might get revisited at some point or it might not.
// The code does more than I need at the moment.
//
// Feel free to do whatever you want with it. No licensing BS. No limitations.
//
// Created by Harijs Zablockis, Intelitech, March 2018
//
// Supproted chips: FDC2112, FDC2114, FDC2212, FDC2214
// Uses SSD1306Ascii library to display result on 0.96 inch ebay OLED screen
// Transmitts data via serial - use SerialPlot to draw graphs
// 
// FDC2x1x hardware configuration:
// Component value as in default circuit form datasheet. (18uH inductor and 33pF cap)
// 
// SD and ADDR pins tied to GND
// INTB pin not used
// 
// OLED <--> ARDUINO <--> FDC
// SDA <---> A4 <-------> SDA
// SCL <---> A5 <-------> SCL
// 
// !!!!!! Arduinos are mostly 5V. FDC chips are 3.3V, so either use 3.3V version of Arduino, like pro mini, or use level shifter on I2C bus.
// 

// ### OLED.
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#define I2C_ADDRESS 0x3C
#define RST_PIN -1
SSD1306AsciiWire oled;

// ### FDC
#include "FDC2214.h"
FDC2214 capsense(FDC2214_I2C_ADDR_0); // Use FDC2214_I2C_ADDR_1 
unsigned long sensorThreshold[2]; // threshold for proximity detection

// ### Local function prototypes 
int iicScan(); // check for attached devices on I2C bus

// ###
void setup() {
  
  // ### Start I2C 
  Wire.begin();
//  Wire.setClock(400000L);
  
  // ### Start serial
  Serial.begin(115200);
  Serial.println("\nI2C Scanner");
  
  // ### Start OLED display
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.setFont(Adafruit5x7);
  oled.clear();
  oled.println("Intelitech ");
  int devices = iicScan();
  oled.print(":");
  oled.print(devices);
  oled.println("devs");

  // ### Start FDC
  // Start FDC2212 with 2 channels init
//  bool capOk = capsense.begin(0x3, 0x4, 0x5, false); //setup first two channels, autoscan with 2 channels, deglitch at 10MHz, external oscillator 
  // Start FDC2214 with 4 channels init
  bool capOk = capsense.begin(0xF, 0x6, 0x5, false); //setup all four channels, autoscan with 4 channels, deglitch at 10MHz, external oscillator 
  // Start FDC2214 with 4 channels init
//  bool capOk = capsense.begin(0xF, 0x6, 0x5, true); //setup all four channels, autoscan with 4 channels, deglitch at 10MHz, internal oscillator 
  if (capOk) oled.println("Sensor OK");  
  else oled.println("Sensor Fail");  
  sensorThreshold[0] = 14000000+320000;
  sensorThreshold[1] = 320000;

};

// ### Tell aplication how many chanels will be smapled in main loop
#define CHAN_COUNT 4 

// ###
void loop() {
  unsigned long capa[CHAN_COUNT]; // variable to store data from FDC
  for (int i = 0; i < CHAN_COUNT; i++){ // for each channel
    // ### read 28bit data
    capa[i]= capsense.getReading28(i);  
    // ### jump cursor, set font and display channel data on OLED
    oled.setCursor(0,4+(2*i)); 
    oled.set2X();
    oled.print(capa[i]);
    oled.print("  ");
    oled.set1X();
    // ### Transmit data to serial in simple format readable by SerialPlot application.
    Serial.print(capa[i]);  
    if (i < CHAN_COUNT-1) Serial.print(", ");
    else Serial.println("");
    // ### display proximity threshold on OLED 
    oled.setCursor(110,4+(2*i));
    if (capa[i] < sensorThreshold[i]){
      oled.set2X();
      oled.print("*");
      oled.set1X();
    } else {
      oled.set2X();
      oled.print(" ");
      oled.set1X();      
    }
  }
  // No point in sleeping
  //delay(100); 
}

//
// ### scan for I2C devices. This code fragment comes from I2C scan example.
int iicScan (){
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ )  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    //
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
      oled.print("0x");
      oled.print(address,HEX);
      oled.print(" ");
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
  return nDevices;
  }
