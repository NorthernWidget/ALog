#include "ALog.h"

ALog alog;

// Note: Serial baud rate is set to 38400 bps

////////////////////////////
// USER-ENTERED VARIABLES //
////////////////////////////
char* dataLoggerName = "T01"; // Name of logger; displayed in Serial communications
char* fileName = "T01.txt"; // Name of file for logged data: 8.3 format (e.g,
                            // ABCDEFGH.TXT); <= 8 chars before ".txt" is OK

//Setup logging interval here, may set up more than one variable.
//Minimum interval = 1 sec, maximum interval is 23 hours, 59 minutes, 59 seconds
//0 for all means that the logger will not sleep
int Log_Interval_Seconds = 10; //Valid range is 0-59 seconds
int Log_Interval_Minutes = 0; //Valid range is 0-59 minutes
int Log_Interval_Hours = 0; //Valid range is 0-23 hours
// external_interrupt is true for a tipping bucket rain gauge
bool external_interrupt = false;

void setup(){
  alog.initialize(dataLoggerName, fileName,
    Log_Interval_Hours, Log_Interval_Minutes, Log_Interval_Seconds, 
    external_interrupt);

  // If you are using a standard Arduino board (i.e. not a full ALog data
  // logger) and are not using the Arduino shield, you will have to set the
  // proper pins for the indicator LED (defualts to 9) and the SD card, RTC
  // power, and sensor or other external device power (default to -1).
  // The "-1" default ensures that no pins are affected by commands to switch
  // power to the SD card, RTC power, or sensor power. This is because the
  // typical use case, a standard Arduino Uno, uses the Arduino's built-in
  // 3V3 regulator to power to run the RTC and SD card (as we do with the ALog
  // shield).
  // These can also be set to the same value if they are needed but the same
  // voltage regulator serves multiple purposes.
  // Replace "_pin" with your desired pin number, and uncomment the relevant
  // following line(s).
  // set_LEDpin(_pin);
  // set_SDpowerPin(_pin);
  // set_RTCpowerPin(_pin);
  // set_SensorPowerPin(_pin);

  alog.setupLogger();
}

void loop(){
  // *****************************************************

  alog.goToSleep_if_needed(); // Send logger to sleep
  alog.startLogging(); // Power up all systems, check WDT, reset alarms,
                       // and open data file(s) in write mode

  // ************ DO NOT EDIT ABOVE THIS LINE ************

  ///////////////////////////////////////////////////////
  // READ SENSORS; GATHER/PROCESS DATA: EDIT THIS PART //
  ///////////////////////////////////////////////////////

  alog.sensorPowerOn();

  // Turn on external power (3.3V and 5V in the case of the ALog BottleLogger)
  // for sensors and any other devices.
  // Place commands for all sensors that require this between
  // SensorPowerOn() and SensorPowerOff().
  // If you have no sensors that require power, you should comment out the
  // SensorPowerOn() and SensorPowerOff() commands.

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! //
  // INSERT COMMANDS TO READ SENSORS THAT REQUIRE ALOG-SUPPLIED POWER HERE! //
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! //

  alog.sensorPowerOff();

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! //
  // INSERT COMMANDS TO READ SENSORS THAT DO NOT REQUIRE ALOG-SUPPLIED POWER HERE! //
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! //


  // NOTE: THE BUFFER SIZE IS 512 BYTES;
  // run "alog.bufferWrite()" between your commands in this section
  // if you think you are approaching this limit.
  // Otherwise, the buffer will overflow and I'm not sure what will happen!
  // alog.bufferWrite()

  // ************ DO NOT EDIT BELOW THIS LINE ************

  // Wrap up files, turn off SD card, and go back to sleep
  alog.endLogging();

  // *****************************************************
}
