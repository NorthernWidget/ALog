/**
@file

# ALog.h

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

Copyright (C) 2011-2017, Andrew D. Wickert
Copyright (C) 2016-2017, Andrew D. Wickert and Chad T. Sandell
Copyright (C) 2016-2017, Regents of the University of Minnesota

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

#ifndef ALog_h
#define ALog_h

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
                    // (This is for the program to configure each logger)
#include <SoftwareSerial.h>
#include <EEPROM.h>

// Sensor-centric libraries
#include <SFE_BMP180.h>
//#include <Adafruit_Sensor.h>
//#include <Adafruit_BMP085_U.h>
//#include <eRCaGuy_NewAnalogRead.h>
//#include <AtlasNW.h>

// Outside of class definitions
void wakeUpNow();
void wakeUpNow_tip();
void _ISR_void();
void _anemometer_count_increment();
void _internalDateTime(uint16_t* date, uint16_t* time); // Callback: SD DT stamp

// The rest of the library
class ALog {

  public:
    // Constructor
    ALog();

    // Initialization
    void initialize(char* _logger_name, char* _datafilename, \
         int _hourInterval, int _minInterval, int _secInterval, \
         bool _ext_int=false, bool _LOG_ALL_SENSORS_ON_BUCKET_TIP=false);
    void setupLogger();

    // Code for sleeping, starting up from sleep, synching SD card
    void sleep();
    void goToSleep_if_needed(); // Same as above, but checks if sleep is being used
    void startLogging();
    void endLogging();
    void startAnalog(); // DEPRECATED
    void endAnalog(); // DEPRECATED
    void sensorPowerOn();
    void sensorPowerOff();

    // Getters and setters
    bool get_use_sleep_mode();
    void set_LEDpin(int8_t _pin);
    void set_SDpowerPin(int8_t _pin);
    void set_RTCpowerPin(int8_t _pin);
    void set_SensorPowerPin(int8_t _pin);
    // Important subset: EEPROM: Serial number and calibrations
    uint16_t get_serial_number();
    float get_3V3_measured_voltage();
    float get_5V_measured_voltage();

    // Sensors - standard procedure (wake up, log, sleep)
    float readPin(uint8_t pin);
    float readPinOversample(uint8_t pin, uint8_t adc_bits);
    float analogReadOversample(uint8_t pin, uint8_t adc_bits=10, \
          uint8_t nsamples=1, bool debug=false);
    float thermistorB(float R0, float B, float Rref, float T0degC, \
          uint8_t thermPin, uint8_t ADC_resolution_nbits=14, \
          bool Rref_on_GND_side=true, bool oversample_debug=false, \
          bool record_results=true);
    // Print order: Distance [cm], standard deviation [cm]
    void ultrasonicMB_analog_1cm(uint8_t nping, uint8_t EX, uint8_t sonicPin, \
         bool writeAll);
    float maxbotixHRXL_WR_Serial(uint8_t Ex, uint8_t npings, bool writeAll, \
          int maxRange, bool RS232=false);
    // Print order: Distance [cm], standard deviation [cm]
    void maxbotixHRXL_WR_analog(uint8_t nping=10, uint8_t sonicPin=A0,
         uint8_t EX=99, bool writeAll=true, uint8_t ADC_resolution_nbits=10);
    void Decagon5TE(uint8_t excitPin, uint8_t dataPin); // Poorly-tested
    void DecagonGS1(uint8_t pin, float Vref, uint8_t ADC_resolution_nbits=14);
    void vdivR(uint8_t pin, float Rref, uint8_t ADC_resolution_nbits=10, \
         bool Rref_on_GND_side=true);
    // FLEX SENSOR READING DOES NOT STABLIIZE: CHANGES CONSISTENTLY WITH TIME.
    //void flex(int flexPin, float Rref, float calib1, float calib2);
    void linearPotentiometer(uint8_t linpotPin, float Rref, float slope, \
         char* _distance_units, float intercept=0, \
         uint8_t ADC_resolution_nbits=14, bool Rref_on_GND_side=true);
    void HTM2500LF_humidity_temperature(uint8_t humidPin, uint8_t thermPin, \
         float Rref_therm, uint8_t ADC_resolution_nbits=14);
    void HM1500LF_humidity_with_external_temperature(uint8_t humidPin, \
         float R0_therm, float B_therm, float Rref_therm, float T0degC_therm, \
         uint8_t thermPin_therm, uint8_t ADC_resolution_nbits=14);
    void Inclinometer_SCA100T_D02_analog_Tcorr(uint8_t xPin, uint8_t yPin, \
         float Vref, float Vsupply, float R0_therm, float B_therm, \
         float Rref_therm, float T0degC_therm, uint8_t thermPin_therm, \
         uint8_t ADC_resolution_nbits=14);
    void Anemometer_reed_switch(uint8_t interrupt_pin_number,
         unsigned long reading_duration_milliseconds, \
         float meters_per_second_per_rotation);
    void Wind_Vane_Inspeed(uint8_t vanePin);
    void Pyranometer(uint8_t analogPin, float raw_mV_per_W_per_m2, \
         float gain, float V_ref, uint8_t ADC_resolution_nbits=14);
    void Barometer_BMP180();
    void _sensor_function_template(uint8_t pin, float param1, \
         float param2, uint8_t ADC_bits=14, bool flag=false);

    // Sensors - triggered
    // Camera on/off function; decision made in end-user Arduino script
    void HackHD(int control_pin, bool want_camera_on);
    float Honeywell_HSC_analog(int pin, float Vsupply, float Vref, \
          float Pmin, float Pmax, int TransferFunction_number, int units, \
          uint8_t ADC_resolution_nbits=14);

  private:
    void pinUnavailable(uint8_t pin);
    char *nameFile(char * _sitecode);

    // Sleep and alarms
    void sleepNow();
    void sleepNow_nap();
    // wakeUpNow defined outside of class; see above
    void alarm(uint8_t hourInterval, uint8_t minInterval, \
         uint8_t secInterval);
    void displayAlarms(); //debug tool delete if desired.
    void checkAlarms();  //debug tool delete if desired.
    void displayTime();   //debug tool delete if desired.
    // LED signals
    void LEDwarn(uint8_t nflash);
    void LEDgood();
    void LEDtimeWrong(uint8_t ncycles);

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

    // Clock and SD card power
    void SDon_RTCon();
    void SDoff_RTCsleep();

    // Clock setting
    void clockSet();
    void GetDateStuff(byte& Year, byte& Month, byte& Day, byte& DoW,
		byte& Hour, byte& Minute, byte& Second);

    // Sensor utility codes
    float _vdivR(uint8_t pin, float Rref, uint8_t adc_bits=10, \
          bool Rref_on_GND_side=true, bool oversample_debug=false);
    // I don't trust the software serial version -- should do some lab testing
    // to make sure that chances of clock speed / interrupt problems,
    // especially with so much time in sleep mode, are near-0.
    //int maxbotix_soft_Serial_parse(int Ex, int Rx, bool RS232=false);
    int maxbotix_Serial_parse(uint8_t Ex);
    float standard_deviation_from_array(float values[], int nvalues, \
          float mean);
    float standard_deviation_from_array(int values[], int nvalues, float mean);

    // Special sensor codes
    // Rain gauge - will wake logger up and cause it to log to a different file
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
