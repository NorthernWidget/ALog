#include "Logger.h"

// Libraries don't seem to automatically come along
#include <Arduino.h> // For 1.0
#include <SdFat.h>
#include <Wire.h>
#include <DS3231.h>
#include <math.h>
#include <avr/sleep.h>
//#include <NewSoftSerial.h> // in case Decagon loggers are used / Digital IO is needed
#include <SoftwareSerial.h>

// Sensor-centric libraries
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <eRCaGuy_NewAnalogRead.h>

Logger logger;

////////////////////////////
// USER-ENTERED VARIABLES //
////////////////////////////
char* dataLoggerName = "SC 01";
char* fileName = "SC01.txt"; // Name of file for logged data: 8.3 format (e.g, 
                             // ABCDEFGH.TXT); <= 8 chars before ".txt" is OK
int dataLoggingIntervalMinutes = 1; // Will log when the remainder of "minutes", divided by
                                    // this, equals 0. For regular logging intervals, make  
                                    // sure that this number divides evenly into 60.
bool external_interrupt = false; // e.g., rain gage

void setup(){
  logger.initialize(dataLoggerName, fileName, dataLoggingIntervalMinutes, external_interrupt);
  logger.setupLogger();
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

// Arguments in order:
// 1. Resistance R0 at temperature T0
// 2. b-value
// 3. Reference resistor resistance
// 4. Temperature T0 at which thermistor resistance is R0
// 5. Analog pin to which thermistor is attached.
logger.thermistorB(10000, 3950, 10000, 25, 0); // +5  cm
logger.thermistorB(10000, 3950, 10000, 25, 1); // -2  cm
logger.thermistorB(10000, 3950, 10000, 25, 2); // -5  cm
logger.thermistorB(10000, 3950, 10000, 25, 3); // -10 cm
logger.thermistorB(10000, 3950, 10000, 25, 6); // -15 cm
logger.thermistorB(10000, 3950, 10000, 25, 7); // -25 cm

logger.endAnalog();

//
// INSERT DIGITAL SENSOR READING COMMANDS HERE!
//

// NOTE: THE BUFFER SIZE IS (CHECK ON THIS!!!!!!!!!!!!!!) 256 BYTES;
// run "logger.bufferWrite" if you think you are approaching this limit.
// Otherwise, the buffer will overflow and I'm not sure what will happen.


// ****** DO NOT EDIT BELOW THIS LINE ****** 

// Wrap up files, turn off SD card, and go back to sleep
logger.endLogging();

// ***************************************** 

}
