//This example code is in the Public Domain (or CC0 licensed, at your option.)
//By Evandro Copercini - 2018
//
//This example creates a bridge between Serial and Classical Bluetooth (SPP)
//and also demonstrate that SerialBT have the same functionalities of a normal Serial



//flash memory
// read as many times
// most devices are designed for about 100,000 to 1,000,000 write operations
//EEPROM library
//512 addresses to save data
//EEPROM.write(address, value) eg EEPROM.write(0, 5)
//EEPROM.read(address)
//brightness
//on/off


#include "BluetoothSerial.h"
#include <WString.h>
#include <EEPROM.h>



#define EEPROM_SIZE 1




int pump = 33;


//#define USE_PIN // Uncomment this to use PIN during pairing. The pin is specified on the line below
const char *pin = "1234";  // Change this to more secure PIN.

String device_name = "Daniel is cool";

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run make menuconfig to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;


void setup() {
  pinMode(pump, OUTPUT);
  initalizeBluetooth();
}

void runpump(){
  digitalWrite(pump, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(2000);                       // wait for a second
  digitalWrite(pump, LOW);    // turn the LED off by making the voltage LOW
}

void loop() {
  if(SerialBT.available()){
    char incomingChar = SerialBT.read();
    Serial.print("Recieved: ");
    Serial.println(incomingChar);
    runpump();
  }
}

void initalizeBluetooth() {
  Serial.begin(115200);
  SerialBT.begin(device_name);  //Bluetooth device name
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
  runpump();  
}