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
#include <SPI.h>
#include <math.h>
#include <avr/sleep.h>
#include <stdlib.h> // For turning incoming ASCII character strings into int with atol

#include <RTClib.h>
#include <RTC_DS3234.h>

// Outside of class definitions
void wakeUpNow();
void wakeUpNow_tip();

// The rest of the library

class Logger {

  public:
    // Constructor
    Logger();
    
    // Initialization
    void initialize(char* _logger_name, char* _sitecode, int _log_minutes, bool _ext_int=false);
    void setupLogger();
    
    // Code for sleeping, starting up from sleep, synching SD card
    void sleep(int minutes);
    void startLogging();
    void endLogging();
    void startAnalog();
    void endAnalog();
    
    // Sensors - standard procedure (wake up, log, sleep)
    void thermistorB(float R0,float B,float Rref,float T0degC,int thermPin);
    void ultrasonicMB_analog_1cm(int nping,int EX,int sonicPin,bool writeAll); // Print order: Distance [cm], standard deviation [cm]
    float maxbotixHRXL_WR_Serial(int Ex, int Rx, int nping, bool writeAll, int maxRange, bool RS232=false);
    void maxbotixHRXL_WR_analog(int nping=10,int sonicPin=A0,int EX=99,bool writeAll=true); // Print order: Distance [cm], standard deviation [cm]
    void decagon5TE(int excitPin, int dataPin); // Print order: Dielectric permittivity [-unitless-], Electrical Conductivity [dS/m], Temperature [degrees C]
    void vdivR(int pin, float Rref);
    void flex(int flexPin, float Rref, float calib1, float calib2);
    void linearPotentiometer(int linpotPin, float Rref, float slope, float intercept);
    
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
    void DS3234_alarm1_1min();
    
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
    
    // Clock setting
    void clockSet();
    void GetDateStuff(byte& Year, byte& Month, byte& Day, byte& DoW, 
		byte& Hour, byte& Minute, byte& Second);
    
    // Sensor utility codes
    float _vdivR(int pin, float Rref);
    int maxbotix_Serial_parse(int Ex, int Rx, bool RS232=false);
    float standard_deviation_from_array(float values[], int nvalues, float mean);
    float standard_deviation_from_array(int values[], int nvalues, float mean);
        
    // Logging
    void start_logging_to_otherfile(char* filename);
    void end_logging_to_otherfile();
    void endLine();
    // SD card start/end for multiple SPI devices
    // Onus is on the other device to change SPI_MODE and bit order back to its 
    // preference -- as it should be, since I can't anticipate what the other
    // device will need. This *is* done for the DS3234 clock that is integrated
    // into the LogMega.
    void SDstart();
    void SDend();

};

#endif  
    
///////////////
// REFERENCE //
///////////////

// Reference pinout for ATmega644/1284
// INT2 not currently working
//
//                      +---\/---+
//          (D 0) PB0  1|        |40  PA0 (AI 0 / D24)
//          (D 1) PB1  2|        |39  PA1 (AI 1 / D25)
//INT2-BAD  (D 2) PB2  3|        |38  PA2 (AI 2 / D26)
//      PWM (D 3) PB3  4|        |37  PA3 (AI 3 / D27)
//      PWM (D 4) PB4  5|        |36  PA4 (AI 4 / D28)
//     MOSI (D 5) PB5  6|        |35  PA5 (AI 5 / D29)
//PWM  MISO (D 6) PB6  7|        |34  PA6 (AI 6 / D30)
//PWM   SCK (D 7) PB7  8|        |33  PA7 (AI 7 / D31)
//                RST  9|        |32  AREF
//                VCC 10|        |31  GND 
//                GND 11|        |30  AVCC
//              XTAL2 12|        |29  PC7 (D 23)
//              XTAL1 13|        |28  PC6 (D 22)
//     RX0 (D 8)  PD0 14|        |27  PC5 (D 21) TDI
//     TX0 (D 9)  PD1 15|        |26  PC4 (D 20) TDO
//INT0 RX1 (D 10) PD2 16|        |25  PC3 (D 19) TMS
//INT1 TX1 (D 11) PD3 17|        |24  PC2 (D 18) TCK
//     PWM (D 12) PD4 18|        |23  PC1 (D 17) SDA
//     PWM (D 13) PD5 19|        |22  PC0 (D 16) SCL
//     PWM (D 14) PD6 20|        |21  PD7 (D 15) PWM
//                      +--------+

