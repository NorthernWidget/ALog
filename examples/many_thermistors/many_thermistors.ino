#include "ALog.h"

ALog alog;

// Note: Serial baud rate is set to 38400 bps

////////////////////////////
// USER-ENTERED VARIABLES //
////////////////////////////
char* dataLoggerName = "SC 01";
char* fileName = "SC01.txt"; // Name of file for logged data: 8.3 format (e.g, 
                             // ABCDEFGH.TXT); <= 8 chars before ".txt" is OK

//Setup logging interval here, may setup more than one variable. 
//Minimum interval = 1 sec, maximum interval is 23 hours, 59 minutes, 59 seconds  
//0 for all means that the logger will not sleep
int Log_Interval_Seconds = 5; //Valid range is 0-59 seconds
int Log_Interval_Minutes = 0; //Valid range is 0-59 minutes
int Log_Interval_Hours = 0; //Valid range is 0-23 hours
bool external_interrupt = false; // e.g., rain gage

void setup(){
  alog.initialize(dataLoggerName, fileName,
    Log_Interval_Hours, Log_Interval_Minutes, Log_Interval_Seconds, 
    external_interrupt);
  alog.setupLogger();
}

void loop(){
  // ***************************************** 
  alog.goToSleep_if_needed(); // Send logger to sleep
  alog.startLogging();  // Wake up and initialize
  // ****** DO NOT EDIT ABOVE THIS LINE ****** 

  //////////////////////////////////
  // READ SENSORS: EDIT THIS PART //
  //////////////////////////////////

  // Analog sensors: place all analog sensor commands between
  // startAnalog() and endAnalog().
  // If you have no analog sensors, you should comment out the 
  // startAnalog() and endAnalog() commands

  alog.startAnalog();

  // Arguments in order:
  // 1. Resistance R0 at temperature T0
  // 2. b-value
  // 3. Reference resistor resistance
  // 4. Temperature T0 at which thermistor resistance is R0
  // 5. Analog pin to which thermistor is attached.
  alog.thermistorB(10000, 3950, 10000, 25, 0); 
  alog.thermistorB(10000, 3950, 10000, 25, 1); 
  alog.thermistorB(10000, 3950, 10000, 25, 2); 
  alog.thermistorB(10000, 3950, 10000, 25, 3); 
  alog.thermistorB(10000, 3950, 10000, 25, 6); 
  alog.thermistorB(10000, 3950, 10000, 25, 7); 

  alog.endAnalog();

  //
  // INSERT DIGITAL SENSOR READING COMMANDS HERE!
  //

  // NOTE: THE BUFFER SIZE IS (CHECK ON THIS!!!!!!!!!!!!!!) 256 BYTES;
  // run "alog.bufferWrite" if you think you are approaching this limit.
  // Otherwise, the buffer will overflow and I'm not sure what will happen.


  // ****** DO NOT EDIT BELOW THIS LINE ****** 

  // Wrap up files, turn off SD card, and go back to sleep
  alog.endLogging();

  // ***************************************** 
}
