#include "Logger.h"

Logger logger;

////////////////////////////
// USER-ENTERED VARIABLES //
////////////////////////////
char* dataLoggerName = "SC 01";
char* fileName = "SC01.txt"; // Name of file for logged data: 8.3 format (e.g, 
                             // ABCDEFGH.TXT); <= 8 chars before ".txt" is OK

//Setup logging interval here, may setup more than one variable. 
//Minimum interval = 1 sec, maximum interval is 1 second short of 7 days.  
int Log_Interval_Seconds = 30; //Valid range is 0-59 seconds
int Log_Interval_Minutes = 0; //Valid range is 0-59 minutes
int Log_Interval_Hours = 0; //Valid range is 0-23 hours
int Log_Interval_Days = 0; //Valid range is 0-6 days
bool external_interrupt = false; // e.g., rain gage

void setup(){logger.initialize(dataLoggerName, fileName, Log_Interval_Days, Log_Interval_Hours, Log_Interval_Minutes, Log_Interval_Seconds, external_interrupt);logger.setupLogger();}

void loop(){
// ***************************************** 
logger.sleep(); // Send logger to sleep
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
logger.AtlasScientific("w", 6, 7, 300, false, false);

// Temperature calibration
if (temperature_float >= 0){
  dtostrf(temperature_float, 6, 3, &AtlasTemp[2]);
}
else{
  // Space for "-" sign
  dtostrf(temperature_float, 6, 2, &AtlasTemp[2]);
}
logger.AtlasScientific(AtlasTemp, 6, 7, 300, false, false);

// Read Atlas sensor
logger.AtlasScientific("R", 6, 7, 300, true, true);

// Sleep mode -- power save
// sensor returns "*SL" when put into sleep
// but the AtlasScientific function in the Logger library will get rid of this
logger.AtlasScientific("Sleep", 6, 7, 300, false, false);

// ****** DO NOT EDIT BELOW THIS LINE ****** 

// Wrap up files, turn off SD card, and go back to sleep
logger.endLogging(); // No internal voltage reading: short I haven't found.

// ***************************************** 

}

