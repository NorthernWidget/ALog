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

// Also, for float to string function
#include<stdlib.h>

Logger logger;

////////////////////////////
// USER-ENTERED VARIABLES //
////////////////////////////
char* dataLoggerName = "Kennicott Lake Lower";
char* fileName = "TEST.TXT"; // Name of file for logged data: 8.3 format (e.g, 
                                   // ABCDEFGH.TXT); <= 8 chars before ".txt" is OK
// NOTE! LOGGING EVERY MINUTE BECAUSE OF OLD CODE WHERE
// LOG INTERVAL WOULD NEED TO BE DECLARED TWICE. UGH.
// FIXED IN MAIN CPP SO UPDATE NEXT TIME -- REMOVE
// dataLoggingIntervalMinutes from Sleep function var pass.
int dataLoggingIntervalMinutes = 1; // Will log when the remainder of "minutes", divided by
                                     // this, equals 0. For regular logging intervals, make  
                                     // sure that this number divides evenly into 60.
bool external_interrupt = false; // e.g., rain gage

// For temperature calibration
float temperature_float;
char AtlasTemp[9] = "T,";

void setup(){
  logger.initialize(dataLoggerName, fileName, dataLoggingIntervalMinutes, external_interrupt);
  logger.setupLogger();
  // Not using switchable voltage regulators
  // And anyway, should have this in the library in the future
  //pinMode(39, OUTPUT);
  //digitalWrite(39, LOW);
}

void loop(){

// ***************************************** 

// Sleep 
logger.sleep();

// Wake up and initialize
logger.startLogging();

// ****** DO NOT EDIT ABOVE THIS LINE ****** 

//////////////////////////////////
// READ SENSORS: EDIT THIS PART //
//////////////////////////////////

// Analog sensors: place all analog sensor commands between
// startAnalog() and endAnalog().
// If you have no analog sensors, you should comment out the 
// startAnalog() and endAnalog() commands

logger.startAnalog();

// Water temperature at location of conductivity probe
// Update values with those from calibration of thermistor
// (Katy's yellow notebook)
temperature_float = logger.thermistorB(10000, 3950, 25200, 25, A0);

logger.endAnalog();

//
// INSERT DIGITAL SENSOR READING COMMANDS HERE!
//


// just need to send *anything* to wake
// "w" is not a command, but this is key -- logger will send "WA", but
// Atlas command includes "while( Serial3.read() != -1 )"
// to dump the incoming buffer, which keeps the rest of the code in synch.
logger.AtlasScientific("w", 3, 38400, false, false);

// Temperature calibration
if (temperature_float >= 0){
  dtostrf(temperature_float, 6, 3, &AtlasTemp[2]);
}
else{
  // Space for "-" sign
  dtostrf(temperature_float, 6, 2, &AtlasTemp[2]);
}
logger.AtlasScientific(AtlasTemp, 3, 38400, false, false);

// Read Atlas sensor
logger.AtlasScientific("R", 3, 38400, true, true);

// Sleep mode -- power save
// sensor returns "*SL" when put into sleep
// but the AtlasScientific function in the Logger library will get rid of this
logger.AtlasScientific("Sleep", 3, 38400, false, false);

// ****** DO NOT EDIT BELOW THIS LINE ****** 

// Wrap up files, turn off SD card, and go back to sleep
logger.endLogging(true); // No internal voltage reading: short I haven't found.

// ***************************************** 

}

