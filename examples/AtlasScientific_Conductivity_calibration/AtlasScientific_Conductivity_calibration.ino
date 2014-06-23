#include "Logger.h"

// Libraries don't seem to automatically come along
#include <Arduino.h> // For 1.0
#include <SdFat.h>
#include <Wire.h>
#include <SPI.h>
#include <RTClib.h>
#include <RTC_DS3234.h>
#include <math.h>
#include <avr/sleep.h>
#include <SoftwareSerial.h>

Logger logger;

char SerialNumberASCII[1];
int SerialNumber = -1; // Not selected at start
bool LoopFlag = true; // True if staying in loop
int length; // String length
char inchar; // Holds input character from user serial input
String LowCalString = ""; // To read preferred calibration value
                         // Strings are memory hogs, but loggers can manage it
String HighCalString = "";

void setup()
{
  Serial.begin(57600);
}

void loop()
{
  Serial.println("Ready to calibrate your Atlas Scientific conductivity sensor.");
  Serial.println("Type the UART (Serial) port number (0, 1, 2, 3)");
  Serial.println("to which the sensor is atached.");
  while(SerialNumber < 0){
    if (Serial.available()){
      SerialNumberASCII[0] = (Serial.read());
      SerialNumber = atoi(SerialNumberASCII); // Convert to integer
    }
  }
  // Enable Serial
  Serial.println();
  Serial.println();
  Serial.print("OK, will communicate with Serial port ");
  Serial.println(SerialNumber);
  //logger.StartHardwareSerial(SerialNumber, 38400);
  Serial.println();

  // Debugging LEDs
  Serial.println("Enabling debugging LEDs (if not already on).");
  logger.AtlasScientific("L,1", SerialNumber, 38400, true, false);
  Serial.println("Done.");
  Serial.println();
  
  // Dry calibration
  Serial.println("Ready to start dry calibration.");
  Serial.println("Ensure that sensor is fully dry.");
  Serial.println("Press ENTER when ready to calibrate.");
  while (Serial.available()){
    Serial.read(); // get rid of any "enter", etc. pressed before this.
  }
  while (LoopFlag){
    if (Serial.available()){
      inchar = Serial.read();
      //Serial.println(inchar);
      if (inchar == '\r' || inchar == '\n'){
        LoopFlag = false;
        Serial.println(LoopFlag);
      }
      while (Serial.available() ){
        Serial.read(); // clear buffer
      }
      Serial.println("Performing dry calibration. Please wait.");
      logger.StartHardwareSerial(SerialNumber, 38400);
      logger.AtlasScientific("Cal,dry", SerialNumber, 38400, true, false);
      logger.EndHardwareSerial(SerialNumber, 38400);
    }
  }
  LoopFlag = true;
  Serial.println("Dry calibration complete.");
  Serial.println();
  delay(500);
  
  // Low conductivity solution calibration
  Serial.println("Place the sensor in the calibration solution with the lower");
  Serial.println("electrical conductivity.");
  delay(2000);
  Serial.println("When the sensor is ready to be calibrated, enter the calibration");
  Serial.println("fluid conductivity in uS, and then press ENTER");
  while (Serial.available()){
    Serial.read(); // get rid of any "enter", etc. pressed before this.
  }
  while (LoopFlag){
    if (Serial.available()){
      inchar = (char)Serial.read(); // (char) not necessary; 13 == '\r'
      if (inchar != '\r' && inchar != '\n'){
        LowCalString += inchar;
      }
      else{
        Serial.read();
        LoopFlag = false;
      }
    }
  }
  Serial.println();
  Serial.print("Performing wet calibration at ");
  Serial.print(LowCalString);
  Serial.print(". ");
  Serial.println("Please wait.");
  length = LowCalString.length();
  char LowCalCharArray[length];
  LowCalString.toCharArray(LowCalCharArray, length);
  logger.StartHardwareSerial(SerialNumber, 38400);
  logger.PrintHardwareSerial(SerialNumber, "Cal,low,");
  logger.EndHardwareSerial(SerialNumber, 38400);
  logger.AtlasScientific(LowCalCharArray, SerialNumber, 38400, true, false);
  LoopFlag = true;
  Serial.println("Low salinity calibration complete.");
  Serial.println("Please remove the sensor from the low salinity solution");
  Serial.println("and dry it thoroughly such that there is no cross-contamination");
  Serial.println("with the high conductivity solution.");
  delay(5000);

  // High conductivity solution calibration
  Serial.println("Place the sensor in the calibration solution with the higher");
  Serial.println("electrical conductivity.");
  delay(2000);
  Serial.println("When the sensor is ready to be calibrated, enter the calibration");
  Serial.println("fluid conductivity in uS, and then press ENTER");
  while (Serial.available()){
    Serial.read(); // get rid of any "enter", etc. pressed before this.
  }
  while (LoopFlag){
    if (Serial.available()){
      inchar = (char)Serial.read();
      if (inchar != '\r' && inchar != '\n'){
        HighCalString += inchar;
      }
      else{
        Serial.read();
        LoopFlag = false;
      }
    }
  }
  Serial.println();
  Serial.print("Performing wet calibration at ");
  Serial.print(HighCalString);
  Serial.print(". ");
  Serial.println("Please wait.");
  length = HighCalString.length();
  char HighCalCharArray[length];
  HighCalString.toCharArray(HighCalCharArray, length);
  logger.StartHardwareSerial(SerialNumber, 38400);
  logger.PrintHardwareSerial(SerialNumber, "Cal,high,");
  logger.EndHardwareSerial(SerialNumber, 38400);
  logger.AtlasScientific(HighCalCharArray, SerialNumber, 38400, true, false);
  LoopFlag = true;
  Serial.println("High salinity calibration complete.");
  delay(500);
  Serial.println("All calibration complete.");
  Serial.println("Please remove the sensor from the high salinity solution,");
  Serial.println("cap the solutions, and mark this sensor as calibrated.");

  Serial.println();
  Serial.println();

  //logger.EndHardwareSerial(SerialNumber, 38400);

  delay(1000);

}
