#include "Logger.h"

Logger logger;

////////////////////////////
// USER-ENTERED VARIABLES //
////////////////////////////
char* dataLoggerName = "Thermistor Logger";
char* fileName = "TempLog.txt"; // Name of file for logged data: 8.3 format (e.g, 
                             // ABCDEFGH.TXT); <= 8 chars before ".txt" is OK

//Setup logging interval here, may setup more than one variable. 
//Minimum interval = 1 sec, maximum interval is 23 hours, 59 minutes, 59 seconds
//0 for all means that the logger will not sleep
int Log_Interval_Seconds = 10; //Valid range is 0-59 seconds
int Log_Interval_Minutes = 0; //Valid range is 0-59 minutes
int Log_Interval_Hours = 0; //Valid range is 0-23 hours
bool external_interrupt = false; // e.g., rain gage

void setup(){  //Serial baud rate is set to 38400
  logger.initialize(dataLoggerName, fileName,
    Log_Interval_Hours, Log_Interval_Minutes, Log_Interval_Seconds, 
    external_interrupt);logger.setupLogger();
}

void loop(){
// ***************************************** 
logger.goToSleep_if_needed(); // Send logger to sleep
logger.startLogging();  // Wake up and initialize
// ****** DO NOT EDIT ABOVE THIS LINE ****** 

//////////////////////////////////
// READ SENSORS: EDIT THIS PART //
//////////////////////////////////

// Analog sensors: place all analog sensor commands between
// startAnalog() and endAnalog().
// If you have no analog sensors, you should comment out the 
// startAnalog() and endAnalog() commands

logger.startAnalog();

// CanTherm small bead
//logger.thermistorB(10000, 3950, 10000, 25, 0);
// CanTherm expoxy bead: CWF1B103F3380
logger.thermistorB(10000, 3380, 10000, 25, 0);

logger.endAnalog();

//
// INSERT DIGITAL SENSOR READING COMMANDS HERE!
//

// NOTE: THE BUFFER SIZE IS 512 BYTES;
// run "logger.bufferWrite" if you think you are approaching this limit.
// Otherwise, the buffer will overflow and I'm not sure what will happen.

// ****** DO NOT EDIT BELOW THIS LINE ****** 

// Wrap up files, turn off SD card, and go back to sleep
logger.endLogging();

// ***************************************** 

}
