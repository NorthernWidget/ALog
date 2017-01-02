#include "Logger.h"

Logger logger;

////////////////////////////
// USER-ENTERED VARIABLES //
////////////////////////////
char* dataLoggerName = "T01";
char* fileName = "T01.txt"; // Name of file for logged data: 8.3 format (e.g, 
                            // ABCDEFGH.TXT); <= 8 chars before ".txt" is OK

//Setup logging interval here, may setup more than one variable. 
//Minimum interval = 1 sec, maximum interval is 1 second short of 7 days.  
int Log_Interval_Seconds = 0; //Valid range is 0-59 seconds
int Log_Interval_Minutes = 0; //Valid range is 0-59 minutes
int Log_Interval_Hours = 0; //Valid range is 0-23 hours
int Log_Interval_Days = 0; //Valid range is 0-6 days
// external_interrupt is true for a tipping bucket rain gauge
bool external_interrupt = false;

void setup(){
  logger.initialize(dataLoggerName, fileName, Log_Interval_Days, \
      Log_Interval_Hours, Log_Interval_Minutes, Log_Interval_Seconds,
      external_interrupt);
  logger.setupLogger();
}

void loop(){
  // ***************************************** 
  if (logger.get_use_sleep_mode() == true){
    logger.sleep(); // Send logger to sleep, and then wake up
  }
  logger.startLogging(); // Power up all systems, check WDT, reset alarms  
                         // Open data file in write mode
                         
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
