#include "Logger.h"

// Libraries don't seem to automatically come along
#include <Arduino.h> // For 1.0
#include <SdFat.h>
#include <Wire.h>
#include <DS3231.h>
#include <math.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
//#include <NewSoftSerial.h> // in case Decagon loggers are used / Digital IO is needed
#include <SoftwareSerial.h>

Logger logger;

////////////////////////////
// USER-ENTERED VARIABLES //
////////////////////////////
char* dataLoggerName = "T01";
char* fileName = "T01.txt"; // Name of file for logged data: 8.3 format (e.g, 
                             // ABCDEFGH.TXT); <= 8 chars before ".txt" is OK

//Setup logging interval here, may setup more than one variable. 
//Minimum interval = 1 sec, maximum interval is 1 second short of 7 days.  
int Log_Interval_Seconds = 30; //Valid range is 0-59 seconds
int Log_Interval_Minutes = 0; //Valid range is 0-59 minutes
int Log_Interval_Hours = 0; //Valid range is 0-23 hours
int Log_Interval_Days = 0; //Valid range is 0-6 days
bool external_interrupt = false; // e.g., rain gage

void setup(){MCUSR = 0;wdt_disable();logger.initialize(dataLoggerName, fileName, Log_Interval_Days, Log_Interval_Hours, Log_Interval_Minutes, Log_Interval_Seconds, external_interrupt);logger.setupLogger();}

void loop(){

// ***************************************** 
wdt_reset();wdt_disable();logger.sleep();
wdt_enable(WDTO_8S);  //  8 second watchdog timer, log cycle must not exceed 8 seconds.  
                      //  Insert 'wdtreset();' in log cycle code to delay reset by another 8 seconds.
logger.startLogging();  // Wake up and initialize

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
