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
//#include <NewSoftSerial.h> // in case Decagon loggers are used / Digital IO is needed
#include <SoftwareSerial.h>

// Also, for float to string function
//#include<stdlib.h>

Logger logger;

////////////////////////////
// USER-ENTERED VARIABLES //
////////////////////////////
char* dataLoggerName = "Kennicott Lake Lower";
char* fileName = "KL_Lower.TXT"; // Name of file for logged data: 8.3 format (e.g, 
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

void setup(){
  logger.initialize(dataLoggerName, fileName, dataLoggingIntervalMinutes, external_interrupt);
  logger.setupLogger();
  pinMode(39, OUTPUT);
  digitalWrite(39, LOW);
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
temperature_float = logger.thermistorB(100000, 3950, 25200, 25, A0);

logger.endAnalog();

//
// INSERT DIGITAL SENSOR READING COMMANDS HERE!
//


// Wake with something silly to get rid of serial stream
logger.AtlasScientific("I", 3, 38400, false, false);
// Get rid of the incoming Serial stream
//while (_tx_buffer->head != _tx_buffer->tail); // get rid of the rest of the buffer after Line 1
while( Serial3.read() != -1 );

// Temperature calibration
Serial3.print("T,");
Serial3.print(temperature_float);
logger.AtlasScientific("0", 3, 38400, true, false);

// Read Atlas sensor
logger.AtlasScientific("R", 3, 38400, true, true);

// Sleep mode -- power save
logger.AtlasScientific("Sleep", 3, 38400, false, false);

// ****** DO NOT EDIT BELOW THIS LINE ****** 

// Wrap up files, turn off SD card, and go back to sleep
logger.endLogging(true); // No internal voltage reading: short I haven't found.

// ***************************************** 

}

