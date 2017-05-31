#include "ALog.h"

ALog alog;

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
  delay(50);
  pinMode(3, INPUT); // Include in library later
  digitalWrite(3, HIGH);
  alog.initialize(dataLoggerName, fileName, Log_Interval_Days, \
      Log_Interval_Hours, Log_Interval_Minutes, Log_Interval_Seconds,
      external_interrupt);alog.setupLogger();
  pinMode(3, INPUT); // Include in library later
  digitalWrite(3, HIGH);
  delay(100);
  interrupts();

alog.startAnalog();
}

void loop(){
// ***************************************** 
if (alog.get_use_sleep_mode() == true){
  alog.sleep(); // Send logger to sleep, and then wake up
}
alog.startLogging(); // Power up all systems, check WDT, reset alarms  
                       // Open data file in write mode
// ****** DO NOT EDIT ABOVE THIS LINE ****** 

//////////////////////////////////
// READ SENSORS: EDIT THIS PART //
//////////////////////////////////

// Analog sensors: place all analog sensor commands between
// startAnalog() and endAnalog().
// If you have no analog sensors, you should comment out the 
// startAnalog() and endAnalog() commands

//alog.startAnalog();

//I2C Barometer:
alog.Barometer_BMP180();

//Thermistor (0):
alog.thermistorB(10000, 3950, 10000, 25, A0); //Displayed with RH sensor function

//Relative Humidity (1):
//alog.HM1500LF_humidity_with_external_temperature(A1,10000,3950,10000,25,A0,12);

//Ultrasonic rangefinder (2):
//alog.ultrasonicMB_analog_1cm(5, 99, A1, 0);
alog.maxbotixHRXL_WR_analog(5,A1,99,0);

//Wind Speed (3):
//alog.Anemometer_reed_switch(3, 2000, 1.1176);

//Wind Direction
//alog.Wind_Vane_Inspeed(

//Force sensor (2):
alog.readPin(A2);

//Water pressure (7):
//***Honeywell_HSC_analog(int pin, float Vsupply, float vRef, float Pmin, float Pmax, int TransferFunction_number, int units)***

//TransferFunction: 1 = 10% to 90% of Vsupply (A in second to last digit of part number)
//TransferFunction: 2 = 5% to 95% of Vsupply (A in second to last digit of part number)
//TransferFunction: 3 = 5% to 85% of Vsupply (A in second to last digit of part number)
//TransferFunction: 4 = 4% to 94% of Vsupply (A in second to last digit of part number)

//Units: 0 = mbar
//Units: 1 = bar
//Units: 2 = Pa
//Units: 3 = KPa
//Units: 4 = MPa
//Units: 5 = inH2O
//Units: 6 = PSI

alog.Honeywell_HSC_analog(A7,5.0,3.3,0.0,30.0,1,6);

//alog.endAnalog();

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
