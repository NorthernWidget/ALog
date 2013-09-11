/*
alog_no_sensors
This is a framework code for using the Northern Widget "Logger" library,
with appropriate places to add in analog and digital sensors to read.

# LICENSE: GNU GPL v3

alog_no_sensors.ino is part of Logger, an Arduino library written by Andrew D. 
Wickert.
Copyright (C) 2011-2013, Andrew D. Wickert and Northern Widget LLC

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

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

Logger logger;

////////////////////////////
// USER-ENTERED VARIABLES //
////////////////////////////
char* dataLoggerName = "BG 01";
char* fileName = "BG01.txt"; // Name of file for logged data: 8.3 format (e.g, 
                                   // ABCDEFGH.TXT); <= 8 chars before ".txt" is OK
int dataLoggingIntervalMinutes = 10; // Will log when the remainder of "minutes", divided by
                                     // this, equals 0. For regular logging intervals, make  
                                     // sure that this number divides evenly into 60.
bool external_interrupt = true; // e.g., rain gage

void setup(){
  logger.initialize(dataLoggerName, fileName, dataLoggingIntervalMinutes, external_interrupt);
  logger.setupLogger();
}

void loop(){

// ***************************************** 

// Sleep 
logger.sleep(1);

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

//logger.startAnalog();

//logger.endAnalog();

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
