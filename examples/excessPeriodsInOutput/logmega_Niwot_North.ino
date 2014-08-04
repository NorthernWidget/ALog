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

Logger logger;

////////////////////////////
// USER-ENTERED VARIABLES //
////////////////////////////
char* dataLoggerName = "Niwot North";
char* fileName = "NR_NORTH.TXT"; // Name of file for logged data: 8.3 format (e.g, 
                                   // ABCDEFGH.TXT); <= 8 chars before ".txt" is OK
int dataLoggingIntervalMinutes = 1; // Will log when the remainder of "minutes", divided by
                                     // this, equals 0. For regular logging intervals, make  
                                     // sure that this number divides evenly into 60.
bool external_interrupt = false; // e.g., rain gage

// Heave meter calibration
/*
float xHeave[] = {0, 0.5, 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5, 5.5, 6, 6.5, 7, 7.5, 8, 8.5, 9, 9.5, 10};
float R_Heave_2014_001[] = {2.4, 129.5, 343.3, 531, 707, 893, 1077, 1256, 1425, 1591, 1757, 1980, 2038, 3055, 4505, 5910, 7380, 8750, 9960, 10130, 10210};
float R_Heave_2014_002[] = {2.4, 122.7, 307.6, 486.8, 663, 834, 1012, 1186, 1363, 1569, 1760, 2000, 2296, 2972, 4188, 5523, 6950, 8330, 9390, 9580, 9650};
float R_Heave_2014_003[] = {2.3, 120.7, 280.7, 474.8, 631, 810, 968, 1145, 1320, 1496, 1661, 1878, 2179, 3071, 4424, 5910, 7290, 8750, 9930, 10100, 10170};
float R_Heave_2014_004[] = {2.3, 140.2, 322.5, 494.3, 670, 845, 1008, 1207, 1407, 1622, 1816, 2045, 2330, 3147, 4460, 5930, 7190, 8720, 9780, 9950, 10030};
*/

void setup(){
  logger.initialize(dataLoggerName, fileName, dataLoggingIntervalMinutes, external_interrupt);
  logger.setupLogger();
  pinMode(39, OUTPUT);
  digitalWrite(39, LOW);
  /*
  digitalWrite(39, HIGH);
  delay(5000);
  digitalWrite(39, LOW);
  delay(5000);
  digitalWrite(39, HIGH);
  delay(5000);
  digitalWrite(39, LOW);
  delay(5000);
  digitalWrite(39, HIGH);
  */
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

logger.startAnalog();

// Nominal resistance thermistors -- uncalibrated; held as archive for factory characterization
/*
logger.thermistorB(100000, 3950, 25200, 25, A0);
logger.thermistorB(100000, 3950, 25200, 25, A1);
logger.thermistorB(100000, 3950, 25200, 25, A2);
logger.thermistorB(100000, 3950, 25200, 25, A3);
logger.thermistorB(100000, 3950, 25200, 25, A4);
*/

// Temperature with depth
logger.thermistorB(322300, 3950, 25200, 0, A0); // 5 cm
logger.thermistorB(325200, 3950, 25200, 0, A1); // 10 cm
logger.thermistorB(324500, 3950, 25200, 0, A2); // 20 cm
logger.thermistorB(325900, 3950, 25200, 0, A3); // 30 cm
logger.thermistorB(322000, 3950, 25200, 0, A4); // 50 cm

// Flex Sensors
logger.vdivR(A13, 10000); // Upper
logger.vdivR(A14, 10000); // Lower

// Heave Meter
logger.vdivR(A6, 25200); // Null (Standard)
logger.vdivR(A7, 25200); // Variable (Connected to foot that goes to the ground)
// Once I get these internal calibrations done: for now, will just use them as voltage dividers and post-process
// logger.displacementMeasuredByResistance_piecewiseLinear(A6, 25200, xHeave, R_Heave_2014_001); // Null (Standard)
// logger.displacementMeasuredByResistance_piecewiseLinear(A7, 25200, xHeave, R_Heave_2014_002); // Variable (Connected to foot that goes to the ground)

// Air temperature
logger.thermistorB(325100, 3950, 25200, 0, A11);

// Distance to ground (snow measurement)
logger.ultrasonicMB_analog_1cm(10, 99, A9, true);

logger.endAnalog();

//
// INSERT DIGITAL SENSOR READING COMMANDS HERE!
//

// Delay needed between Decagons to avoid crosstalk on lines
logger.DecagonMPS2(44, 3);
delay(2000);
logger.Decagon5TE(10, 3);
delay(2000);
logger.DecagonMPS2(3, 3);
delay(2000);
logger.Decagon5TE(11, 3);

// NOTE: THE BUFFER SIZE IS (CHECK ON THIS!!!!!!!!!!!!!!) 256 BYTES;
// run "logger.bufferWrite" if you think you are approaching this limit.
// Otherwise, the buffer will overflow and I'm not sure what will happen.


// ****** DO NOT EDIT BELOW THIS LINE ****** 

// Wrap up files, turn off SD card, and go back to sleep
logger.endLogging();

// ***************************************** 

}

