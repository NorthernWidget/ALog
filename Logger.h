/**********************************************

Logger.h

Data logger library header

Andy Wickert, started 27 September 2011

Designed to greatly simplify Arduino sketches 
for my data logger and reduce what the end 
user needs to do into relatively simple 
one-line calls.


# LICENSE: GNU GPL v3

Logger.h is part of Logger, an Arduino library written by Andrew D. Wickert.
Copyright (C) 2011-2013, Andrew D. Wickert

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

**********************************************/

/////////////////////////////////
// DEFINE SELF / INCLUDE GUARD //
/////////////////////////////////

#ifndef Logger_h
#define Logger_h

///////////////////////
// INCLUDE LIBRARIES //
///////////////////////

// Due to the release of Arduino 1.0, I need to check the version and 
// import libraries based on that.
//#if ARDUINO < 100
//#include <WProgram.h>
//#include <NewSoftSerial.h> // in case Decagon loggers are used / Digital IO is needed -- becomes Software Serial in Arduino 1.0 and greater
//#else
#include <Arduino.h>
#include <SoftwareSerial.h>
//#endif

#include <SdFat.h>
#include <Wire.h>
#include <DS3231.h>
#include <math.h>
#include <avr/sleep.h>
#include <stdlib.h> // For turning incoming ASCII character strings into int with atol

// Outside of class definitions
void wakeUpNow();
void wakeUpNow_tip();

// The rest of the library

class Logger {

  public:
    // Constructor
    Logger();
    
    // Initialization
    void initialize(char* _logger_name, char* _sitecode, int _log_minutes, bool _ext_int=false, bool _LOG_ON_BUCKET_TIP=false);
    void setupLogger();
    
    // Code for sleeping, starting up from sleep, synching SD card
    void sleep(int minutes); // soon to be deprecated: requires log_minutes to be declared twice, caused ADW some confusion
    void sleep();
    void startLogging();
    void endLogging();
    void startAnalog();
    void endAnalog();
    
    // Sensors - standard procedure (wake up, log, sleep)
    float thermistorB(float R0,float B,float Rref,float T0degC,int thermPin,bool Rref_on_GND_side=true);
    void ultrasonicMB_analog_1cm(int nping,int EX,int sonicPin,bool writeAll); // Print order: Distance [cm], standard deviation [cm]
    float maxbotixHRXL_WR_Serial(int Ex, int Rx, int nping, bool writeAll, int maxRange, bool RS232=false);
    void maxbotixHRXL_WR_analog(int nping=10,int sonicPin=A0,int EX=99,bool writeAll=true); // Print order: Distance [cm], standard deviation [cm]
    //void decagon5TE(int excitPin, int dataPin); // Print order: Dielectric permittivity [-unitless-], Electrical Conductivity [dS/m], Temperature [degrees C]
    void DecagonGS1(int pin, float Vref);
    void vdivR(int pin, float Rref, bool Rref_on_GND_side=true);
    void flex(int flexPin, float Rref, float calib1, float calib2);
    void linearPotentiometer(int linpotPin, float Rref, float slope, float intercept);
    void AtlasScientific(char* command, int softSerRX=6, int softSerTX=7, uint32_t baudRate=38400, bool printReturn=true, bool saveReturn=true);
    void HTM2500LF_humidity_temperature(int humidPin, int thermPin, float Rref);
    void HM1500LF_humidity_with_external_temperature(int humidPin, float Vref, float R0, float B, float Rref, float T0degC, int thermPin);
    void Inclinometer_SCA100T_D02_analog_Tcorr(int xPin, int yPin, float R0, float B, float Rref, float T0degC, int thermPin);
    
    // Sensors - special
    // Rain gage - will wake logger up and cause it to log to a different file
    /////////// PLACEHOLDER
    // Camera on/off function; decision made in end-user Arduino script
    void HackHD(int control_pin, bool want_camera_on);
    // The default interrupt is Pin 3, INT1 on ATmega328
    // because this is free for a rain gage on the ALog BottleLogger
    void TippingBucketRainGage();

  private:
    void pinUnavailable(int pin);
    char *nameFile(char * _sitecode);

    // Sleep and alarms
    void sleepNow();
    // wakeUpNow defined outside of class; see above
    void alarm2reset();
    void alarm2_1min();
    
    // LED signals
    void LEDwarn(int nflash);
    void LEDgood();
    void LEDtimeWrong(int ncycles);
    
    // Time
    void unixDatestamp();
    
    // Logger-computer communications
    void name();
    void print_time();
    void set_time_main();
    void announce_start();
    void startup_sequence();
    
    // Clock power
    void RTCon();
    void RTCsleep();
    
    // SD card power
    void SDpowerOn();
    void SDpowerOff();
    
    // Clock setting
    void clockSet();
    void GetDateStuff(byte& Year, byte& Month, byte& Day, byte& DoW, 
		byte& Hour, byte& Minute, byte& Second);
    
    // Sensor utility codes
    float _vdivR(int pin, float Rref, bool Rref_on_GND_side=true);
    int maxbotix_soft_Serial_parse(int Ex, int Rx, bool RS232=false);
    int maxbotix_Serial_parse(int Ex);
    float standard_deviation_from_array(float values[], int nvalues, float mean);
    float standard_deviation_from_array(int values[], int nvalues, float mean);
        
    // Logging
    void start_logging_to_otherfile(char* filename);
    void end_logging_to_otherfile();
    void endLine();

};

#endif  

