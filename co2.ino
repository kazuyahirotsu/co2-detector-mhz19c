#include <Arduino.h>
#include "MHZ19.h"                                        
#include <SoftwareSerial.h>                                // Remove if using HardwareSerial
#include "SevSeg.h"
#include "Volume.h"                                        // Include the Volume library


#define RX_PIN 14                                          // Rx pin which the MHZ19 Tx pin is attached to
#define TX_PIN 15                                          // Tx pin which the MHZ19 Rx pin is attached to
#define BZ_PIN 16
#define BAUDRATE 9600                                      // Device to MH-Z19 Serial baudrate (should not be changed)

MHZ19 myMHZ19;                                             // Constructor for library
SevSeg sevseg;                                             // Instantiate a seven segment controller object
SoftwareSerial mySerial(RX_PIN, TX_PIN);                   // (Uno example) create device to MH-Z19 serial

Volume vol;

unsigned long getDataTimer = 0;

void setup()
{   pinMode(14,INPUT);
    pinMode(15,OUTPUT);
    pinMode(16,OUTPUT);
    Serial.begin(9600);                                     // Device to serial monitor feedback

    mySerial.begin(BAUDRATE);                               // (Uno example) device to MH-Z19 serial start   
    myMHZ19.begin(mySerial);                                // *Serial(Stream) refence must be passed to library begin(). 

    myMHZ19.autoCalibration();                              // Turn auto calibration ON (OFF autoCalibration(false))
    byte numDigits = 4;
    byte digitPins[] = {2, 3, 4, 5};
    byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13};
    bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
    byte hardwareConfig = COMMON_CATHODE; // See README.md for options
    bool updateWithDelays = false; // Default 'false' is Recommended
    bool leadingZeros = false; // Use 'true' if you'd like to keep the leading zeros
    bool disableDecPoint = false; // Use 'true' if your decimal point doesn't exist or isn't connected
    
    sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
    updateWithDelays, leadingZeros, disableDecPoint);
    sevseg.setBrightness(90);

    vol.begin();
}

void loop()
{     if (millis() - getDataTimer >= 100000){
      int CO2; 

      /* note: getCO2() default is command "CO2 Unlimited". This returns the correct CO2 reading even 
      if below background CO2 levels or above range (useful to validate sensor). You can use the 
      usual documented command with getCO2(false) */

      CO2 = myMHZ19.getCO2();                             // Request CO2 (as ppm)
      
      Serial.print("CO2 (ppm): ");                      
      Serial.println(CO2);
      Serial.print(",");                                

      int8_t Temp;
      Temp = myMHZ19.getTemperature();                     // Request Temperature (as Celsius)
      Serial.print("Temperature (C): ");                  
      Serial.println(Temp);

      if(CO2>1000){
        tone(BZ_PIN,2000,1000);
      }
      sevseg.setNumber(CO2,0);
      
      getDataTimer = millis();
       }
    sevseg.refreshDisplay();                             // Must run repeatedly
}
