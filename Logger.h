/**
@file

# Logger.h

Data logger library header<br>
Designed for the ALog<br>
Modules should work for any Arduino-based board with minimal modificiation.<br>
Goals:
1. Manage logger utility functions, largely behind-the-scenes
2. Simplify data logger operations to one-line calls

Written by Andy Wickert, 2011-2017, and Chad Sandell, 2017<br>
Started 27 September 2011

Designed to greatly simplify Arduino sketches 
for the ALog and reduce what the end 
user needs to do into relatively simple 
one-line calls.

## LICENSE: GNU GPL v3

Logger.h is part of Logger, an Arduino library written by Andrew D. Wickert
and Chad T. Sandell.

Copyright (C) 2011-2015, Andrew D. Wickert<br>
Copyright (C) 2016-2017, Andrew D. Wickert and Chad T. Sandell

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
*/


/////////////////////////////////
// DEFINE SELF / INCLUDE GUARD //
/////////////////////////////////

#ifndef Logger_h
#define Logger_h

///////////////////////
// INCLUDE LIBRARIES //
///////////////////////

// Arduino 1.0 libraries now default; no if/else
#include <Arduino.h>

// Standard libraries
#include <SdFat.h>
#include <Wire.h>
#include <DS3231.h>
#include <math.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <stdlib.h> // For turning incoming ASCII character
                    // strings into int with atol
#include <EEPROM.h> // Save serial number and reference voltage
                    // Serial number cannot be written here
#include <SoftwareSerial.h>

// Sensor-centric libraries
#include <SFE_BMP180.h>
//#include <Adafruit_Sensor.h>
//#include <Adafruit_BMP085_U.h>
//#include <eRCaGuy_NewAnalogRead.h>
#include <AtlasNW.h>

// Outside of class definitions
void wakeUpNow();
void wakeUpNow_tip();
void _ISR_void();
void _anemometer_count_increment();
void _internalDateTime(uint16_t* date, uint16_t* time); // Callback: SD DT stamp
// The rest of the library

class Logger {

  public:
    // Constructor
    Logger();
    
    // Initialization
    void initialize(char* _logger_name, char* _datafilename, int _dayInterval, \
         int _hourInterval, int _minInterval, int _secInterval, \
         bool _ext_int=false, bool _LOG_ON_BUCKET_TIP=false);
    void setupLogger();
    
    // Code for sleeping, starting up from sleep, synching SD card
    void sleep();
    void goToSleep_if_needed(); // Same as above, but checks if sleep is being used
    void startLogging();
    void endLogging();
    void startAnalog();
    void endAnalog();
    
    // Getters and setters
    bool get_use_sleep_mode();
    
    // Sensors - standard procedure (wake up, log, sleep)
    float readPin(int pin);
    float readPinOversample(int pin, int bits);
    float analogReadOversample(int pin, uint8_t adc_bits=10, int nsamples=1, \
          bool debug=false);
    float thermistorB(float R0, float B, float Rref, float T0degC, \
          int thermPin, uint8_t ADC_resolution_nbits=14, \
          bool Rref_on_GND_side=true, bool oversample_debug=false, bool record_results=true);
    // Print order: Distance [cm], standard deviation [cm]
    void ultrasonicMB_analog_1cm(int nping, int EX, int sonicPin, \
         bool writeAll);
    float maxbotixHRXL_WR_Serial(int Ex, int nping, bool writeAll, \
          int maxRange, bool RS232=false);
    // Print order: Distance [cm], standard deviation [cm]
    void maxbotixHRXL_WR_analog(int nping=10, int sonicPin=A0, int EX=99, \
         bool writeAll=true, \
         uint8_t ADC_resolution_nbits=10);
    void Decagon5TE(int excitPin, int dataPin); // Poorly-tested
    void DecagonGS1(int pin, float Vref, uint8_t ADC_resolution_nbits=14);
    void vdivR(int pin, float Rref, uint8_t ADC_resolution_nbits=10, \
         bool Rref_on_GND_side=true);
    // FLEX SENSOR READING DOES NOT STABLIIZE: CHANGES CONSISTENTLY WITH TIME.
    //void flex(int flexPin, float Rref, float calib1, float calib2);
    void linearPotentiometer(int linpotPin, float Rref, float slope, \
         char* _distance_units, float intercept=0, \
         uint8_t ADC_resolution_nbits=14, bool Rref_on_GND_side=true);
    void HTM2500LF_humidity_temperature(int humidPin, int thermPin, \
         float Rref_therm, uint8_t ADC_resolution_nbits=14);
    void HM1500LF_humidity_with_external_temperature(int humidPin, \
         float R0_therm, float B_therm, float Rref_therm, float T0degC_therm, \
         int thermPin_therm, uint8_t ADC_resolution_nbits=14);
    void Inclinometer_SCA100T_D02_analog_Tcorr(int xPin, int yPin, \
         float Vref, float Vsupply, float R0_therm, float B_therm, \
         float Rref_therm, float T0degC_therm, int thermPin_therm, \
         uint8_t ADC_resolution_nbits=14);
    void Anemometer_reed_switch(int interrupt_pin_number, 
         unsigned long reading_duration_milliseconds, \
         float meters_per_second_per_rotation);
    void Wind_Vane_Inspeed(int vanePin);
    void Pyranometer(int analogPin, float raw_mV_per_W_per_m2, \
         float gain, float V_ref, uint8_t ADC_resolution_nbits=14);
    void Barometer_BMP180();
    void AtlasConductivity();
    void _sensor_function_template(int pin, float param1, \
         float param2, int ADC_bits=14, bool flag=false);
    
    // Sensors - triggered
    // Camera on/off function; decision made in end-user Arduino script
    void HackHD(int control_pin, bool want_camera_on);
    float Honeywell_HSC_analog(int pin, float Vsupply, float Vref, \
          float Pmin, float Pmax, int TransferFunction_number, int units, \
          uint8_t ADC_resolution_nbits=14);

  private:
    void pinUnavailable(int pin);
    char *nameFile(char * _sitecode);

    // Sleep and alarms
    void sleepNow();
    void sleepNow_nap();
    // wakeUpNow defined outside of class; see above
    void alarm(int dayInterval ,int hourInterval, int minInterval, \
         int secInterval);
    void displayAlarms(); //debug tool delete if desired.
    void checkAlarms();  //debug tool delete if desired. 
    void displayTime();   //debug tool delete if desired. 
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
    void establishContact_Tx();
    bool establishContact_Rx();
    
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
    float _vdivR(int pin, float Rref, uint8_t adc_bits=10, \
          bool Rref_on_GND_side=true, bool oversample_debug=false);
    // I don't trust the software serial version
    //int maxbotix_soft_Serial_parse(int Ex, int Rx, bool RS232=false);
    int maxbotix_Serial_parse(int Ex);
    float standard_deviation_from_array(float values[], int nvalues, \
          float mean);
    float standard_deviation_from_array(int values[], int nvalues, float mean);

    // Special sensor codes
    // Rain gage - will wake logger up and cause it to log to a different file
    // The default interrupt is Pin 3, INT1 on ATmega328 (ALog BottleLogger).
    // This runs of bool _ext_int = true in loger.initialize()
    void TippingBucketRainGage();
    
    // Logging
    void start_logging_to_datafile();
    void start_logging_to_otherfile(char* filename);
    void end_logging_to_otherfile();
    void start_logging_to_headerfile();
    void end_logging_to_headerfile();
    void endLine();

};

// End include guard
#endif  

