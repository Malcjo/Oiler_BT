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
  #include <Preferences.h>


  #define EEPROM_SIZE 1

  Preferences pref;
  String prefName = "Oiler_app";
  int pump = 33;  //pin number

  String myInt = "0";
  String runTime = "0";
  String freq = "0";


  String runTimeKey = "runTime";
  String freqKey = "freq";


  String device_name = "Daniel is cool";

  unsigned long lastPumpTime = 0;  // For non-blocking pump operation
  bool isPumpRunning = false;

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
    initalizeStoredData();
  }

  void loop() {
    runIfMessageRecieved();
    runPumpNonBlocking();
  }

  //initalized Bluetooth connection, and run pump once when turning on
  void initalizeBluetooth() {
    Serial.begin(115200);
    SerialBT.begin(device_name);  //Bluetooth device name
    Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
    //runpump();
  }

  void initalizeStoredData() {
    //SerialBT.println("Saved value: " + String(GetInt()));
    SerialBT.println(String(getFreq() / 60000));    // Send frequency in minutes
    SerialBT.println(String(getRunTime() / 1000));  // Send runtime in seconds
  }

  void runPumpNonBlocking() {
    unsigned long currentMillis = millis();
    int freq = getFreq();
    int runtime = getRunTime();

    if (!isPumpRunning && currentMillis - lastPumpTime >= freq) {
      digitalWrite(pump, HIGH);  // turn on pump
      isPumpRunning = true;
      lastPumpTime = currentMillis;  //reset timer
    }
    if (isPumpRunning && currentMillis - lastPumpTime >= runtime) {
      digitalWrite(pump, LOW);  // Turn off the pump after runtime
      isPumpRunning = false;
      lastPumpTime = currentMillis;  // Reset the timer for the next frequency cycle
    }
  }

  //if received any message from bluetooth run the pump
  void runIfMessageRecieved() {
    if (SerialBT.available()) {

      String incomingMessage = SerialBT.readString();
      incomingMessage.trim();

      Serial.print("Recieved: ");
      Serial.println(incomingMessage);


      // Split messages if they are combined with a hyphen, e.g., setfreq10-setruntime20
      if (incomingMessage.indexOf('-') != -1) {
        String freqMessage = incomingMessage.substring(0, incomingMessage.indexOf('-'));
        String runtimeMessage = incomingMessage.substring(incomingMessage.indexOf('-') + 1);

        CheckSetters(freqMessage);
        CheckSetters(runtimeMessage);
      } else {
        // Handle single messages
        CheckSetters(incomingMessage);
        CheckSetters(incomingMessage);
      }
    }
  }

  //checking functions
  void checkGetters(String incomingMessage) {
    if (incomingMessage.equals("getfreq")) {
      String message = "freq " + String(getFreq());
      SerialBT.println(message);

      incomingMessage = "";
    }
    if (incomingMessage.equals("getruntime")) {
      String message = "runtime " + String(getRunTime());
      SerialBT.println(message);
      incomingMessage = "";
    }
  }


  void CheckSetters(String incomingMessage) {
    //eg write set10 and will change the stored into value to 10
    if (incomingMessage.startsWith("setFreq")) {
      int newValue = incomingMessage.substring(7, incomingMessage.length()).toInt();
      setFreq(newValue);
      SerialBT.println("Frequency set to: " + String(newValue) + " minutes");
      SerialBT.print(newValue);
      incomingMessage = "";
    }

    if (incomingMessage.startsWith("setruntime")) {
      int newValue = incomingMessage.substring(10, incomingMessage.length()).toInt();
      setRunTime(newValue);
      SerialBT.println("Runtime set to: " + String(newValue) + " seconds");
      SerialBT.print(newValue);
      incomingMessage = "";
    }
  }


  //Frequency

  int getFreq() {
    pref.begin(prefName.c_str(), true);
    int val = pref.getInt(freqKey.c_str(), 60000);
    pref.end();
    Serial.println("Retrieved frequency (ms): " + String(val));
    return val;
  }
  void setFreq(int val) {
    pref.begin(prefName.c_str(), false);
    int newFreq = val * 60000;  //convert from minites to milliseconds
    pref.putInt(freqKey.c_str(), newFreq);
    Serial.println("Set frequency to (ms): " + String(newFreq));
    pref.end();
  }

  //RunTime

  int getRunTime() {
    pref.begin(prefName.c_str(), true);
    int val = pref.getInt(runTimeKey.c_str(), 1000);
    pref.end();
    Serial.println("Retrieved runtime (ms): " + String(val));
    return val;
  }

  void setRunTime(int val) {
    pref.begin(prefName.c_str(), false);
    int newRunTime = val * 1000;  // convert from seconds to milliseconds
    pref.putInt(runTimeKey.c_str(), newRunTime);
    Serial.println("Set runtime to (ms): " + String(newRunTime));
    pref.end();
  }




  /*
    void SetInt(int val){
      //false opens the app in read-write mode
      //c_str() converts the prefName String into a C-style string
      pref.begin(prefName.c_str(), false);
      pref.putInt(myInt.c_str(), val);
      pref.end();
    }

    int GetInt(){
      // true opens the app in read-only mode
      //c_str() converts the prefName String into a C-style string
      pref.begin(prefName.c_str(), true);
      int val = pref.getInt(myInt.c_str(), 0);//default to 0 if no value stored
      pref.end();
      return val;
    }
    */

  /*
        if(incomingMessage.equals("getint")){
          SerialBT.println(GetInt());
          incomingMessage="";
        }

        if(incomingMessage.equals("pump")){
          runpump();
          incomingMessage="";
        }

        //eg write set10 and will change the stored into value to 10
        if(incomingMessage.startsWith("set")){
          int newValue = incomingMessage.substring(3,incomingMessage.length()).toInt();
          SetInt(newValue);
          SerialBT.println("Stored Value: ");
          SerialBT.print(newValue);
          incomingMessage="";
        }
        //runpump();
        */

  /*
    //run the pump then turn off after a delay
    void runpump() {
      delay(getFreq());
      digitalWrite(pump, HIGH);  // turn the LED on (HIGH is the voltage level)
      delay(getRunTime());       // wait for a second
      digitalWrite(pump, LOW);   // turn the LED off by making the voltage LOW
    }
  */
