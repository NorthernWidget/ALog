/**
@file ALog.cpp

Data logger library
Designed for the ALog
Modules should work for any Arduino-based board with minimal modificiation
Goals: (1) Manage logger utility functions, largely behind-the-scenes
       (2) Simplify data logger operations to one-line calls

Written by Andy Wickert, 2011-2017, and Chad Sandell, 2016-2017
Started 27 September 2011

Designed to greatly simplify Arduino sketches
for the ALog and reduce what the end
user needs to do into relatively simple
one-line calls.

# LICENSE: GNU GPL v3

ALog.cpp is part of ALog, an Arduino library written by Andrew D. Wickert
and Chad T. Sandell
Copyright (C) 2011-2017, Andrew D. Wickert
Copyright (C) 2016-2017, Chad T. Sandell
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

////////////////////////////////////////////////////
// INCLUDE HEADER FILE AND ITS INCLUDED LIBRARIES //
////////////////////////////////////////////////////

#include <ALog.h>


// Breaking things up at first, but will try to just put all of the initialize /
// setup stuff in the constructor eventually (or maybe just lump "initialize"
// and "setup")

/////////////////////////////////////////////////////
// STATIC MEMBERS OF CLASS, SO ACCESSIBLE ANYWHERE //
/////////////////////////////////////////////////////

// MAYBE PUT UNDERSCORES BEFORE ALL OF THESE VARS, IF I THINK THERE IS RISK OF
// RE-DEFINING THEM IN SKETCH

// DECLARE PINS
// Full declaration here with some "if's" so I can do "const [int/byte/...]"
// Compiler optimizes all ints below, so no need to convert to byte

/////////////////
// ASSIGN PINS //
/////////////////


// True for all: UNO (ATMega168/328)
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega8__) || defined(__AVR_ATmega88__)
  // SD card: CSpin and protected pins
  const int8_t SCKpin = 13;
  const int8_t MISOpin = 12;
  const int8_t MOSIpin = 11;
  const int8_t CSpin = 10;
  // Protected I2C pins
  const int8_t SDApin = A4;
  const int8_t SCLpin = A5;
  // Sleep mode pins
  const int8_t wakePin = 2; // interrupt pin used for waking up via the alarm
  const int8_t manualWakePin = 5; // Wakes the logger with a manual button - overrides the "wait for right minute" commands
  #if defined(ARDUINO_AVR_ALOG_BOTTLELOGGER_V2)
    // SD and RTC power switch
    // 7 for both? Maybe this is better than redefining variables.
    // GitHub issue is open on this; should guide future decisions
    int8_t SDpowerPin = 7; // Turns on voltage source to SD card
    int8_t RTCpowerPin = 7; // Activates voltage regulator to power the RTC (otherwise is on backup power from VCC or batt)
    // LED power switch
    int8_t LEDpin = 8; // LED to tell user if logger is working properly
    // External device power switch
    int8_t SensorPowerPin = 4; // Activates voltage regulator to give power to sensors
  #elif defined(ARDUINO_AVR_ALOG_BOTTLELOGGER_PRE_V200)
    // SD and RTC power switch
    int8_t SDpowerPin = 8; // Turns on voltage source to SD card
    int8_t RTCpowerPin = 6; // Activates voltage regulator to power the RTC (otherwise is on backup power from VCC or batt)
    // LED power switch
    int8_t LEDpin = 9; // LED to tell user if logger is working properly
    // External device power switch
    int8_t SensorPowerPin = 4; // Activates voltage regulator to give power to sensors
  #else
    // These can be variable; default values here are for the ALog shield
    // No pin is appropriate for the ALog shield for SDpowerPin and RTCpowerPin,
    // as these are controlled by the internal 3V3 regulator
    int8_t LEDpin = 8;
    int8_t SDpowerPin = -1;
    int8_t RTCpowerPin = -1;
    // External device power switch
    int8_t SensorPowerPin = -1; // Activates voltage regulator to give power to sensors
  #endif
#endif
// True for all: MEGA (ATMega1280/2560)
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  // SD card: CSpin and protected pins
  const int8_t SCKpin = 52;
  const int8_t MISOpin = 50;
  const int8_t MOSIpin = 51;
  const int8_t CSpin = 53;
  // Protected I2C pins
  const int8_t SDApin = 20;
  const int8_t SCLpin = 21;
  // Sleep mode pins: same as for 328
  const int8_t wakePin = 2; // interrupt pin used for waking up via the alarm
#endif

/*
#elif(_model == big_log)
  // SD card: CSpin and protected pins
  const int SCKpin = 7;
  const int MISOpin = 6;
  const int MOSIpin = 5;
  const int CSpin = 4;
  // Protected I2C pins
  const int SDApin = 23;
  const int SCLpin = 22;
  // Digital pins
  const int SensorPowerPin = 21; // Activates voltage regulator to give power to sensors
  const int SDpowerPin = 22; // Turns on voltage source to SD card
  const int LEDpin = 23; // LED to tell user if logger is working properly
  const int wakePin = 10; // interrupt pin used for waking up via the alarm
*/

// Clock mode
const bool h12 = false;

/////////////////////////////////////////////////
// GLOBAL VARIABLES DEFINED IN INITIALIZE STEP //
/////////////////////////////////////////////////

// Logging interval - wake when minutes == this
//int log_minutes; //**chad delete when new alarm works
uint8_t hourInterval;
uint8_t minInterval;
uint8_t secInterval;
// Alarm counters
uint8_t _hours;
uint8_t _minutes;
uint8_t _seconds;

// Use the sleep mode?
bool _use_sleep_mode = true; // Defaults to true

bool CAMERA_IS_ON = false; // for a video camera

// IS_LOGGING tells the logger if it is awake and actively logging
// Prevents being put back to sleep by an event (e.g., rain gage bucket tip)
// if it is in the middle of logging, so it will return to logging instead.
bool IS_LOGGING = false;

// Filename and logger name
// Filename is set up as 8.3 filename:
//char filename[12];
char* datafilename;
char* logger_name;

// For interrupt from sensor
bool extInt; // This will default to Pin 3 (INT(errupt) 1 on ALog BottleLogger)
bool NEW_RAIN_BUCKET_TIP = false; // flag
bool LOG_ALL_SENSORS_ON_BUCKET_TIP; // Defaults to False, true if you should
                                    // all sensors every time an event (e.g.,
                                    // rain gage bucket tip) happens

// Flag for first logging attempt after booting up
// Starts out as True, and is set False at endLogging()
// When True, writes header information to a separate file.
// This tells the user if the logger reboots, and save the full history
// of headers
bool first_log_after_booting_up = true;

// Rotation count for anemometer
// Unlikely to go over 255...???
uint8_t rotation_count = 0;

// Generic output from an ASCII char array
//char ASCII_out[100];

// Flag for whether or not there is a sensor in the UART port.
// If there is, disable the time-setting protocols; these will be triggered
// by detection of the sensor.
// bool sensor_on_UART,


/////////////////////////
// INSTANTIATE CLASSES //
/////////////////////////

// Both for same clock, but need to create instances of both
// classes in library (due to my glomming of two libs together)
RTClib RTC;
DS3231 Clock;

// SD CLASSES
SdFat sd;
SdFile datafile;
SdFile otherfile; // for rain gage, camera timing, and anything else that
                  // doesn't follow the standard logging cycle / regular timing
SdFile headerfile; // Holds header data; re-printed on each reboot for a full
                   // history of the logger's activity and to see if it has
                   // rebooted while in the field (e.g., due to the
                   // watchdog timer rescuing it from hanging)

DateTime now;

/////////////////////////////////
/////////////////////////////////
//// ALOG LIBRARY COMPONENTS ////
/////////////////////////////////
/////////////////////////////////

/**
 * @brief
 * ALog library for the Arduino-based data loggers
 *
 * @details
 * ALog data logger library: methods to:
 * * Initialize the data logger
 * * Sleep and wake
 * * Interact with the real-time clock (RTC)
 * * Write data to the SD card
 * * Manage power
 * * Interact with a range of sensors
 *
 * All help documentation here assumes you have created an instance of the
 * "ALog" class.
 * ```
 * ALog alog;
 * ```
 *
*/



// Constructor
ALog::ALog(){}

void ALog::initialize(char* _logger_name, char* _datafilename, \
           int _hourInterval, int _minInterval, int _secInterval, \
           bool _ext_int, bool _LOG_ALL_SENSORS_ON_BUCKET_TIP){
           // bool _sensor_on_UART,
  /**
   * @brief
   * Pass all variables needed to initialize logging.
   *
   * @param _logger_name: Name associated with this data logger; often helps to
   * relate it to the project or site
   *
   * @param _datafilename: Name of main data file saved to SD card; often helps
   * to relate it to the project or site; used to be limited to 8.3 file
   * naming convention, but now strictly is not. (I still use
   * 8.3 names for safety's sake!)
   *
   * @param _hourInterval: How many hours to wait before logging again; can range
   * from 0-24.
   *
   * @param _minInterval: How many minutes to wait before logging again; can range
   * from 0-59.
   *
   * @param _secInterval: How many seconds to wait before logging again; can range
   * from 0-59.
   *
   * If all time-setting functions are 0, then the logger will not sleep, and
   * instead will log continuously. This sets the flag "_use_sleep_mode"
   * to be false.
   *
   * @param _ext_int: External interrupt, set to be a tipping-bucket rain gauge,
   * that triggers event-based logging of a timestamp
   *
   * @param _LOG_ALL_SENSORS_ON_BUCKET_TIP: Flag that tells the logger to read
   * every sensor when the bucket tips (if _ext_int is true) and write their
   * outputs to "datafile" (i.e. the main data file whose name you specify
   * with \b _filename; this is in addition to writing the timestamp of the
   * rain gauge bucket tip.
   *
   * @details
   * ALog Data logger model does not need to be set:
   * it is automatically determined from the MCU type and is used to
   * modify pinout-dependent functions.
   *
   * Example:
   * ```
   * \\ Log every five minutes
   * alog.initialize('TestLogger01', 'lab_bench_test.alog', 0, 0, 5, 0);
   * ```
   *
  */

  wdt_disable();
  wdt_enable(WDTO_8S);    // Enable the watchdog timer interupt.

  ///////////////////
  // SLEEP COUNTER //
  ///////////////////

  // Assign the global variables (not intended to change) to the input values
  logger_name = _logger_name;
  datafilename = _datafilename;
  hourInterval = _hourInterval;
  minInterval = _minInterval;
  secInterval = _secInterval;

  // If all logging intervals are 0, then this means that we don't go to sleep:
  // continuous logging!
  // (If all were set to 0, logger would try to log continuously anyway; this
  // just makes things easier by avoiding the sleep function
  if ((hourInterval || minInterval || secInterval) == false){
    _use_sleep_mode = false;
    IS_LOGGING = true; // is always logging, in this case!
  }
  // Assign the global and changable variables to input values
  LOG_ALL_SENSORS_ON_BUCKET_TIP = _LOG_ALL_SENSORS_ON_BUCKET_TIP;

  //////////////////////////////////////////
  // EXTERNAL INTERRUPT (E.G., RAIN GAGE) //
  //////////////////////////////////////////

  // Specific for the bottle logger!
  extInt = _ext_int;
  if (extInt){
    pinMode(extInt, INPUT);
    digitalWrite(3, HIGH); // enable internal 20K pull-up
  }

  ////////////
  // SERIAL //
  ////////////

  Serial.begin(38400);

  /////////////////////////////
  // Logger models and setup //
  /////////////////////////////

  Serial.print("Logger model = ");
  #if defined(ARDUINO_AVR_ALOG_BOTTLELOGGER_PRE_V200)
    Serial.println("ALog BottleLogger pre-v2.0.0");
  #elif defined(ARDUINO_AVR_ALOG_BOTTLELOGGER_V2)
    Serial.println("ALog BottleLogger v2.X.Y");
  #elif defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) \
          || defined(__AVR_ATmega8__) || defined(__AVR_ATmega88__)
    Serial.print("ATMega168/328 Arduino ");
    Serial.println("e.g., Uno with or without ALog shield)");
  #elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    Serial.println("ATMega1280/2560 Arduino Mega");
  #else
    Serial.println(F("Error: Arduino model not recognized by the ALog library."));
    Serial.println(F("Please add support for it and/or contact the developers."));
    Serial.println(F("info@northernwidget.com"));
    LEDwarn(100); // 100 quick flashes of the LED
    sleepNow(); // Do nothing until reset
  #endif

  // From weather station code For power savings
  // http://jeelabs.net/projects/11/wiki/Weather_station_code
  #ifndef cbi
  #define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
  #endif
  #ifndef sbi
  #define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
  #endif

  //////////////////////
  // LOGGER FILE NAME //
  //////////////////////

  delay(10);

  Serial.print(F("Filename: "));
  Serial.println(datafilename);

  Serial.println(F("Logger done initializing."));
  delay(10);

}

void ALog::setupLogger(){
  /**
   * @brief
   * Readies the ALog to begin measurements
   *
   * @details
   * Sets all pins, alarms, clock, SD card, etc: everything needed for the
   * ALog to run properly.
   */

  wdt_reset();

  Serial.println(F("Beginning logger setup."));

  // We use a 3.3V regulator that we can switch on and off (to conserve power)
  // to power the instruments. Therefore, we set the analog reference to
  // "EXTERNAL". Do NOT set it to the internal 1.1V reference voltage or to
  // "DEFAULT" (VCC), UNLESS you are  absolutely sure that you need to and the
  // 3.3V regulator connected to the AREF pin is off. Otherwise, you will short
  // 1.1V (or VCC) against 3.3V and likely damage the MCU / fry the ADC(?)
  analogReference(EXTERNAL);

  //////////////
  // SET PINS //
  //////////////

  pinMode(wakePin,INPUT); // Interrupt to wake up
  digitalWrite(wakePin,HIGH); // enable internal 20K pull-up
  // Set the rest of the pins
  pinMode(CSpin,OUTPUT);
  pinMode(SensorPowerPin,OUTPUT);
  pinMode(LEDpin,OUTPUT);
  pinMode(SDpowerPin,OUTPUT);
  pinMode(RTCpowerPin,OUTPUT);
  // Manual wake pin
  pinMode(manualWakePin,INPUT); // LOG NOW button
  digitalWrite(manualWakePin,HIGH); // enable internal 20K pull-up
  // Have Sensor Power set HIGH, because if any I2C sensors are attached
  // and unpowered, they will drag down the signal from the RTC, and the
  // logger will not properly initialize
  digitalWrite(SensorPowerPin,HIGH);

  ////////////
  // SERIAL //
  ////////////

  Serial.begin(38400);

  announce_start(); // Announce start

  ///////////////////
  // WIRE: I2C RTC //
  ///////////////////

  Wire.begin();
  Wire.setTimeout(100);

  SDon_RTCon();

  /////////////////
  // CHECK CLOCK //
  /////////////////

  // Includes check whether you are talking to Python terminal
  startup_sequence();

  ////////////////////////////////////////////////////////////
  // SET FIRST ALARM TO OCCUR ON THE NEXT INTEGER-DIVISIBLE //
  //  NUMBER OF WHATEVER THE SMALLEST NEXT QUANTITY IS      //
  ////////////////////////////////////////////////////////////
  Clock.checkIfAlarm(1); //Clear alarm flags
  Clock.checkIfAlarm(2); //Clear alarm flags

  //uint32_t seconds_now;
  //uint32_t seconds_at_alarm;
  uint32_t SECONDS_FROM_1970_TO_2000 = 946684800;
  uint32_t unixtime_now;
  uint32_t log_interval_seconds;
  uint32_t unixtime_at_next_log;

  // First, what time is it now?
  now = RTC.now();

  // Second, what is the next time on which we fall on an integer of the
  // logging interval?
  // Always act as if logging started at Midnight on Jan 1, 1970, to ensure
  // that even terrible, irregular logging intervals produce times that
  // line up with one anohter
  // Note: no leap seconds!
  unixtime_now = now.unixtime();
  log_interval_seconds = hourInterval*3600 + minInterval*60 + secInterval;
  unixtime_at_next_log = unixtime_now + log_interval_seconds - \
                         (unixtime_now % log_interval_seconds);
  // Ensure we have enough time to not pass the logging event accidentally
  // 5 seconds is way more than needed, but better safe than sorry...
  if (unixtime_at_next_log - unixtime_now < 5){
    unixtime_at_next_log += log_interval_seconds;
  }
  // Create a new object
  DateTime t_nextLog = unixtime_at_next_log;
  _hours = t_nextLog.hour();
  _minutes = t_nextLog.minute();
  _seconds = t_nextLog.second();

  alarm(_hours, _minutes, _seconds);  //Set first alarm.

  displayAlarms();  // Verify Alarms and display time

  delay(10);

  ///////////////////
  // SD CARD SETUP //
  ///////////////////

  // Initialize SdFat or print a detailed error message and halt
  // Use half speed like the native library.
  // change to SPI_FULL_SPEED for more performance.

  name();

  delay(5);

  // Callback to set date and time in SD card file metadata
  // Following: https://forum.arduino.cc/index.php?topic=348562.0
  // See: https://github.com/NorthernWidget/Logger/issues/6
  SdFile::dateTimeCallback(_internalDateTime);

  delay(5);
  Serial.print(F("Initializing SD card..."));
  if (!sd.begin(CSpin, SPI_HALF_SPEED)){
    Serial.println(F("Card failed, or not present"));
    LEDwarn(20); // 20 quick flashes of the LED
    sd.initErrorHalt();
  }

  Serial.println(F("card initialized."));
  Serial.println();
  LEDgood(); // LED flashes peppy happy pattern, indicating that all is well

  start_logging_to_datafile();
  start_logging_to_headerfile();

  name();
  Serial.println(F("Logger initialization complete! Ciao bellos."));

  delay(20);

  if (_use_sleep_mode){
    SDoff_RTCsleep();
  }
  wdt_reset();
}

/////////////////////////////////////////////
// GETTERS AND SETTERS: ADD MORE AS NEEDED //
/////////////////////////////////////////////

bool ALog::get_use_sleep_mode(){
  /**
   * @brief Does the logger enter a low-power sleep mode? T/F.
   *
   * @details
   * * True if the logger is going to sleep between
   *   pases through the data-reading loop.
   * * False if the logger is looping over its logging step (inside
   *   void loop() in the *.ino code) continuously without sleeping
   */
  return _use_sleep_mode;
}

void ALog::set_LEDpin(int8_t _pin){
  /**
   * @brief Set which pin to use for the main indicator LED.
   *
   * @details
   * Run this, if needed, before setupLogger()
   */
  LEDpin = _pin;
}

void ALog::set_SDpowerPin(int8_t _pin){
  /**
   * @brief Set which pin activates the 3V3 regulator to power the SD card.
   *
   * @details
   * * Set to -1 if not being used
   * * Set to the same as RTCpowerPin if these are connected
   *   (standard for the ALog BottleLogger)
   * Run this, if needed, before setupLogger()
   */
   SDpowerPin = _pin;
}

void ALog::set_RTCpowerPin(int8_t _pin){
  /**
   * @brief Set which pin activates the 3V3 regulator to power the RTC
   *        (real-time clock).
   *
   * @details
   * * Set to -1 if not being used
   * * Set to the same as SDpowerPin if these are connected
   *   (standard for the ALog BottleLogger)
   * Run this, if needed, before setupLogger()
   */
   RTCpowerPin = _pin;
}

void ALog::set_SensorPowerPin(int8_t _pin){
  /**
   * @brief Set which pin activates the 3V3 regulator to power sensors and
   * any other external 3V3 devices that receive power from the ALog's
   * 3V3 regulator.
   *
   * @details
   * * Set to -1 if not being used
   * * Otherwise, set to the number of the pin controlling the 3V3 regulator
   *   that goes to the sensors and other peripherals.
   * Run this, if needed, before setupLogger()
   */
   SensorPowerPin = _pin;
}


/////////////////////////////////////////////////////////////////
// EEPROM: SERIAL NUMBERS, MEASURED VOLTAGE REGULATOR VOLTAGES //
/////////////////////////////////////////////////////////////////

uint16_t ALog::get_serial_number(){
  /**
   * @brief Retrieve the ALog's serial number from EEPROM .
   *
   * @details
   * It is stored in bytes 0 and 1 of the EEPROM.
   */
   uint16_t serialNumber;
   EEPROM.get(0, serialNumber);
   return serialNumber;
}

float ALog::get_3V3_measured_voltage(){
  /**
   * @brief Retrieve the ALog's 3.3V regulator's actual measured voltage
   * under load, stored in the EEPROM.
   *
   * @details
   * It is stored in bytes 2, 3, 4, and 5 of the EEPROM.
   */
   uint16_t voltage;
   EEPROM.get(2, voltage);
   return voltage;
}

float ALog::get_5V_measured_voltage(){
  /**
   * @brief Retrieve the ALog's 5V charge pump's actual measured voltage
   * under load, stored in the EEPROM.
   *
   * @details
   * It is stored in bytes 6, 7, 8, and 9 of the EEPROM.
   */
   uint16_t voltage;
   EEPROM.get(6, voltage);
   return voltage;
}

/////////////////////////////////////////////////////
// PRIVATE FUNCTIONS: UTILITIES FOR LOGGER LIBRARY //
/////////////////////////////////////////////////////

void ALog::pinUnavailable(uint8_t pin){
  int _errorFlag = 0;

  char* _pinNameList_crit[9] = {"CSpin", "SensorPowerPin", "SDpowerPin", \
                                "RTCpowerPin", "LEDpin", "wakePin"};
  int _pinList_crit[9] = {CSpin, SensorPowerPin, SDpowerPin, RTCpowerPin, \
                          LEDpin, wakePin};

  char* _pinNameList[9] = {"MISOpin", "MOSIpin", "SCKpin", "SDApin", \
                           "SCLpin"};
  int _pinList[9] = {MISOpin, MOSIpin, SCKpin, SDApin, SCLpin};

  for (int i=0; i<9; i++){
    if (pin == _pinList[i]){
      _errorFlag++;
      Serial.print(F("Error: trying to alter the state of Pin "));
      Serial.println(_pinList[i]);
      Serial.print(F("This pin is assigned in a system-critical role as: "));
      Serial.println(_pinNameList[i]);
      // Note: numbers >13 in standard Arduino are analog pins
    }
  }

  bool SPI_or_I2C_flag = false;
  for (int i=0; i<9; i++){
    if (pin == _pinList_crit[i]){
      SPI_or_I2C_flag = true;
      break;
    }
  }
  if (SPI_or_I2C_flag){
    Serial.println(F("You are using the SPI or I2C bus; take care that this does not clash"));
    Serial.println(F("with the SD card interface (SPI) or the clock interface (I2C)."));
  }

  for (int i=0; i<9; i++){
    if (pin == _pinList_crit[i]){
      _errorFlag++;
      Serial.print(F("Error: trying to alter the state of Pin "));
      Serial.println(_pinList_crit[i]);
      Serial.print(F("This pin is assigned in a system-critical role as: "));
      Serial.println(_pinNameList_crit[i]);
      // Note: numbers >13 in standard Arduino are analog pins
    }
  }

  if (_errorFlag){
    Serial.println(F("Error encountered."));
    Serial.println(F("Stalling program: cannot reassign critical pins to sensors, etc."));
    LEDwarn(50); // 50 quick flashes of the LED
    // Do nothing until reset
    while(1){}
  }
}

void ALog::sleepNow()         // here we put the arduino to sleep
{
  IS_LOGGING = false;           // Definitely not logging anymore

    /* Now is the time to set the sleep mode. In the Atmega8 datasheet
     * http://www.atmel.com/dyn/resources/prod_documents/doc2486.pdf on page 35
     * there is a list of sleep modes which explains which clocks and
     * wake up sources are available in which sleep mode.
     *
     * In the avr/sleep.h file, the call names of these sleep modes are to be found:
     *
     * The 5 different modes are:
     *     SLEEP_MODE_IDLE         -the least power savings
     *     SLEEP_MODE_ADC
     *     SLEEP_MODE_PWR_SAVE
     *     SLEEP_MODE_STANDBY
     *     SLEEP_MODE_PWR_DOWN     -the most power savings
     *
     * For now, we want as much power savings as possible, so we
     * choose the according
     * sleep mode: SLEEP_MODE_PWR_DOWN
     *
     */
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here

//    setPrescaler(6); // Clock prescaler of 64, slows down to conserve power
    cbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter OFF

    sleep_enable();          // enables the sleep bit in the mcucr register
                             // so sleep is possible. just a safety pin

    /* Now it is time to enable an interrupt. We do it here so an
     * accidentally pushed interrupt button doesn't interrupt
     * our running program. if you want to be able to run
     * interrupt code besides the sleep function, place it in
     * setup() for example.
     *
     * In the function call attachInterrupt(A, B, C)
     * A   can be either 0 or 1 for interrupts on pin 2 or 3.
     *
     * B   Name of a function you want to execute at interrupt for A.
     *
     * C   Trigger mode of the interrupt pin. can be:
     *             LOW        a low level triggers
     *             CHANGE     a change in level triggers
     *             RISING     a rising edge of a level triggers
     *             FALLING    a falling edge of a level triggers
     *
     * In all but the IDLE sleep modes only LOW can be used.
     */

    // START HERE!
    if (hourInterval && minInterval && secInterval == -1 && extInt == false){
      Serial.println(F("All inputs to wake from sleep disabled! Reprogram, please!"));
    }
      //Serial.print(F("interrupt"));  delay(10);
    if (hourInterval || minInterval || secInterval != -1){
      attachInterrupt(digitalPinToInterrupt(wakePin), wakeUpNow, LOW); // wakeUpNow when wakePin goes LOW
      //Serial.println(F(" attached")); delay(10);
    }

    if (extInt){
      attachInterrupt(digitalPinToInterrupt(3), wakeUpNow_tip, LOW);
    }

    sleep_mode();            // here the device is actually put to sleep!!

                            // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP.

    sleep_disable();         // first thing after waking from sleep:
                             // disable sleep...

    // detachInterrupt(1); // crude, but keeps interrupts from clashing. Need to improve this to allow both measurements types!
    // 06-11-2015: The above line commented to allow the rain gage to be read
    // at the same time as other readings
    // Maybe move this to specific post-wakeup code?
    detachInterrupt(digitalPinToInterrupt(wakePin)); // disables interrupt so the
                                        // wakeUpNow code will not be executed
                                        // during normal running time.
}

// Must be defined outside of Logger class
void wakeUpNow()        // here the interrupt is handled after wakeup
{
  // execute code here after wake-up before returning to the loop() function
  // timers and code using timers (serial.print and more...) will not work here.
  // we don't really need to execute any special functions here, since we
  // just want the thing to wake up
  IS_LOGGING = true;                   // Currently logging
                                       //    this will allow the logging
                                       //    to happen even if the logger is
                                       //    already awake to deal with a
                                       //    rain gauge bucket tip
 }

void wakeUpNow_tip()        // here the interrupt is handled after wakeup
{
  // execute code here after wake-up before returning to the loop() function
  // timers and code using timers (serial.print and more...) will not work here.
  // we don't really need to execute any special functions here, since we
  // just want the thing to wake up
  //sleep_disable();         // first thing after waking from sleep:
                             // disable sleep...
  NEW_RAIN_BUCKET_TIP = true;
  // If the logger is already logging, run
  // !!!!!!!!!! WHAT WAS SUPPOSED TO GO INSIDE HERE?
  if (IS_LOGGING){

  }
}

void ALog::alarm(uint8_t _hours, uint8_t _minutes, uint8_t _seconds){

  /* Alarm bit info:
   * A1Dy true makes the alarm go on A1Day = Day of Week,
   * A1Dy false makes the alarm go on A1Day = Date of month.
   *
   * byte AlarmBits sets the behavior of the alarms:
   *	Dy	A1M4	A1M3	A1M2	A1M1	Rate
   *	X	1		1		1		1		Once per second
   *	X	1		1		1		0		Alarm when seconds match
   *	X	1		1		0		0		Alarm when min, sec match
   *	X	1		0		0		0		Alarm when hour, min, sec match
   *	0	0		0		0		0		Alarm when date, h, m, s match
   *	1	0		0		0		0		Alarm when DoW, h, m, s match
   *
   *	Dy	A2M4	A2M3	A2M2	Rate
   *	X	1		1		1		Once per minute (at seconds = 00)
   *	X	1		1		0		Alarm when minutes match
   *	X	1		0		0		Alarm when hours and minutes match
   *	0	0		0		0		Alarm when date, hour, min match
   *	1	0		0		0		Alarm when DoW, hour, min match


  const int ALRM1_MATCH_EVERY_SEC  0b1111  // once a second
  const int ALRM1_MATCH_SEC        0b1110  // when seconds match
  const int ALRM1_MATCH_MIN_SEC    0b1100  // when minutes and seconds match
  const int ALRM1_MATCH_HR_MIN_SEC 0b1000  // when hours, minutes, and seconds match
      byte ALRM1_SET = ALRM1_MATCH_HR_MIN_SEC;

  const int ALRM2_ONCE_PER_MIN     0b111   // once per minute (00 seconds of every minute)
  const int ALRM2_MATCH_MIN        0b110   // when minutes match
  const int ALRM2_MATCH_HR_MIN     0b100   // when hours and minutes match
  const int ALRM2_DATE_TIME        0b000   // when hours and minutes match
      byte ALRM2_SET = ALRM2_DISABLE;

        // Set AlarmBits, ALRM2 first, followed by ALRM1
        AlarmBits = ALRM2_SET;
        AlarmBits <<= 4;
        AlarmBits |= ALRM1_SET;
  */

  SDon_RTCon();

  byte AlarmBits = 0b01001000;
  Clock.turnOffAlarm(1); //Turn off alarms before setting.
  Clock.turnOffAlarm(2);

  Clock.checkIfAlarm(1); //Clear alarm flags, do I need to do this here?
  Clock.checkIfAlarm(2); //Clear alarm flags

  // This is the primary alarm
  Clock.setA1Time(0, _hours, _minutes, _seconds, AlarmBits, true, false, false);
  delay(2);

  // This is a backup alarm that will wake the logger in case it misses the
  // first alarm for some unknown reason
  uint8_t _hours_backup = _hours;
  uint8_t _minutes_backup = _minutes+2;

  if(_minutes_backup > 59){_minutes_backup = _minutes_backup - 60; _hours_backup++;}
  if(_hours_backup > 23){_hours_backup = _hours_backup - 24;}
  Clock.setA2Time(0, _hours_backup, _minutes_backup, AlarmBits, true, false, false);  //setting as backup wake function
  delay(2);
  Clock.turnOnAlarm(1); //Turn on alarms.
  delay(1);
  Clock.turnOnAlarm(2);
  delay(1);
  //Serial.print(' '); Using Serial to fix code from freezing; don't understand.
  // Have looked through HW Serial, Print, Stream libraries; can't tell why yet
  // (was a <15 minute look)
  // Somehow, freezes when next needs to talk to clock if I don't do this!
  Serial.write(2); // write ASCII STX; somehow Serial comes to the rescue again!
  Serial.write(2); // STX just as a character that doesn't take much space in
                   // the Serial monitor
  Serial.flush();
  //delay(10);

  //bool ADy, Apm, A12h;
  //byte ADay, AHour, AMinute, ASecond;
  //Clock.getA1Time(ADay, AHour, AMinute, ASecond, AlarmBits, ADy, A12h, Apm);
  //Clock.getA2Time(ADay, AHour, AMinute, AlarmBits, ADy, A12h, Apm);
  //displayAlarms(); delay(10);  //Uncomment to see alarms on serial monitor.

}


void ALog::displayAlarms(){
  bool ADy; // Looks like this can be empty because it is defined within
            // function... meaning that DS3231 library could use some updates.
            // But will hold off on that until a full overhaul / upgrade /
            // migration to a fully RTClib-compatible system
  bool Apm; // Empty, but must be declared for clock function
  bool A12h = false;
  uint8_t ADay, AHour, AMinute, ASecond, AlarmBits;
  Serial.print(F("Alarm 1 (d/h/m/s): "));
	Clock.getA1Time(ADay, AHour, AMinute, ASecond, AlarmBits, ADy, A12h, Apm);
	/*
	if (ADy) {
		Serial.print(F("DoW "));
	}
	else {
		Serial.print(F("Date "));
	}
	*/
	Serial.print(AHour, DEC);
	Serial.print(':');
	Serial.print(AMinute, DEC);
	Serial.print(':');
	Serial.print(ASecond, DEC);
	Serial.print(':');
	if (A12h) {
		if (Apm) {
			Serial.print(F("pm"));
		} else {
			Serial.print(F("am"));
		}
	}
	if (Clock.checkAlarmEnabled(1)){
		Serial.print(F("enabled"));
  }
  else{
    Serial.print(F("not enabled"));
  }

	Serial.print('\n');
	// Display Alarm 2 information
	Serial.print(F("Alarm 2 (d/h/m): "));
	Clock.getA2Time(ADay, AHour, AMinute, AlarmBits, ADy, A12h, Apm);
	/*
	if (ADy){
		Serial.print(F("DoW "));
	}
    else
  {
    Serial.print(F("Date "));
  }
	Serial.print(ADay, DEC);
	Serial.print(' ');
  */
	Serial.print(AHour, DEC);
	Serial.print(' ');
	Serial.print(AMinute, DEC);
	Serial.print(' ');
	if (A12h) {
		if (Apm) {
			Serial.print(F("pm "));
		} else {
			Serial.print(F("am "));
		}
	}
	if (Clock.checkAlarmEnabled(2)) {
		Serial.print(F("enabled"));
	} else{
    Serial.print(F("not enabled"));
  }
  Serial.println();
	// display alarm bits
  //	Serial.print('\n');
  //	Serial.println(AlarmBits, BIN);
  //  Serial.println(Clock.getSecond());

  // Finally, print the current time to Serial
  //displayTime();
}

void ALog::checkAlarms(){
  bool ADy;
  bool Apm;
  bool A12h = false;

	Clock.checkIfAlarm(1);

//if (_use_sleep_mode){  //Removed by Chad 4/20/17
  if (Clock.checkIfAlarm(2)) {
	  Serial.println("Alarm missed! Resetting logger.");
    datafile.close();
    LEDwarn(5);
    delay(30);

    // Callback to set date and time in SD card file metadata
    // Following: https://forum.arduino.cc/index.php?topic=348562.0
    // See: https://github.com/NorthernWidget/Logger/issues/6
    SdFile::dateTimeCallback(_internalDateTime);

    if (!sd.begin(CSpin, SPI_HALF_SPEED)) {
      Serial.println(F("Card failed, or not present"));
      LEDwarn(20); // 20 quick flashes of the LED
    }
    // Prepare to record times when the alarms were missed and the watchdog
    // timer was needed to reset the logger
    start_logging_to_otherfile("Alarm_miss.txt");

    bool ADy;
    bool Apm;
    byte ADay, AHour, AMinute, ASecond, AlarmBits;
    otherfile.print(now.unixtime());
    otherfile.print(",");
    otherfile.print("Alarm: ");
    Clock.getA1Time(ADay, AHour, AMinute, ASecond, AlarmBits, ADy, A12h, Apm);
    otherfile.print(ADay, DEC);
    otherfile.print(" DoW");
    otherfile.print(' ');
    otherfile.print(AHour, DEC);
    otherfile.print(' ');
    otherfile.print(AMinute, DEC);
    otherfile.print(' ');
    otherfile.print(ASecond, DEC);
    otherfile.print(' ');
    if (A12h) {
	    if (Apm) {
		    otherfile.print("PM");
	    }
	    else {
		    otherfile.print("AM");
	    }
    }
    end_logging_to_otherfile();
    delay(20000); // Wait until WDT resets logger (<= 8 seconds)
  }
//}  //Removed by Chad 4/20/17
}

void ALog::displayTime(){
  /*
   Get current time.
   Always 24-hour clock
   */
  bool PM;
  Serial.print("UTC DATE/TIME: ");
  delay(5);
  now = RTC.now();
	Serial.print(2000+now.year(), DEC);
	Serial.print(F("."));
	// then the month
	Serial.print(now.month(), DEC);
	Serial.print(F("."));
	// then the date
	Serial.print(now.day(), DEC);
	Serial.print(F(" "));
	// and the day of the week
	//Serial.print(Clock.getDoW(), DEC);
	//Serial.print(' ');
	// Finally the hour, minute, and second
	Serial.print(now.hour(), DEC);
	Serial.print(":");
	Serial.print(now.minute(), DEC);
	Serial.print(":");
	Serial.println(now.second(), DEC);
	// Enough time to print
  delay(2);
}

void ALog::LEDwarn(uint8_t nflash)
{
  // Flash LED quickly to say that the SD card (and therefore the logger)
  // has not properly initialized upon restart
  // No
  for(uint8_t i=0;i<=nflash;i++){
    digitalWrite(LEDpin,HIGH);
    delay(40);
    digitalWrite(LEDpin,LOW);
    delay(40);
  }
}

void ALog::LEDgood()
{
  // Peppy blinky pattern to show that the logger has successfully initialized
  digitalWrite(LEDpin,HIGH);
  delay(1000);
  digitalWrite(LEDpin,LOW);
  delay(300);
  digitalWrite(LEDpin,HIGH);
  delay(100);
  digitalWrite(LEDpin,LOW);
  delay(100);
  digitalWrite(LEDpin,HIGH);
  delay(100);
  digitalWrite(LEDpin,LOW);
}

void ALog::LEDtimeWrong(uint8_t ncycles)
{
  // Syncopated pattern to show that the clock has probably reset to January
  // 1st, 2000
  for(uint8_t i=0;i<=ncycles;i++)
  {
    digitalWrite(LEDpin,HIGH);
    delay(250);
    digitalWrite(LEDpin,LOW);
    delay(100);
    digitalWrite(LEDpin,HIGH);
    delay(100);
    digitalWrite(LEDpin,LOW);
    delay(100);
  }
}

void ALog::unixDatestamp(){

  if (first_log_after_booting_up){
    now = RTC.now();
    // One row for date stamp; the next for real header info
    headerfile.print(now.unixtime());
    headerfile.println();
    headerfile.print("UNIX time stamp");
    headerfile.print(",");
  }

  now = RTC.now();
  datafile.print(now.unixtime());
  datafile.print(F(","));

  // Echo to serial
  Serial.print(now.unixtime());
  Serial.print(F(","));
}

void ALog::endLine(){
  // Ends the line in the file; do this at end of recording instance
  // before going back to sleep
  datafile.println();
  Serial.println();
}

float ALog::_vdivR(uint8_t pin, float Rref, uint8_t adc_bits, \
            bool Rref_on_GND_side, bool oversample_debug){
  // Same as public vidvR code, but returns value instead of
  // saving it to a file
  float _ADC;
  float _R;
  _ADC = analogReadOversample(pin, adc_bits, 1, oversample_debug);
  float _ADCnorm = _ADC/1023.0; // Normalize to 0-1
  if(Rref_on_GND_side){
    // Standard case for the provided slots for reference resistors
    // This is the default.
    _R = Rref/_ADCnorm - Rref; // R1 = (R2*Vin)/Vout - R2
  }
  else {
    // This could happen if an external sensor has a different setup for
    // its known and unknown resistors; in this case, place the reference
    // resistor between the analog pin and 3V3. (The sensor, internally,
    // has its thermistor connected to GND.)
    _R = Rref * (1. / ((1./_ADCnorm) - 1.)); // R2 = R1* (1 / ((Vin/Vout) - 1))
  }
  return _R;
}

void ALog::SDon_RTCon(){
  // Turn on power to clock and SD card
  digitalWrite(SDpowerPin,HIGH); //Chad -- one model's pull-ups attached to SDpowerPin
  digitalWrite(RTCpowerPin,HIGH);
  delay(20);
}

void ALog::SDoff_RTCsleep(){
  // Turn off power to the clock (RTC) and SD card
  // At this point, the RTC runs on VCC (if logger is powered... which it is
  // if this program is running) via the backup battery power supply.
  // This "tricks" it into turning off its I2C bus and saves power on the
  // board, but keeps its alarm functionality on.
  // (Idea to do this courtesy of Gerhard Oberforcher)
  digitalWrite(SDpowerPin,LOW); //Chad -- one model's pull-ups attached to SDpowerPin
  digitalWrite(RTCpowerPin,LOW);
  delay(2);
}

////////////////////////////////////////////////////////////
// PUBLIC UTILITY FUNCTIONS TO IMPLEMENT LOGGER IN SKETCH //
////////////////////////////////////////////////////////////

void ALog::sleep(){
  /**
   * @brief
   * Puts the ALog data logger into a low-power sleep mode
   *
   * @details
   * Sets the "IS_LOGGING" flag to false, disables the watchdog timer, and
   * puts the logger to sleep.
   */
  IS_LOGGING = false; // not logging when sleeping!

  wdt_disable();  //Disable the watchdog timer

  sleepNow();
}

void ALog::goToSleep_if_needed(){
  /**
   * @brief
   * Places logger into sleep mode iff this is being used.
   *
   * @details
   * Function is accessible from Arduino sketch; is designed for cases in
   * which an external override may be required.
   */
  if (_use_sleep_mode){
    sleep();
  }
}

void ALog::startLogging(){
  /**
   * @brief
   * Wakes the logger and starts logging
   *
   * @details
   * Wakes the logger: sets the watchdog timer (a failsafe in case the logger
   * hangs), checks and clears alarm flags, looks for rain gauge bucket tips
   * (if they occur during the middle of a logging event (ignore) or if they
   * include a command to read all sensors with a tip),
   * and starts to log to "datafile", if it can.
   *
   * <b>If the logger cannot reach the SD card,
   * it sends out an LED warning message of 20 rapid flashes.</b>
   */
  // Wake up

  delay(50);
  //Serial.println("Rise and shine!");

  wdt_disable();
  wdt_enable(WDTO_8S);    // Enable the watchdog timer interupt.
  // Enable ADC
  sbi(ADCSRA,ADEN);        // switch Analog to Digitalconverter ON
  // Turn power on
  SDon_RTCon();

  checkAlarms(); //Check and clear flag
  //displayAlarms();  // Verify Alarms and display time // Here for debugging
                      // why the alarms weren't going off after 1st log.
                      // Mysterious Serial.write (or .print) fixed it.

  // First, check if there was a bucket tip from the rain gage, if present
  if (NEW_RAIN_BUCKET_TIP){
    TippingBucketRainGage();
    if (LOG_ALL_SENSORS_ON_BUCKET_TIP){
      // If we want data recorded when the bucket tips, we don't want it to
      // interrupt a current logging step.
      // And IS_LOGGING will just stay true if we're interrupting that.
      IS_LOGGING = true; // First switch the IS_LOGGING flag because we're
                         // doing it now.
    }
  }
  // Then check if a logging event is supposed to occur
  // that is not part of a new bucket tip
  // IS_LOGGING is
  else if (IS_LOGGING == true){
    // Check if the logger has been awakend by someone pushing the button
    // If so, bypass everything else
    //Serial.println("LOG1!");
    if (digitalRead(manualWakePin) == LOW){
      // Brief light flash to show that logging is happening
      //Serial.println("LOG2!");
      digitalWrite(LEDpin, HIGH);
      delay(5); // to make sure tips aren't double-counted
      digitalWrite(LEDpin, LOW);
      // And now note that it is logging
    }
  }

  pinMode(SDpowerPin, OUTPUT); // Seemed to have forgotten between loops... ?

  // Callback to set date and time in SD card file metadata
  // Following: https://forum.arduino.cc/index.php?topic=348562.0
  // See: https://github.com/NorthernWidget/Logger/issues/6
  SdFile::dateTimeCallback(_internalDateTime);

  // Initialize logger
  if (!sd.begin(CSpin, SPI_HALF_SPEED)) {
    // Just use Serial.println: don't kill batteries by aborting code
    // on error
    Serial.println(F("Card failed, or not present"));
    // WARN THE END USER -- new feature after conversations with Amanda and
    // Crystal about SD cards not being seated correctly, and/or just not
    // knowing if they are.
    LEDwarn(20); // 20 quick flashes of the LED
  }
  delay(10);
  // Datestamp the start of the line
  unixDatestamp();
}

void ALog::endLogging(){
  /**
   * @brief
   * Endslogging and returns to sleep
   *
   * @details
   * Ends line, turns of SD card, and resets alarm: ready to sleep.
   *
   * Also runs tipping bucket rain gauge code (function that records time
   * stamp) if one is attached and activated.
   *
   * \b IMPORTANT: If the logger is not writing data to the card, and the card
   * is properly
   * inserted, the manually-set delay here may be the problem. We think we
   * have made it long enough, but because it is hard-coded, there could be
   * an unforeseen circumstance in which it is not!
   */
  endLine();
  // Write all of the data to the file
  // The buffer is 512 bytes -- so need to use this in-between
  // if there are too many bytes of data
  datafile.sync();
  // Headerfile should be closed at this point, and not reopened
  if (first_log_after_booting_up){
    end_logging_to_headerfile();
    first_log_after_booting_up = false; // the job is done.
  }
  delay(30);
  // THIS DELAY IS ***CRITICAL*** -- WITHOUT IT, THERE IS NOT SUFFICIENT
  // TIME TO WRITE THE DATA TO THE SD CARD!
  // Check right before going back to sleep if there has been a rain
  // gauge bucket tip while it has been on
  // This is a temporary solution!
  // (May be able to reduce delay if not going back to sleep -- i.e., write
  //  to card while logging next step.)
  if (NEW_RAIN_BUCKET_TIP){
    TippingBucketRainGage();
  }
  Serial.println("LOG!"); // This is better! The more we print, the harder it is
                          // to break the system! (????!!!!)
  //Serial.write(7); // Saved by another print statement. That's 3... there must
                   // be a reason!
                   // Without this, pressing the "LOG NOW" button would often
                   // cause the system to freeze.

  bool advance_alarm_flag = true;

  if (_use_sleep_mode){
    // Check if you have passed your logging time -- perhpas the LOG NOW
    // button was pressed, and not during / slightly before (and blocking)
    // the time for the next logging
    now = RTC.now();
    //Calculate for next alarm
    uint32_t seconds_in_day_now = now.hour()*3600 + now.minute()*60 + now.second();
    // 1 second padding in order to ensure that we jump to the next alarm
    // sooner rather than later
    // This is especially for the use of the LOG NOW button.
    uint32_t seconds_in_day_alarm = (_hours*3600 + _minutes*60 + _seconds - 1) % 86400;
    if (seconds_in_day_now < seconds_in_day_alarm){
      advance_alarm_flag = false;
    }

    if (advance_alarm_flag){
      // START HERE! MAKE THIS WORK FOR MIDNIGHT ROLLOVER
      _hours = _hours+hourInterval;
      _minutes = _minutes+minInterval;
      _seconds = _seconds+secInterval;
      if(_seconds > 59){_seconds = _seconds - 60; _minutes++;}
      if(_minutes > 59){_minutes = _minutes - 60; _hours++;}
      if(_hours > 23){_hours = _hours - 24;}

      alarm(_hours, _minutes, _seconds);  //Set new alarms.
    }
    //displayAlarms(); // Verify Alarms and display time
    delay(2);
    SDoff_RTCsleep();
    delay(2);
  }
  // After this step, since everything is in the loop() part of the Arduino
  // sketch, the sketch will cycle back back to sleep(...)
}

void ALog::sensorPowerOn(){
  /**
   * @brief Turn ON power to 3V3 regulator that connects to screw terminals.
   * @details
   * This powers external devices.
   */
  digitalWrite(SensorPowerPin,HIGH);
  delay(2);
}

void ALog::sensorPowerOff(){
  /**
   * @brief Turn OFF power to 3V3 regulator that connects to screw terminals.
   * @details
   * This cuts 3V3 power to external devices.
   */
  digitalWrite(SensorPowerPin,LOW);
  delay(2);
}

// DEPRECATED
void ALog::startAnalog(){
  /**
   * @brief Turn on power to analog sensors
   * @details
   * Actually just turns on 3V3 regulator that could power anything.
   * Keeping this around for backwards compatibility; look to
   * sensorPowerOn() for the preferred version of this.
   */
  sensorPowerOn();
}

// DEPRECATED
void ALog::endAnalog(){
  /**
   * @brief Turn off power to analog sensors
   * @details
   * Actually just turns off 3V3 regulator that could power anything.
   * Keeping this around for backwards compatibility; look to
   * sensorPowerOff() for the preferred version of this.
   */
  sensorPowerOff();
}

////////////////////////////////
// SENSOR INTERFACE FUNCTIONS //
////////////////////////////////

// Read analog pin
//////////////////////////////

float ALog::readPin(uint8_t pin){

  /**
   * @brief Read the analog value of a pin.
   *
   * @details
   * This function returns the analog to digital converter value (0 - 1023).
   * Results are displayed on the serial monitor and saved onto the SD card.
   *
   * @param pin is the analog pin number to be read.
   *
   * Example:
   * ```
   * alog.readPin(2);
   * ```
   *
   *
  */

  float pinValue = analogRead(pin);

  ///////////////
  // SAVE DATA //
  ///////////////

  if (first_log_after_booting_up){
    headerfile.print("Analog pin value");
    headerfile.print(",");
    headerfile.sync();
  }

  // SD write
  datafile.print(pinValue, 1);
  datafile.print(",");

  // Echo to serial
  Serial.print(pinValue, 1);
  Serial.print(",");

  return pinValue;

}

float ALog::readPinOversample(uint8_t pin, uint8_t bits){

  /**
   * @brief
   * Read the analog value of a pin, with extra resolution from oversampling
   *
   * @details
   * This function incorporates oversampling to extend the ADC precision
   * past ten bits by taking more readings and statistically combing them.
   * Results are displayed on the serial monitor and saved onto the SD card.
   *
   * @param pin is the analog pin number to be read.
   *
   * @param adc_bits is the reading precision in bits (2^adc_bits).
   * The ATMega328 (Arduino Uno and ALog BottleLogger core chip)
   * has a base ADC precision of 10 bits (returns values of 0-1023)
   * A reasonable maximum precision gain is (base_value_bits)+6, so
   * 16 bits is a reasonable maximum precision for the ALog BottleLogger.
   *
   * Example:
   * ```
   * alog.readPinOversample(2, 12);
   * ```
   *
   * Output values will range from 0-1023, but be floating-point.
   *
   * Readings that require more bits of precision will take longer.
   *
   *
  */

  float pinValue = analogReadOversample(pin, bits);

  ///////////////
  // SAVE DATA //
  ///////////////

  if (first_log_after_booting_up){
    headerfile.print("Analog pin value");
    headerfile.print(",");
    headerfile.sync();
  }

  // SD write
  datafile.print(pinValue,4);
  datafile.print(",");

  // Echo to serial
  Serial.print(pinValue,4);
  Serial.print(",");

  return pinValue;

}

// Thermistor - with b-value
//////////////////////////////

float ALog::thermistorB(float R0, float B, float Rref, float T0degC, \
            uint8_t thermPin, uint8_t ADC_resolution_nbits, \
            bool Rref_on_GND_side, bool oversample_debug, bool record_results){

  /**
   * @brief
   * Read the analog value of a pin, with extra resolution from oversampling
   *
   * @details
   * This function measures temperature using a thermistor characterised with
   * the B (or β) parameter equation, which is a simplification of the
   * Steinhart-Hart equation
   *
   * The function compares the thermistor risistance with the reference
   * resistor using a voltage divider.
   *
   * It returns a float of the temperature in degrees celsius.
   * Results are displayed on the serial monitor and saved onto the SD
   * card to four decimal places.
   *
   * @param R0 is the resistance of the thermistor at the known temperature
   * @param T0degC. [Ω]
   *
   * @param B is the β parameter of the thermistor. [K]
   *
   * @param Rref is the resistance of the corresponding reference resistor for \
   * the analog pin set by \b ThermPin (below). [Ω]
   *
   * @param T0degC is the temperature at which \b R0 was calibrated. [°C]
   *
   * @param thermPin is the analog pin number to be read. [-]
   *
   * @param ADC_resolution_nbits (10-16 for the ALog BottleLogger) is the
   * number of bits of ADC resolution used (oversampling for >10 bits) [bits]
   *
   * @param Rref_on_GND_side indicates the configuration of the voltage divider.
   * True if using Alog provided Reference resistor terminals. If false,
   * the reference resitor must be instead connected via the screw terminals.
   * This is set true for external sensors that are built to require a
   * VCC-side reference resistor.
   *
   * @param oversample_debug is true if you want a separate file, "Oversample.txt",
   * to record every individual reading used in the oversampling.
   *
   * @param record_results is true if you want to save results to the SD card and
   * print to the serial monitor.

   * Examples:
   * ```
   * // Cantherm from Digikey
   * // 10 kOhm @ 25degC, 3950 K b-value, 30 kOhm reference resistor, on
   * // analog Pin 1, 14-bit precision
   * alog.thermistorB(10000, 3950, 30000, 25, 2, 14);
   * // EPCOS, DigiKey # 495-2153-ND
   * // 10 kOhm @ 25degC, 3988 K b-value, 13.32 kOhm reference resistor, on
   * // analog Pin 1, 12-bit precision
   * alog.thermistorB(10000, 3988, 13320, 25, 1, 12);
   * ```
   *
  */

  // Voltage divider
  float Rtherm = _vdivR(thermPin, Rref, ADC_resolution_nbits, \
                        Rref_on_GND_side, oversample_debug);

  // B-value thermistor equations
  float T0 = T0degC + 273.15;
  float Rinf = R0*exp(-B/T0);
  float T = B / log(Rtherm/Rinf);

  // Convert to celsius
  T = T - 273.15;

  ///////////////
  // SAVE DATA //
  ///////////////

  if(record_results){

    if (first_log_after_booting_up){
      headerfile.print("Temperature [degC]");
      headerfile.print(",");
      headerfile.sync();
    }

    // SD write
    datafile.print(T, 4);
    datafile.print(F(","));

    // Echo to serial
    Serial.print(T, 4);
    Serial.print(F(","));
  }

  return T;

}

// HTM2500LF Humidity and Temperature Sensor
// by TE Connectivity Measurement Specialties
///////////////////////////////////////////////

void ALog::HTM2500LF_humidity_temperature(uint8_t humidPin, uint8_t thermPin, \
           float Rref_therm, uint8_t ADC_resolution_nbits){

  /**
   * @brief HTM2500LF Relative humidity and temperature sensor
   *
   * @details This function measures the relative humidity of using a HTM2500
   * tempurature and relative humidity module.
   * The relative humidity and temperature is measured using a 14 bit
   * oversampling method.
   * Results are displayed on the serial monitor and saved onto the SD
   * card to four decimal places.
   *
   * @param humidPin is the analog pin connected to the humidity output voltage
   * of the module.
   *
   * @param thermPin is the analog pin connected to the tempurature output voltage
   * of the module.
   *
   * @param Rref_therm is the value of the reference resistor that you use
   * with the built-in thermistor (reference resistor supplied separately,
   * placed in appropriate slot in header)
   *
   * @param ADC_resolution_nbits (10-16 for the ALog BottleLogger) is the
   * number of bits of ADC resolution used (oversampling for >10)
   *
   * Example:
   * ```
   * alog.HTM2500LF_humidity_temperature(1, 2, ?);
   * ```
   *
   * This function is designed for ratiometric operation -- that is, the
   * humidity sensor must be powered by the same voltage regulator that is
   * connected to the the analog reference pin -- for the ALog v2.0, this is
   * a high-precision 3V3 regulator.
  */

  // First, measure these pins
  // This will fully calculate and write the temperature data, too.
  // Normalized to 0-1 range
  float V_humid_norm = analogReadOversample(humidPin, ADC_resolution_nbits) \
                                                                 /1023.; // 0-1
  // Commenting out low and high temperature readings;
  // can calculate these after the fact, if needed
  // float Tmin = thermistorB(10000, 3347, Rref_therm, 25, thermPin, false, false, false);
  float Ttyp = thermistorB(10000, 3347, Rref_therm, 25, thermPin, ADC_resolution_nbits, false, false, false);
  //float Tmax = thermistorB(10000, 3413, Rref_therm, 25, thermPin, false, false, false);
  // Then, convert the normalized voltage into a humidity reading
  // The calibration is created for a 5V input, but the data sheet says it
  // is ratiometric, so I think I will just renormalize the voltage to
  // pretend that it is 5V input in order to get the right input values
  // for the equation. Just multiply by 5, and then I can use the equation
  // that is designed for 5V!

  // T error is small, and has a small effect on humidity -- much smaller
  // than published error (see data sheet) -- maybe eventually code error
  // into this function. So just use typical thermistor values.
  float Vh = 5000 * V_humid_norm; // mV
  //float Vh_real = 3300 * V_humid_norm; // switching 3.3V basis

  // RH in percent
  // Must use the pow(base, int) function or do multiplication the long way...
  float RH = ( (-1.9206E-9 * Vh*Vh*Vh) + (1.437E-5 * Vh*Vh) + \
               (3.421E-3 * Vh) - 12.4 ) \
             / (1 + (Ttyp - 23) * 2.4E-3);

  ///////////////
  // SAVE DATA //
  ///////////////

  if (first_log_after_booting_up){
    headerfile.print("Relative humidity [%]");
    headerfile.print(",");
    headerfile.print("Temperature [degC]");
    headerfile.print(",");
    headerfile.sync();
  }

  // SD write
  datafile.print(RH, 4);
  datafile.print(F(","));
  datafile.print(Ttyp, 2);
  datafile.print(F(","));

  // Echo to serial
  Serial.print(RH, 4);
  Serial.print(F(","));
  datafile.print(Ttyp, 2);
  datafile.print(F(","));

}


// HTM1500LF Humidity and Temperature Sensor
// by TE Connectivity Measurement Specialties
///////////////////////////////////////////////

void ALog::HM1500LF_humidity_with_external_temperature(uint8_t humidPin, \
           float R0_therm, float B_therm, float Rref_therm, \
           float T0degC_therm, uint8_t thermPin_therm, \
           uint8_t ADC_resolution_nbits){

  /**
   * @brief HM1500LF Relative humidity sensor with external temperature
   * correction
   *
   * @details This function measures the relative humidity of using a HTM1500
   * relative humidity sensor and an external thermistor.
   * The relative humidity and temperature are measured using an
   * oversampling method.
   * Results are displayed on the serial monitor and saved onto the SD card
   * to four decimal places. Temperature and relative humidity are recorded.
   *
   * @param humidPin is the analog pin connected to the humidity output voltage
   * of the module.
   *
   * @param R0_therm is the resistance of the thermistor at the known temperature.
   *
   * @param B_therm is the B- or β- parameter of the thermistor.
   *
   * @param Rref_therm is the resistance of the corresponding reference resistor for
   * that analog pin.
   *
   * @param T0degC_therm is a thermistor calibration.
   *
   * @param thermPin_therm is the analog pin connected to the tempurature output voltage
   * of the module.
   *
   * @param ADC_resolution_nbits (10-16 for the ALog BottleLogger) is the
   * number of bits of ADC resolution used (oversampling for >10)
   *
   * Example:
   * ```
   * alog.HM1500LF_humidity_with_external_temperature1,10000,3950,10000,25,1,12);
   * ```
   *
  */

  // First, measure these pins
  // This will fully calculate and write the temperature data, too.
  float V_humid_norm = analogReadOversample(humidPin, ADC_resolution_nbits)/1023.; // 0-1
  // Will write temperature to file here
  float T = thermistorB(R0_therm, B_therm, Rref_therm, T0degC_therm, \
                        thermPin_therm, ADC_resolution_nbits, true, false, true);

  // Then, convert the normalized voltage into a humidity reading
  // The calibration is created for a 5V input, but the data sheet says it
  // is ratiometric, so I think I will just renormalize the voltage to
  // pretend that it is 5V input in order to get the right input values
  // for the equation. Just multiply by 5!

  // T error is small, and has a small effect on humidity -- much smaller
  // than published error (see data sheet) -- maybe eventually code error
  // into this function. So just use typical thermistor values.
  float Vh = 5000 * V_humid_norm; // mV
  //float Vh_real = 3300 * V_humid_norm; // switching 3.3V basis

  // RH in percent
  // Got to use the pow(base, int) function or do multiplication the long way...
  float RH_no_T_corr = (-1.91E-9 * Vh*Vh*Vh) + \
+                      (1.33E-5 * Vh*Vh) + (9.56E-3 * Vh) - 2.16E1;
  float RH = RH_no_T_corr + 0.05 * (T - 23);

  ///////////////
  // SAVE DATA //
  ///////////////

  // Print normalized 0-1 voltage in case my 5V conversion doesn't work the way
  // I think it will -- though if it is ratiometric, I think it should.

  if (first_log_after_booting_up){
    headerfile.print("Relative humidity [%]");
    headerfile.print(",");
    headerfile.sync();
  }

  // SD write
  //datafile.print(V_humid_norm);
  //datafile.print(F(","));
  datafile.print(RH, 4);
  datafile.print(F(","));

  // Echo to serial
  //Serial.print(V_humid_norm);
  //Serial.print(F(","));
  Serial.print(RH, 4);
  Serial.print(F(","));

}


// MaxBotix ruggedized standard size ultrasonic rangefinder:
// 1 cm = 1 10-bit ADC interval
//////////////////////////////////////////////////////////////

void ALog::ultrasonicMB_analog_1cm(uint8_t nping, uint8_t Ex, uint8_t sonicPin,
           bool writeAll){

  /**
   * @brief Old 1-cm resolution Maxbotix ultrasonic rangefinders: analog
   * measurements
   *
   * @details
   * This function measures the distance between the ultrasonic sensor and an
   * acustically reflective surface, typically water or snow.
   * Measures distance in centimeters.
   * Results are displayed on the serial monitor and saved onto the SD card.
   *
   * This is for the older MaxBotix sensors, whose maximum precision is
   * in centimeters.
   *
   * @param nping is the number of range readings to take (number of pings).
   * The mean range will be calculated and output to the serial monitor and
   * SD card followed by the standard deviation.
   *
   * @param EX is a digital output pin used for an excitation pulse.  If maxbotix
   * sensor is continuously powered a reading will be taken when this pin is
   * flashed high.
   * Set to '99' if excitation pulse is not needed.
   *
   * @param sonicPin is the analog input channel hooked up to the maxbotix sensor.
   *
   * @param writeAll will write each reading of the sensor (each ping) to the
   * serial monitor and SD card.
   *
   * Example:
   * ```
   * alog.ultrasonicMB_analog_1cm(10, 99, 2, 0);
   * ```
   * Note that sensor should be mounted away from supporting structure.
   * For a mast that is 5 meters high (or higher) the sensor should be
   * mounted at least 100cm away from the mast.
   * For a mast that is 2.5 meters high (or lower) the sensor should be
   * at least 75cm away from the mast.
  */

  float range; // The most recent returned range
  float ranges[nping]; // Array of returned ranges
  float sumRange = 0; // The sum of the ranges measured
  float meanRange; // The average range over all the pings

//  Serial.println();
  // Get range measurements
  // Get rid of any trash; Serial.flush() unnecessary; main thing that
  // is important is getting the 2 pings of junk out of the way
  Serial.flush();
  for (int i=1; i<=2; i++){
    if(Ex != 99){
      digitalWrite(Ex,HIGH);
        delay(1);
      digitalWrite(Ex,LOW);
      }
    delay(100);
    }
  for(int i=1;i<=nping;i++){
    if(Ex != 99){
      digitalWrite(Ex,HIGH);
        delay(1);
      digitalWrite(Ex,LOW);
      }
    delay(100);
    range = analogRead(sonicPin);
    ranges[i-1] = range; // 10-bit ADC value = range in cm
                         // C is 0-indexed, hence the "-1"
    if (writeAll){
      if (first_log_after_booting_up){
        headerfile.print("Distance [cm]");
        headerfile.print(",");
        headerfile.sync();
      }
      Serial.print(range);
      Serial.print(F(","));
      datafile.print(range);
      datafile.print(F(","));
      //SDpowerOff();
    }
  sumRange += range;
  }

  // Find mean of range measurements from sumRange and nping
  meanRange = sumRange/nping;

  // Find standard deviation
  float sumsquares = 0;
  float sigma;
  for(int i=0;i<nping;i++){
    // Sum the squares of the differences from the mean
    sumsquares += square(ranges[i]-meanRange);
  }
  // Calculate stdev
  sigma = sqrt(sumsquares/nping);

  ///////////////
  // SAVE DATA //
  ///////////////

  delay(10);

  if (first_log_after_booting_up){
    headerfile.print("Mean distance [cm]");
    headerfile.print(",");
    headerfile.print("Standard deviation distance [cm]");
    headerfile.print(",");
    headerfile.sync();
  }

  datafile.print(meanRange);
  datafile.print(F(","));
  datafile.print(sigma);
  datafile.print(F(","));

  // Echo to serial
  Serial.print(meanRange);
  Serial.print(F(","));
  Serial.print(sigma);
  Serial.print(F(","));

}

void ALog::maxbotixHRXL_WR_analog(uint8_t nping, uint8_t sonicPin, uint8_t EX, \
           bool writeAll, uint8_t ADC_resolution_nbits){
  /**
   * @brief Newer 1-mm precision MaxBotix rangefinders: analog readings
   *
   * @details
   * This function measures the distance between the ultrasonic sensor and an \
   * acoustically-reflective surface, typically water or snow.
   * Measures distance in milimeters.
   * Results are displayed on the serial monitor and saved onto the SD card.
   *
   * @param nping is the number of range readings to take (number of pings).
   * The mean range will be calculated and output to the serial monitor
   * and SD card followed by the standard deviation.
   *
   * @param sonicPin is the analog input channel hooked up to the maxbotix sensor.
   *
   * @param EX is a digital output pin used for an excitation pulse.
   * If maxbotix sensor is continuously powered, a reading will be taken when
   * this pin is flashed high. Set to '99' if excitation pulse is not needed.
   *
   * @param writeAll will write each reading of the sensor (each ping)
   * to the serial monitor and SD card.
   *
   * @param ADC_resolution_nbits (10-16 for the ALog BottleLogger) is the
   * number of bits of ADC resolution used (oversampling for >10 bits)
   *
   * Example:
   * ```
   * alog.maxbotixHRXL_WR_analog(10,A2,99,0);
   * ```
   * Note that sensor should be mounted away from supporting structure. These
   * are the standard recommendations:
   * * For a mast that is 5 meters high (or higher) the sensor should be
   *   mounted at least 100cm away from the mast.
   * * For a mast that is 2.5 meters high (or lower) the sensor should be at
   *   least 75cm away from the mast.
   *
   * <b>However, in our tests, the sensors with filtering algorithms function
   * perfectly well even when positioned close to the mast, and a short mast
   * increases the rigidity of the installation. This was tested in the lab
   * by placing the MaxBotix sensor flush with table legs and testing distance
   * readings to the floor.</b>
   *
  */

  float range; // The most recent returned range
  float ranges[nping]; // Array of returned ranges
  float sumRange = 0; // The sum of the ranges measured
  float meanRange; // The average range over all the pings
  int sp; // analog reading of sonic pin; probably unnecessary, but Arduino warns against having too many fcns w/ artihmetic, I think

  // Get range measurements
  // Get rid of any trash; Serial.flush() unnecessary; main thing that is important is
  // getting the 2 pings of junk out of the way
  Serial.flush();

  for (int i=1; i<=2; i++){
    if(EX != 99){
      digitalWrite(EX,HIGH);
        delay(1);
      digitalWrite(EX,LOW);
      }
    delay(100);
    }
  sp = analogRead(sonicPin);
  for(int i=1;i<=nping;i++){
    if(EX != 99){
      digitalWrite(EX,HIGH);
        delay(1);
      digitalWrite(EX,LOW);
      }
    delay(100);
    sp = analogRead(sonicPin);
    range = (sp + 1) * 5;
    ranges[i-1] = range; // 10-bit ADC value (1--1024) * 5 = range in mm
                         // C is 0-indexed, hence the "-1"
    if (writeAll){
      if (first_log_after_booting_up){
        headerfile.print("Distance [mm]");
        headerfile.print(",");
        headerfile.sync();
      }
      Serial.print(range, 0);
      Serial.print(F(","));
      datafile.print(range, 0);
      datafile.print(F(","));
      //SDpowerOff();
    }
  sumRange += range;
  }

  // Find mean of range measurements from sumRange and nping
  meanRange = sumRange/nping;

  // Find standard deviation
  float sumsquares = 0;
  float sigma;
  for(int i=0;i<nping;i++){
    // Sum the squares of the differences from the mean
    sumsquares += square(ranges[i]-meanRange);
  }
  // Calculate stdev
  sigma = sqrt(sumsquares/nping);

  ///////////////
  // SAVE DATA //
  ///////////////

  if (first_log_after_booting_up){
    headerfile.print("Mean distance [mm]");
    headerfile.print(",");
    headerfile.print("Standard deviation distance [mm]");
    headerfile.print(",");
    headerfile.sync();
  }

  datafile.print(meanRange);
  datafile.print(F(","));
  datafile.print(sigma);
  datafile.print(F(","));

  // Echo to serial
  Serial.print(meanRange);
  Serial.print(F(","));
  Serial.print(sigma);
  Serial.print(F(","));

}

float ALog::maxbotixHRXL_WR_Serial(uint8_t Ex, uint8_t npings, bool writeAll, \
            int maxRange, bool RS232){
  /**
   * @brief
   * Uses the UART interface to record data from a MaxBotix sensor.
   *
   * @details
   * NOTE: THIS HAS CUASED LOGGERS TO FREEZE IN THE PAST; WHILE IT IS QUITE
   * LIKELY THAT THE ISSUE IS NOW SOLVED, MORE TESTING IS REQUIRED.
   * (ADW, 26 NOVEMBER 2016) (maybe solved w/ HW Serial?)
   *
   * @param Ex Excitation pin that turns the sensor on; if this is not needed (i.e.
   * you are turning main power off and on instead), then just set this to a
   * value that is not a pin, and ensure that you turn the power to the sensor
   * off and on outside of this function
   *
   * @param npings Number of pings over which you average; each ping itself
   * includes ten short readings that the sensor internally processes
   *
   * @param writeAll will write each reading of the sensor (each ping)
   * to the serial monitor and SD card.
   *
   * @param maxRange The range (in mm) at which the logger maxes out; this will
   * be remembered to check for errors and to become a nodata values
   *
   * @param RS232 this is set true if you use inverse (i.e. RS232-style) logic;
   * it works at standard logger voltages (i.e. it is not true RS232). If
   * false, TTL logic will be used.
   *
   *
   * Example:
   * ```
   * // Digital pin 7 controlling sensor excitation, averaging over 10 pings,
   * // not recording the results of each ping, and with a maximum range of
   * // 5000 mm using standard TTL logic
   * alog.maxbotixHRXL_WR_Serial(7, 10, false, 5000, false);
   *
   * ```
   */

  // Stores the ranging output from the MaxBotix sensor
  int myranges[npings]; // Declare an array to store the ranges [mm] // Should be int, but float for passing to fcns
  // Get nodata value - 5000 or 9999 based on logger max range (in meters)
  // I have also made 0 a nodata value, because it appears sometimes and shouldn't
  // (minimum range = 300 mm)
  int nodata_value;
  if (maxRange == 5){
    nodata_value = 5000;
  }
  else if (maxRange == 10){
    nodata_value = 9999;
  }
  // Put all of the range values in the array
  for (int i=0; i<npings; i++){
    // Must add int Rx to use this; currently, don't trust SoftwareSerial
    // myranges[i] = maxbotix_soft_Serial_parse(Ex, Rx, RS232);
    myranges[i] = maxbotix_Serial_parse(Ex);
  }
  // Then get the mean and standard deviation of all of the data
  int npings_with_nodata_returns = 0;
  unsigned long sum_of_good_ranges = 0;
  int good_ranges[npings];
  int j=0;
  for (int i=0; i<npings; i++){
    if (myranges[i] != nodata_value && myranges[i] != 0){
      sum_of_good_ranges += myranges[i];
      good_ranges[j] = myranges[i];
      j++;
    }
    else{
      npings_with_nodata_returns ++;
    }
  }
  float npings_with_real_returns = npings - npings_with_nodata_returns;
  float mean_range;
  float standard_deviation;
  // Avoid div0 errors
  if (npings_with_real_returns > 0){
    mean_range = sum_of_good_ranges / npings_with_real_returns;
    standard_deviation = standard_deviation_from_array(good_ranges, npings_with_real_returns, mean_range);
  }
  else {
    mean_range = -9999;
    standard_deviation = -9999;
  }
  // Write all values if so desired
  if (writeAll){
    for (int i=0; i<npings; i++){
      datafile.print(myranges[i]);
      datafile.print(F(","));
      // Echo to serial
      Serial.print(myranges[i]);
      Serial.print(F(","));
    }

  }

  ///////////////
  // SAVE DATA //
  ///////////////

  if (first_log_after_booting_up){
    headerfile.print("Mean distance [mm]");
    headerfile.print(",");
    headerfile.print("Standard deviation distance [mm]");
    headerfile.print(",");
    headerfile.print("Number of readings with non-error returns");
    headerfile.print(",");
    headerfile.sync();
  }

  // Always write the mean, standard deviation, and number of good returns
  datafile.print(mean_range);
  datafile.print(F(","));
  datafile.print(standard_deviation);
  datafile.print(F(","));
  datafile.print(npings_with_real_returns);
  datafile.print(F(","));

  // Echo to serial
  Serial.print(mean_range);
  Serial.print(F(","));
  Serial.print(standard_deviation);
  Serial.print(F(","));
  Serial.print(npings_with_real_returns);
  Serial.print(F(","));

  // return mean range for functions that need it, e.g., to trigger camera
  return mean_range;
}

float ALog::standard_deviation_from_array(float values[], int nvalues,
            float mean){
  float sumsquares = 0;
  for (int i=0; i<nvalues; i++){
    sumsquares += square(values[i] - mean);
  }
  return sqrt(sumsquares/nvalues);
}

float ALog::standard_deviation_from_array(int values[], int nvalues,
            float mean){
  float sumsquares = 0;
  for (int i=0; i<nvalues; i++){
    sumsquares += square(values[i] - mean);
  }
  return sqrt(sumsquares/nvalues);
}

int ALog::maxbotix_Serial_parse(uint8_t Ex){
  // NOTE: Currently assumes only one Serial port.
  // Excites the MaxBotix sensor and receives its ranging output
  char range[7]; // R####<\r>, so R + 4 chars + carriage return + null
  Serial.end(); // End 38400 bps computer comms
  Serial.begin(9600); // Start 9600 bps logger comms
  //Excite the sensor to produce a pulse
  pinMode(Ex, OUTPUT);
  digitalWrite(Ex, HIGH);
  delay(1);
  digitalWrite(Ex, LOW);
  // Record the result of the ranging
  int i=0; // counter
  // Not sure if this will work - maybe loop around to the other end of the array?
  while (range[i-1] != 13){
    if (Serial.available()){
      range[i] = Serial.read();
      i++;
    }
  }
  Serial.end();
  Serial.begin(38400);
  // Convert to integer
  char r2[4]; // range stripped of all of its extra characters
  for (int i=1; i<5; i++){
    r2[i-1] = range[i];
  }
  int r3 = atol(r2);
  return r3;
}

/*
// CURRENTLY NOT USED -- ADW DOES NOT TRUST SOFTWARE SERIAL TO MAINTAIN
// TIMING (AND NOT HANG)
int ALog::maxbotix_soft_Serial_parse(int Ex, int Rx, bool RS232){
  // Excites the MaxBotix sensor and receives its ranging output
  char range[7]; // R####<\r>, so R + 4 chars + carriage return + null
  SoftwareSerial mySerial(Rx, -1, RS232); // RX, TX, inverse logic - RS232 true, TTL false; defaults to TTL (false)
  mySerial.begin(9600);
  //Excite the sensor to produce a pulse
  pinMode(Ex, OUTPUT);
  digitalWrite(Ex, HIGH);
  delay(1);
  digitalWrite(Ex, LOW);
  // Record the result of the ranging
  int i=0; // counter
  // Not sure if this will work - maybe loop around to the other end of the array?
  while (range[i-1] != 13){
    if (mySerial.available()){
      range[i] = mySerial.read();
      i++;
    }
  }
  //Serial.print(range);
  // Convert to integer
  char r2[4]; // range stripped of all of its extra characters
  for (int i=1; i<5; i++){
    r2[i-1] = range[i];
  }
  int r3 = atol(r2);
  //Serial.print(", ");
  //Serial.print(r3);
  //Serial.print("; ");
  return r3;
  //return atol(r2); // Return integer values in mm; no parsing of error values
}
*/

void ALog::Inclinometer_SCA100T_D02_analog_Tcorr(uint8_t xPin, uint8_t yPin, \
           float Vref, float Vsupply, float R0_therm, float B_therm, \
           float Rref_therm, float T0degC_therm, uint8_t thermPin_therm, \
           uint8_t ADC_resolution_nbits){
  /**
   * @brief
   * Inclinometer, including temperature correction from an external sensor.
   *
   * @details
   * * +/- 90 degree inclinometer, measures +/- 1.0g
   * * Needs 4.75--5.25V input (Vsupply)
   * * In typical usage, turned on and off by a switching 5V charge pump or
   *   boost converter
   *
   * @param xPin Analog pin number corresponding to x-oriented tilts
   *
   * @param yPin Analog pin number corresponding to y-oriented tilts
   *
   * @param Vref is the reference voltage of the analog-digital comparator; it is
   * 3.3V on the ALog.
   *
   * @param Vsupply is the input voltage that drives the sensor, and is typically
   * between 3.3 and 5V.
   *
   * @param R0_therm is a thermistor calibration.
   *
   * @param B_therm is the B- or β- parameter of the thermistor.
   *
   * @param Rref_therm is the resistance of the corresponding reference resistor for
   * that analog pin.
   *
   * @param T0degC_therm is a thermistor calibration.
   *
   * @param thermPin_therm is the analog pin connected to the tempurature output voltage
   * of the module.
   *
   * @param ADC_resolution_nbits (10-16 for the ALog BottleLogger) is the
   * number of bits of ADC resolution used (oversampling for >10).
   * It is applied to both the inclinomter and its temperature correction
   *
   * Example:
   * ```
   * alog.Inclinometer_SCA100T_D02_analog_Tcorr(6, 2, 3.285, 5.191, \
   *      10080.4120953, 3298.34232031, 10000, 25, 0);
   * ```
   *
  */

  float Vout_x = (analogReadOversample(xPin, ADC_resolution_nbits) / 1023.) \
                 * Vref;
  float Vout_y = (analogReadOversample(yPin, ADC_resolution_nbits) / 1023.) \
                 * Vref;

  float Offset = Vsupply/2.;
  float Sensitivity = 2.;

  // Temperature correction
  float T = thermistorB(R0_therm, B_therm, Rref_therm, T0degC_therm, \
                        thermPin_therm, ADC_resolution_nbits, \
                        true, false, false);
  // Sensitivity correction for Scorr
  float Scorr = -0.00011 * T*T + 0.0022 * T + 0.0408;

  float Sensitivity_compensated = Sensitivity * ( 1 + Scorr/100.);

  float angle_x_radians = asin( (Vout_x - Offset)/Sensitivity_compensated );
  float angle_y_radians = asin( (Vout_y - Offset)/Sensitivity_compensated );

  float angle_x_degrees = 180./3.14159 * angle_x_radians;
  float angle_y_degrees = 180./3.14159 * angle_y_radians;

  ///////////////
  // SAVE DATA //
  ///////////////

  if (first_log_after_booting_up){
    headerfile.print("Inclinometer voltage (x-axis) [V]");
    headerfile.print(",");
    headerfile.print("Inclinometer voltage (y-axis) [V]");
    headerfile.print(",");
    headerfile.print("Inclinometer tilt (x-axis) [degrees]");
    headerfile.print(",");
    headerfile.print("Inclinometer tilt (y-axis) [degrees]");
    headerfile.print(",");
    headerfile.sync();
  }

  // SD write
  datafile.print(Vout_x);
  datafile.print(F(","));
  datafile.print(Vout_y);
  datafile.print(F(","));
  datafile.print(angle_x_degrees);
  datafile.print(F(","));
  datafile.print(angle_y_degrees);
  datafile.print(F(","));

  // Echo to serial
  //int a = analogRead(xPin) - 512;
  //int b = analogRead(yPin) - 512;
  //Serial.print(a);
  //Serial.print(F(","));
  //Serial.print(b);
  //Serial.print(F(","));
  //Serial.print(VDD);
  //Serial.print(F(","));
  Serial.print(Vout_x);
  Serial.print(F(","));
  Serial.print(Vout_y);
  Serial.print(F(","));
  Serial.print(angle_x_degrees);
  Serial.print(F(","));
  Serial.print(angle_y_degrees);
  Serial.print(F(","));

}

void ALog::Anemometer_reed_switch(uint8_t interrupt_pin_number, \
           unsigned long reading_duration_milliseconds, \
           float meters_per_second_per_rotation){
  /**
   * @brief
   * Anemometer that flips a reed switch each time it spins.
   *
   * @param interrupt_pin_number is the digital pin number corresponding to
   * the appropriate interrupt; it uses the Arduino digitalPinToInterrupt(n_pin)
   * function to properly attach the interrupt. On the ALog BottleLogger, this
   * number will always be \b 3.
   *
   * @param reading_duration_milliseconds How long will you count revolutions?
   * Shorter durations save power, longer durations increase accuracy;
   * very long durations will produce long-term averages. Typical values are
   * a few seconds.
   *
   * @param meters_per_second_per_rotation: Conversion factor between revolutions
   * and wind speed. For the Inspeed Vortex wind sensor that we have used
   * (http://www.inspeed.com/anemometers/Vortex_Wind_Sensor.asp),
   * this is: <b>2.5 mph/Hz = 1.1176 (m/s)/Hz</b>
   *
   * @details
   * This function depends on the global variable \b rotation_count.
   *
   * Example:
   * ```
   * // 4-second reading with Inspeed Vortex wind sensor on digital pin 3
   * // (interrupt 1), returned in meters per second
   * alog.Anemometer_reed_switch(3, 4000, 1.1176);
   * ```
   *
   */

  // I plan for no more than 40 Hz (100 mph), so will have a delay of
  // 10 ms between rotations to debounce the input. This should allow
  // up to 250 mph, at which point larger problems likely exist...

  // Look up: http://arduino.stackexchange.com/questions/12587/how-can-i-handle-the-millis-rollover

  rotation_count = 0; // Global variable
  //int rotation_count_local;
  float rotation_Hz;
  float wind_speed_meters_per_second;
  float reading_duration_seconds = reading_duration_milliseconds / 1000.;

  pinMode(interrupt_pin_number, INPUT);
  digitalWrite(interrupt_pin_number, HIGH);

  unsigned long millis_start = millis();
  attachInterrupt(digitalPinToInterrupt(interrupt_pin_number), \
                  _anemometer_count_increment, FALLING);

  // Avoid rollovers by comparing unsigned integers with the
  // same number of bits
  // Wait in while loop while interrupt can increment counter.
  while (millis() - millis_start <= reading_duration_milliseconds){
  }
  Serial.print(""); // Well, this is weird, but required to work.
  detachInterrupt(digitalPinToInterrupt(interrupt_pin_number));

  rotation_Hz = rotation_count / reading_duration_seconds;
  wind_speed_meters_per_second = rotation_Hz * meters_per_second_per_rotation;

  ///////////////
  // SAVE DATA //
  ///////////////

  if (first_log_after_booting_up){
    headerfile.print("Number of rotations");
    headerfile.print(",");
    headerfile.print("Rotation frequency [Hz]");
    headerfile.print(",");
    headerfile.print("Wind speed [m/s]");
    headerfile.print(",");
    headerfile.sync();
  }
  // Note: should estimate error based on +/- 1 rotation (depending on whether
  // just starting or just ending at the measurement start time)

  // SD write
  datafile.print(rotation_count);
  datafile.print(F(","));
  datafile.print(rotation_Hz, 4);
  datafile.print(F(","));
  datafile.print(wind_speed_meters_per_second, 4);
  datafile.print(F(","));

  // Echo to serial
  Serial.print(rotation_count);
  Serial.print(F(","));
  Serial.print(rotation_Hz, 4);
  Serial.print(F(","));
  Serial.print(wind_speed_meters_per_second, 4);
  Serial.print(F(","));

}

void ALog::Wind_Vane_Inspeed(uint8_t vanePin){
  /**
   * @brief
   * Wind vane: resistance changes with angle to wind.
   *
   * @param vanePin is the analog pin that reads the wind vane resistance
   *
   * @details
   * This function is specialized for the Inspeed eVane2.
   * Here, a resistance of 0 equates to wind from the north, and
   * resistence increases in a clockwise direction.
   *
   * Connect one wire to power supply, one wire to analog pin, one wire to GND
   *
   * From documentation:
   * * 5 - 95% of power supply input voltage = 0 to 360 degrees of rotation.
   * * Uses Hall Effect Sensor
   * * Don't forget to use set screw to zero wind sensor before starting!
   *
   * Example:
   * ```
   * // After setting up and zeroing the eVane to North, you wire it to
   * // analog pin 7 on the ALog
   * alog.Wind_Vane_Inspeed(A7);
   * ```
   *
   */
  float Vin_normalized = (analogRead(vanePin) / 1023.);
  float Vin_stretched = (Vin_normalized - 0.05) / 0.9;
  float Wind_angle = Vin_stretched * 360.; // Degrees -- azimuth

  ///////////////
  // SAVE DATA //
  ///////////////

  // SD write
  datafile.print(Wind_angle);
  datafile.print(F(","));

  // Echo to serial
  Serial.print(Wind_angle);
  Serial.print(F(","));
}

void ALog::Pyranometer(uint8_t analogPin, float raw_mV_per_W_per_m2, \
           float gain, float Vref, uint8_t ADC_resolution_nbits){
  /**
   * @brief
   * Pyranometer wtih instrumentation amplifier
   *
   * @details
   * Pyranomiter is from Kipp and Zonen
   *
   * nominal raw_output_per_W_per_m2_in_mV = 10./1000.; // 10 mV at 1000 W/m**2
   *
   * Actual raw output is based on calibration.
   *
   * @param analogPin is the pin that receives the amplified voltage input
   *
   * @param raw_mV_per_W_per_m2 is the conversion factor of the pyranometer:
   * number of millivolts per (watt/meter^2).
   * This does not include amplification!
   *
   * @param gain is the amplification factor
   *
   * @param Vref is the reference voltage of the ADC; on the ALog, this is
   * a precision 3.3V regulator (unless a special unit without this regulator
   * is ordered; the regulator uses significant power)
   *
   * @param ADC_resolution_nbits (10-16 for the ALog BottleLogger) is the
   * number of bits of ADC resolution used (oversampling for >10 bits)
   *
   * Example:
   * ```
   * // Using precision voltage reference and 16-bit resolution (highest
   * // defensible oversampling resolution)
   * alog.Pyranometer(A0, 0.0136, 120, 3.300, 16);
   * ```
   */

  // V
  // Vref V --> mV
  float Vin = (analogReadOversample(analogPin, ADC_resolution_nbits) / 1023.) \
               * Vref * 1000.;
  //float Vin = Vref * 1000. * analogRead(analogPin) / 1023.; // No oversampling
  float Radiation_W_m2 = Vin / (raw_mV_per_W_per_m2 * gain);

  ///////////////
  // SAVE DATA //
  ///////////////

  if (first_log_after_booting_up){
    headerfile.print("Radiation [W/m^2]");
    headerfile.print(",");
    headerfile.sync();
  }

  // SD write
  datafile.print(Radiation_W_m2, 4);
  datafile.print(F(","));

  // Echo to serial
  Serial.print(Radiation_W_m2, 4);
  Serial.print(F(","));
}

float ALog::analogReadOversample(uint8_t pin, uint8_t adc_bits, \
            uint8_t nsamples, bool debug){
  /**
   * @brief
   * Higher analog resolution through oversampling
   *
   * @details
   * This function incorporates oversampling to extend the ADC precision
   * past ten bits by taking more readings and statistically combing them.
   *
   * Returns a floating point number between 0 and 1023 in order to be
   * intechangable with the Arduino core AnalogRead() function
   *
   * It is often used within other sensor functinons to increase measurement
   * precision.
   *
   * @param pin is the analog pin number
   *
   * @param adc_bits is the reading precision in bits (2^adc_bits).
   * The ATMega328 (Arduino Uno and ALog BottleLogger core chip)
   * has a base ADC precision of 10 bits (returns values of 0-1023)
   * A reasonable maximum precision gain is (base_value_bits)+6, so
   * 16 bits is a reasonable maximum precision for the ALog BottleLogger.
   *
   * @param nsamples is the number of times you want to poll the particular
   * sensor and write the output to file.
   *
   * @param debug is a flag that, if true, will write all of the values read during
   * the oversampling to "Oversample.txt".
   *
   * Example:
   * ```
   * // 12-bit measurement of Pin 2
   * // Leaves nsamples at its default value of 1 (single reading of sensor)
   * alog.analogReadOversample(2, 12);
   * ```
   *
   * Readings that require more bits of precision will take longer.
   *
   * For analog measurements that do not require more than 10 bits of precision,
   * use alog.readpin(int pin) or the standard Arduino "AnalogRead" function.
   *
   * Based on eRCaGuy_NewAnalogRead::takeSamples(uint8_t analogPin)
   *
   * Example:
   * ```
   * // Take a single sample at 14-bit resolution and store it as "myReading"
   * myReading = alog.analogReadOversample(A3, 14, 1);
   * ```
   *
  */

  if(debug){
    start_logging_to_otherfile("Oversample.txt");
  }
  float analog_reading;

  uint8_t n = adc_bits - 10; //"rightshift" value, AKA: "n"
  // Bit shift to calculate 4^n
  // (Best and fastest method to calculate 4 to a power)
  unsigned long oversample_num = 1UL<<(2*n);
  // Bit shift to calculate 2^n
  unsigned int divisor = 1<<n;

  //outer loop: get the number of samples to avg
  unsigned long reading_sum = 0;
  for (unsigned long i=0; i<nsamples; i++)
  {
    //inner loop: do oversampling, per AVR121 Application Note,
    // in order to enhance resolution of 10-bit ADC
    unsigned long inner_sum = 0;
    for (unsigned long j=0; j<oversample_num; j++)
    {
      inner_sum += analogRead(pin); //take a 10-bit reading on the Arduino ADC
      if(debug){
        otherfile.print(analogRead(pin));
        otherfile.print(F(","));
      }
    }
    //Convert these many 10-bit samples to a single higher-resolution sample:
    //Standard Method:
    //unsigned int reading = inner_sum >> n; //See AVR121 Application Note
    //Rounding Method (to nearest integer):
    unsigned long reading = (inner_sum + (unsigned long)divisor/2UL) >> n;
    reading_sum += reading;
  }
  float avg_reading = (float)reading_sum/(float)nsamples;

  // Normalize to 10 bits for all of the stuff here that expects that
  float precision_above_ten = pow(2., adc_bits - 10.);
  analog_reading = avg_reading / precision_above_ten; // 0-1023, but float

  return analog_reading;
}


void ALog::Barometer_BMP180(){

  /**
   * @brief
   * Read absolute pressure in mbar.
   *
   * @details
   * This function reads the absolute pressure in mbar (hPa).  BMP180 sensor
   * incorporates on board temperature correction.  Uses I2C protocol.
   *
   * Example:
   * ```
   * alog.Barometer_BMP180();
   * ```
   *
  */

  SFE_BMP180 pressure;
  char status;
  double P;
  double T;

  if (pressure.begin())
  {
  // A temperature measurement is needed to calibrate a pressure reading.
  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.
  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);  //Wait for the measurement to complete:
    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Function returns 1 if successful, 0 if failure.
    status = pressure.getTemperature(T);
    if (status != 0)
    {
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.
      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);  // Wait for the measurement to complete:
        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Note also that the function requires the previous temperature
        // measurement (T).
        // (If temperature is stable, you can do one temperature measurement
        // for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.
        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          ///////////////
          // SAVE DATA //
          ///////////////

          if (first_log_after_booting_up){
            headerfile.print("Barometric pressure [hPa]");
            headerfile.print(",");
            headerfile.sync();
          }

          // SD write
          //datafile.print(T);
          //datafile.print(F(","));
          datafile.print(P,2);
          datafile.print(F(","));

          // Echo to serial
          //Serial.print(T);
          //Serial.print(F(","));
          Serial.print(P);
          Serial.print(F(","));
          }
          else Serial.println(F("Er retrieve P"));
        }
        else Serial.println(F("Er start P"));
      }
      else Serial.println(F("Er retrieve T"));
    }
    else Serial.println(F("Er start T"));
  }
else Serial.println(F("BMP180 init fail"));

// Note: this is currnetly not printing to header or results if it is not
// working; it is like it is not even there in this case.
}

void ALog::_sensor_function_template(uint8_t pin, float param1, float param2,
           uint8_t ADC_bits, bool flag){
  /**
   * @brief
   * Function to help lay out a new sensor interface.
   * This need not be "void": it may return a value as well.
   *
   * @details
   * Details about sensor go here
   *
   * @param pin You often need to specify interface pins
   *
   * @param param1 A variable for the sensor or to interpret its value
   *
   * @param param2 A variable for the sensor or to interpret its value
   *
   * @param ADC_bits You often need to specify how much the analog-to-digital
   *                 converter should be oversampled; this can range from
   *                 10 (no oversampling) to 16 (maximum possible
   *                 oversampling before certainty in the oversampling method
   *                 drops)
   *
   * @param flag Something that helps to set an option
   *
   * Example (made up):
   * ```
   * alog.Example(A2, 1021.3, 15.2, True);
   * ```
   *
  */

  float Vout_normalized_analog_example = analogReadOversample(pin, \
            ADC_bits) / 1023.;

  float Some_variable = Vout_normalized_analog_example * param1 / param2;
  if (flag){
    Some_variable /= 2.;
  }

  ///////////////
  // SAVE DATA //
  ///////////////

  if (first_log_after_booting_up){
    headerfile.print("Some variable [units]");
    headerfile.print(",");
    headerfile.sync();
  }

  // SD write
  datafile.print(Some_variable);
  datafile.print(F(","));

  // Echo to serial
  Serial.print(Some_variable);
  Serial.print(F(","));

}


void ALog::sleepNow_nap()         // here we put the arduino to sleep between interrupt readings
{
    set_sleep_mode(SLEEP_MODE_STANDBY);   // sleep mode is set here

    cbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter OFF

    sleep_enable();          // enables the sleep bit in the mcucr register
                             // so sleep is possible. just a safety pin
    sleep_mode();            // here the device is actually put to sleep!!
                             // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP

    // After waking, run sleep mode function, and then remainder of this function (below)
    sleep_disable();         // first thing after waking from sleep:
                             // disable sleep...
    // detachInterrupt(1); // crude, but keeps interrupts from clashing. Need to improve this to allow both measurements types!
    // 06-11-2015: The above line commented to allow the rain gage to be read
    // at the same time as other readings
                        // Maybe move this to specific post-wakeup code?
}

// Must be defined outside of Logger class
void _ISR_void(){
}

void _anemometer_count_increment(){
  rotation_count ++;
  detachInterrupt(1);
  delay(20); // debounce
  attachInterrupt(digitalPinToInterrupt(3), _anemometer_count_increment, FALLING);
}

void ALog::HackHD(int control_pin, bool want_camera_on){
  /**
   * @brief
   * HackHD camera control function
   *
   * @details
   * Control the HackHD camera: this function turns the HackHD on or off
   * and records the time stamp from when the HackHD turns on/off in a file
   * called "camera.txt".
   *
   * Because this function turns the camera on or off, you have to ensure
   * that you write a mechanism to keep it on for some time in your code. This
   * could be checking the time each time you wake and deciding what to do,
   * for example. In short: this function is a lower-level utility that
   * requires the end-user to write the rest of the camera control sequence
   * themselves.
   *
   * @param control_pin is the pin connected to the HackHD on/off switch;
   * Dropping control_pin to GND for 200 ms turns camera on or off.
   *
   * @param want_camera_on is true if you want to turn the camera on, false if
   * you want to turn the camera off.
   *
   * \b CAMERA_IS_ON is a global varaible attached to this function that
   * saves the state of the camera; it will be compared to "want_camera_on",
   * such that this function will do nothing if the camera is already on (or
   * off) and you want it on (or off).
   *
   * Power requirements:
   *
   * * 0.2 mA quiescent current draw;
   * * 600 mA while recording
   *
   * Example (not tested):
   *
   * ```
   * // Before "setup":
   * uint32_t t_camera_timeout_start_unixtime;
   * int timeout_secs = 300;
   * book camera_on = false;
   * // ...
   *
   * // Inside "loop":
   * // Turn the camera on after some triggering event, and keep it on for as
   * // long as this condition is met, and for at least 5 minutes afterwards.
   * //
   * // >> some code to measure a variable's "distance"
   * // ...
   * //
   * if (distance < 1500){
   *   alog.HackHD(8, true);
   *   camera_on = true; // Maybe I can get a global variable from this library
   *                     // or have HackHD function return the camera state?
   *   now = RTC.now();
   *   // Reset the timeout clock
   *   t_camera_timeout_start_unixtime = now.unixtime();
   * }
   * else if(camera_on){
   *   now = RTC.now();
   *   // If timed out, turn it off.
   *   if ((t_camera_timeout_start_unixtime - now.unixtime()) > timeout_secs){
   *     alog.HackHD(8, false);
   *     camera_on = false;
   *   }
   * }
   * ```
   *
   * This example could be used to capture flow during a flash flood.
   * See:
   * * Website: http://instaar.colorado.edu/~wickert/atvis/
   * * AGU poster: https://www.researchgate.net/publication/241478936_The_Automatically_Triggered_Video_or_Imaging_Station_ATVIS_An_Inexpensive_Way_to_Catch_Geomorphic_Events_on_Camera
   *
   */

//void ALog::HackHD(int control_pin, int indicator_pin, bool want_camera_on){
  // Drop control_pin to GND to turn camera on or off
  Serial.print(F("C"));
  Serial.print(F("Camera is on"));
  Serial.print(want_camera_on);
  // Turn camera on or off if needed
  if (CAMERA_IS_ON != want_camera_on){
    pinMode(control_pin, OUTPUT);
    digitalWrite(control_pin, LOW);
    delay(200);
    pinMode(control_pin, INPUT);
    digitalWrite(control_pin, HIGH);
    CAMERA_IS_ON = 1 - CAMERA_IS_ON; // flips it from true to false and vice versa
    // Use this to get times of camera on/off
    start_logging_to_otherfile("camera.txt");

    now = RTC.now();

    // SD
    otherfile.print(now.unixtime());
    otherfile.print(",");

    // Echo to serial
    Serial.print(now.unixtime());
    Serial.print(F(","));

    if (want_camera_on == 1){
      otherfile.print("ON");
    }
    else if (want_camera_on == 0){
      otherfile.print("OFF");
    }
    end_logging_to_otherfile();
  }
  // Otherwise, these conditions match and we are in good shape.
}

void ALog::TippingBucketRainGage(){
  /**
   * @brief
   * Tipping-bucket rain gauge
   *
   * @details
   * Uses the interrupt to read a tipping-bucket rain gage.
   * Then prints date stamp
   *
  */

  detachInterrupt(1);

  pinMode(SDpowerPin,OUTPUT); // Seemed to have forgotten between loops... ?
  // might want to use a digitalread for better incorporation into normal logging cycle

  // Callback to set date and time in SD card file metadata
  // Following: https://forum.arduino.cc/index.php?topic=348562.0
  // See: https://github.com/NorthernWidget/Logger/issues/6
  SdFile::dateTimeCallback(_internalDateTime);

  if (!sd.begin(CSpin, SPI_HALF_SPEED)) {
    // Just use Serial.println: don't kill batteries by aborting code
    // on error
    Serial.println(F("Error initializing SD card for writing"));
    LEDwarn(40);
  }

  delay(10);
  start_logging_to_otherfile("bucket_tips.txt");
  now = RTC.now();

  // SD
  otherfile.print(now.unixtime());
  otherfile.print(",");

  // Echo to serial
  Serial.print(now.unixtime());
  Serial.print(F(","));
  end_logging_to_otherfile();

  // START TEMPORARY CODE TO NOTE BUCKET TIP RESPONSE
  pinMode(LEDpin, OUTPUT);
  digitalWrite(LEDpin, HIGH);
  // END TEMPORARY CODE TO NOTE BUCKET TIP RESPONSE
  Serial.println(F("Tip!"));
  delay(50); // to make sure tips aren't double-counted
  // START TEMPORARY CODE TO NOTE BUCKET TIP RESPONSE
  digitalWrite(LEDpin, LOW);
  pinMode(LEDpin, INPUT);
  // END TEMPORARY CODE TO NOTE BUCKET TIP RESPONSE
  NEW_RAIN_BUCKET_TIP = false;

  // Sets flag to log data if the "LOG_ALL_SENSORS_ON_BUCKET_TIP" flag is set
  // "TRUE"
  if (LOG_ALL_SENSORS_ON_BUCKET_TIP){
    IS_LOGGING = true;
  }

  //delay(2000);

  attachInterrupt(digitalPinToInterrupt(3), wakeUpNow_tip, LOW);

  // Then based on whether we are already logging or if we are supposed to
  // start logging here, we can continue with the logging process, or just
  // go back to sleep
  if (_use_sleep_mode){
    // Nested recursion to next-level-up function, hopefully doesn't
    // chew through too much memory!
    if (!IS_LOGGING){
      sleep();
    }
  }
}

void ALog::start_logging_to_datafile(){
  // Open the file for writing
  if (!datafile.open(datafilename, O_WRITE | O_CREAT | O_AT_END)) {
    Serial.print(F("Opening "));
    Serial.print(datafilename);
    Serial.println(F(" for write failed"));
  delay(10);
  }
}

void ALog::start_logging_to_headerfile(){
  // Open the file for writing
  if (!headerfile.open("header.txt", O_WRITE | O_CREAT | O_AT_END)) {
    Serial.print(F("Opening "));
    Serial.print("header.txt");
    Serial.println(F(" for write failed"));
  delay(10);
  }
}

void ALog::start_logging_to_otherfile(char* _filename){
  // open the file for write at end like the Native SD library
  if (!otherfile.open(_filename, O_WRITE | O_CREAT | O_AT_END)) {
    // Just use Serial.println: don't kill batteries by aborting code
    // on error
    Serial.print(F("Opening "));
    Serial.print(_filename);
    Serial.println(F(" for write failed"));
    delay(10);
  }
}

void ALog::end_logging_to_otherfile(){
  // Ends line and closes otherfile
  // Copied from endLine function
  otherfile.println();
  Serial.println();
  // close the file: (This does the actual sync() step too - writes buffer)
  otherfile.close();
  delay(10);
}

void ALog::end_logging_to_headerfile(){
  // Ends line and closes otherfile
  // Copied from endLine function
  headerfile.println();
  // close the file: (This does the actual sync() step too - writes buffer)
  headerfile.close();
  delay(10);
}

void ALog::Decagon5TE(uint8_t excitPin, uint8_t dataPin){
  /**
   * @brief
   * Reads a Decagon Devices 5TE soil moisture probe.
   *
   * @details
   * NEEDS TESTING with current ALog version.
   *
   * Returns Dielectric permittivity [-unitless-], electrical conductivity
   * [dS/m], and temperature [degrees C].
   * Soil moisture is calculated through postprocessing.
   *
   * Uses \b SoftwareSerial, and therefore has the potential to go unstable;
   * however, we have a time limit, so this won't crash the logger: it will
   * just keep the logger from recording good data.
   *
   * Modified from Steve Hicks' code for an LCD reader by Andy Wickert
   *
   * @param excitPin activates the probe and powers it
   *
   * @param dataPin receives incoming serial data at 1200 bps
   *
   * Example:
   * ```
   * alog.Decagon5TE(7, 8);
   * ```
   *
   */

  SoftwareSerial mySerial(excitPin, dataPin);  //5tm's red wire (serial data out) connected to pin 5, pin 6 goes nowhere
  int Epsilon_Raw, Sigma_Raw, T_Raw;   //temporary integer variables to store the 3 parts of the incoming serial stream from the 5TM
  char dataStream[14];   // Max 14 characters: 4x3 + 2 spaces
  int startflag=1;
  int endflag=0;
  int i=0;
  unsigned int startMillis; // same comment as right below
  unsigned int elapsed = 0; // shouldn't overflow on the time scales I'm using

  if(startflag){
    digitalWrite(excitPin,HIGH);
    delay(100); // max. time for probe's comms to start up (see datasheet)
    startMillis = millis();
    startflag=0;
    Serial.println(startMillis);
  }

  // Start serial port
  // Using standard Decagon DDI Serial; simpler than SDI-12
  mySerial.begin(1200); // 1200 bits per second

  // OK if it takes longer, so long as data stream is continuous
  // so we don't break out of inner while loop, and we start
  // receiving before 200 ms is up
  while (elapsed < 200){
    elapsed = millis() - startMillis;
    Serial.print(F("ms elapsed = "));
    Serial.println(elapsed);
    //  code keeps looping until incoming serial data appears on the mySerial pin
    while (mySerial.available()) {
      //Serial.println("Getting data:");
        delay(1);
      if (mySerial.available() >0) {
    		endflag=1;
        char c = mySerial.read();  //gets one byte from serial buffer
          Serial.println(c);
        if((c>='0' and c<='9') || c==' ') {
         dataStream[i] = c; //makes the string readString
         i++;
        }
      }
    }
  }

  if(endflag==1){

    digitalWrite(excitPin,LOW);
    Serial.println(dataStream);
    endflag=0;

    // Declare to make C++ happy
    float Epsilon_a;
    float EC;
    float T;

    // parse the array into 3 integers  (for the 5TM, y is always 0)
    sscanf (dataStream, "%d %d %d", &Epsilon_Raw, &Sigma_Raw, &T_Raw);

    // Change measured values into real values, via equations in Decagon 5TE
    // manual

    // Dielectric permittivity [-unitless-]
    if (Epsilon_Raw == 4095){
      // Error alert!
      //delete Epsilon_a;
      //char Epsilon_a[6] = "ERROR";
      Epsilon_a = -9999;
    }
    else {
      Epsilon_a = Epsilon_Raw/50.;
    }
    // Electrical Conductivity [dS/m]
    if (Sigma_Raw == 1023){
      // Error alert!
      //char EC[6] = "ERROR";
      EC = -9999;
    }
    else if (Sigma_Raw <= 700){
      EC = Sigma_Raw/100.;
    }
    else {
      // (i.e. Sigma_Raw > 700, but no elif needed so long as input string
      // parses correctly... hmm, should maybe protect against that)
      EC = (700. + 5.*(Sigma_Raw- 700.))/100.;
    }
    // Temperature [degrees C]
    // Combined both steps of the operation as given in the manual
    if (T_Raw == 1023){
      // Error alert!
      // char T[6] = "ERROR";
      T = -9999;
    }
    else if (T_Raw <= 900){
      T = (T_Raw - 400.) / 10.;
    }
    else {
      // (i.e. T_Raw > 900, but no elif needed so long as input string
      // parses correctly... hmm, should maybe protect against that)
      T = ((900. + 5.*(T_Raw-900.) - 400.)) / 10.;
    }

    ///////////////
    // SAVE DATA //
    ///////////////

    if (first_log_after_booting_up){
      headerfile.print("Dielectric permittivity [-]");
      headerfile.print(",");
      headerfile.print("Electrical Conductivity [dS/m]");
      headerfile.print(",");
      headerfile.print("Temperature [degrees C]");
      headerfile.print(",");
      headerfile.sync();
    }

    // SD write
    datafile.print(Epsilon_a);
    datafile.print(F(","));
    datafile.print(EC);
    datafile.print(F(","));
    datafile.print(T);
    datafile.print(F(","));

    // Echo to serial
    Serial.print(Epsilon_a);
    Serial.print(F(","));
    Serial.print(EC);
    Serial.print(F(","));
    Serial.print(T);
    Serial.print(F(","));
  }
}

void ALog::DecagonGS1(uint8_t pin, float Vref, uint8_t ADC_resolution_nbits){
  /**
   * @brief Ruggedized Decagon Devices soil moisture sensor
   *
   * @param pin Analog pin number
   *
   * @param Vref is the reference voltage of the ADC; on the ALog, this is
   * a precision 3.3V regulator (unless a special unit without this regulator
   * is ordered; the regulator uses significant power)
   *
   * @param ADC_resolution_nbits (10-16 for the ALog BottleLogger) is the
   * number of bits of ADC resolution used (oversampling for >10 bits)
   *
   * @details
   * Example:
   * ```
   * // Using a non-precision Rref that is slightly off
   * alog.DecagonGS1(A1, 3.27, 14);
   * ```
   *
   */

  // Vref in volts
  float _ADC;
  float voltage;
  float volumetric_water_content;
  _ADC = analogReadOversample(pin, ADC_resolution_nbits);
  voltage = Vref * _ADC / 1023.;

  // Standard Decagon equation -- linear, for up to 60% VWC
  // Decagon sensor returns the same value of voltage regardless of its
  // main power voltage (3 to 15 V allowed)
  volumetric_water_content = 0.494 * voltage - 0.554;

  ///////////////
  // SAVE DATA //
  ///////////////

  if (first_log_after_booting_up){
    headerfile.print("Voltage [V]");
    headerfile.print(",");
    headerfile.print("Volumentric water content [-]");
    headerfile.print(",");
    headerfile.sync();
  }

  // SD write
  datafile.print(voltage, 4);
  datafile.print(F(","));
  datafile.print(volumetric_water_content, 4);
  datafile.print(F(","));

  // Echo to serial
  Serial.print(voltage, 4);
  Serial.print(F(","));
  Serial.print(volumetric_water_content, 4);
  Serial.print(F(","));

}

// Honeywell_HSC_analog
//////////////////////////////

float ALog::Honeywell_HSC_analog(int pin, float Vsupply, float Vref, \
                                   float Pmin, float Pmax, \
                                   int TransferFunction_number, int units, \
                                   uint8_t ADC_resolution_nbits){
  /**
   * @brief
   * Cost-effective pressure sensor from Honeywell
   *
   * @details
   * Datasheet: http://sensing.honeywell.com/index.php?ci_id=151133
   *
   * See also the \b Honeywell_HSC_analog example.
   *
   * @param pin Analog pin number
   *
   * @param Vsupply Supply voltage to sensor
   *
   * @param Vref is the reference voltage of the ADC; on the ALog, this is
   * a precision 3.3V regulator (unless a special unit without this regulator
   * is ordered; the regulator uses significant power)
   *
   * @param Pmin Minimum pressure in range of sensor
   *
   * @param Pmax Maximum pressure in range of sensor
   *
   * @param Pmax Maximum pressure in range of sensor
   *
   * @param TransferFunction_number: 1, 2, 3, or 4: which transfer function is
   * used to convert voltage to pressure
   * * TransferFunction: 1 = 10% to 90% of Vsupply
   *   ("A" in second to last digit of part number)
   * * TransferFunction: 2 = 5% to 95% of Vsupply
   *   ("A" in second to last digit of part number)
   * * TransferFunction: 3 = 5% to 85% of Vsupply
   *   ("A" in second to last digit of part number)
   * * TransferFunction: 4 = 4% to 94% of Vsupply
   *   ("A" in second to last digit of part number)
   *
   * @param units: Output units
   * * Units: 0 = mbar
   * * Units: 1 = bar
   * * Units: 2 = Pa
   * * Units: 3 = KPa
   * * Units: 4 = MPa
   * * Units: 5 = inH2O
   * * Units: 6 = PSI
   *
   * @param ADC_resolution_nbits (10-16 for the ALog BottleLogger) is the
   * number of bits of ADC resolution used (oversampling for >10 bits)
   *
   * Example:
   * ```
   * alog.Honeywell_HSC_analog(A1, 5, 3.3, 0, 30, 1, 6);
   * ```
   *
   */

  //

  // Read pin voltage
  float reading = analogReadOversample(pin, ADC_resolution_nbits);
  float Vout = reading/1023*Vref;

  // Apply transfer function
  float P;

  if(TransferFunction_number == 1){
  P = (Vout - 0.1*Vsupply) * ((Pmax-Pmin)/(0.8*Vsupply)) + Pmin;
  }
  if(TransferFunction_number == 2){
  P = (Vout - 0.05*Vsupply) * ((Pmax-Pmin)/(0.9*Vsupply)) + Pmin;
  }
  if(TransferFunction_number == 3){
  P = (Vout - 0.05*Vsupply) * ((Pmax-Pmin)/(0.8*Vsupply)) + Pmin;
  }
  if(TransferFunction_number == 4){
  P = (Vout - 0.04*Vsupply) * ((Pmax-Pmin)/(0.9*Vsupply)) + Pmin;
  }

  char* _units[]={"mbar", "bar", "Pa", "KPa", "Mpa", "inH2O", "PSI", "why"};

  ///////////////
  // SAVE DATA //
  ///////////////

  if (first_log_after_booting_up){
    headerfile.print("Pressure [");
    headerfile.print(_units[units]);
    headerfile.print("]");
    headerfile.print(",");
    headerfile.sync();
  }

  // SD write
  datafile.print(P, 4);
  //datafile.print(F(" "));
  //datafile.print(_units[units]);
  datafile.print(F(","));

  // Echo to serial
  Serial.print(P, 4);
  //Serial.print(F(" "));
  //Serial.print(_units[units]);
  Serial.print(F(","));

  return P;

}

void ALog::vdivR(uint8_t pin, float Rref, uint8_t ADC_resolution_nbits, \
           bool Rref_on_GND_side){
  /**
   * @brief
   * Resistance from a simple voltage divider
   *
   * @param pin Analog pin number
   *
   * @param Rref Resistance value of reference resistor [ohms]
   *
   * @param ADC_resolution_nbits (10-16 for the ALog BottleLogger) is the
   * number of bits of ADC resolution used (oversampling for >10 bits)
   *
   * @param Rref_on_GND_side indicates the configuration of the voltage divider.
   * True if using Alog provided Reference resistor terminals. If false,
   * the reference resitor must be instead connected via the screw terminals.
   * This is set true for external sensors that are built to require a
   * VCC-side reference resistor.
   *
   * @details
   * Example:
   * ```
   * // Use standard reference resistor headers: let last parameter be false
   * // (default)
   * alog.vdivR(A2, 10000, 12);
   * ```
   */

  float _R = _vdivR(pin, Rref, ADC_resolution_nbits, Rref_on_GND_side);

  ///////////////
  // SAVE DATA //
  ///////////////

  if (first_log_after_booting_up){
    headerfile.print("Resistance [Ohms]");
    headerfile.print(",");
    headerfile.sync();
  }

  // SD write
  datafile.print(_R);
  datafile.print(F(","));

  // Echo to serial
  Serial.print(_R);
  Serial.print(F(","));

}

/*
SENSOR DOES NOT STABILIZE (FLEXFORCE SENSOR)
void ALog::flex(int flexPin, float Rref, float calib1, float calib2){
  float _Rflex = _vdivR(flexPin, Rref);
  // FINISH WRITING CODE
}
*/

void ALog::linearPotentiometer(uint8_t linpotPin, float Rref, float slope, \
           char* _distance_units, float intercept, \
           uint8_t ADC_resolution_nbits, bool Rref_on_GND_side){
  /**
   * @brief
   * Linear potentiometer (radio tuner) to measure distance
   *
   * @details
   * Distance based on resistance in a sliding potentiometer whose resistance
   * may be described as a linear function
   *
   * @param linpotPin Analog pin number
   *
   * @param Rref Resistance value of reference resistor [ohms]
   *
   * @param slope Slope of the line (distance = (slope)R + R0)
   *
   * @param intercept (R0) of the line (distance = (slope)R + R0)
   *
   * @param ADC_resolution_nbits (10-16 for the ALog BottleLogger) is the
   * number of bits of ADC resolution used (oversampling for >10 bits)
   *
   * @param _distance_units is the name of the units of distance that are used
   * in the linear calibration equation, and are therefore the units of this
   * function's output.
   *
   * @param Rref_on_GND_side indicates the configuration of the voltage divider.
   * True if using Alog provided Reference resistor terminals. If false,
   * the reference resitor must be instead connected via the screw terminals.
   * This is set true for external sensors that are built to require a
   * VCC-side reference resistor.
   *
   * The output units will be whatever you have used to create your linear
   * calibration equation
   *
   * Example:
   * ```
   * // Using a 0-10k ohm radio tuner with units in mm and a perfect intercept;
   * // maintaining default 14-bit readings with standard-side (ALog header)
   * // reference resistor set-up
   * alog.linearPotentiometer(A0, 5000, 0.0008);
   * ```
   *
   */

  float _Rpot = _vdivR(linpotPin, Rref, ADC_resolution_nbits, Rref_on_GND_side);
  float _dist = slope*_Rpot + intercept;

  ///////////////
  // SAVE DATA //
  ///////////////

  if (first_log_after_booting_up){
    headerfile.print("Distance [");
    headerfile.print(_distance_units);
    headerfile.print("]");
    headerfile.print(",");
    headerfile.sync();
  }

  // SD write
  datafile.print(_dist);
  datafile.print(F(","));

  // Echo to serial
  Serial.print(_dist);
  Serial.print(F(","));

}

void save_Aref(float _V){
  /**
   * @brief
   * Saves a float as the reference voltage for the ADC ("Vref") to the EEPROM
   *
   * @details
   * ADC == analog-digital comparator
   * EEPROM = permanent memory (persists after shutdown)
   * See: https://www.arduino.cc/en/Reference/EEPROMPut
   *
   * This function is only called rarely, as this value is typically measured
   * only once.
   *
   * @param _V reference voltage, ideally measured under load [V]
   *
   * Example:
   * ```
   * // Measuring 3.297V with a calibrated multimeter between 3V3 and GND
   * // Then:
   * alog.saveAref(3.297);
   * ```
   *
   */
   EEPROM.put(4, _V);
}

float read_Aref(){
  /**
   * @brief
   * Read the analog (ADC) sensor reference voltage from EEPROM, return float.
   *
   * @details
   * ADC == analog-digital comparator
   * EEPROM = permanent memory (persists after shutdown)
   * See: https://www.arduino.cc/en/Reference/EEPROMGet
   *
   * Example:
   * ```
   * float Vref;
   * Vref = alog.readAref();
   * ```
   *
   */
  float _V;
  EEPROM.get(4, _V);
  return _V;
}

// NEW STUFF: (MAINLY) INTERNAL FUNCTIONS

void ALog::name(){
  // Self-identify before talking
  Serial.print(F("<"));
  Serial.print(logger_name);
  Serial.print(F(">: "));
}


void ALog::print_time(){
  boolean exit_flag = 1;
  // Wait for computer to tell logger to start sending its time
  char go;
  while( exit_flag ){
    go = Serial.read();
    if( (go == 'g') && exit_flag ){
      exit_flag = 0; // Exit loop once this is done
      // First, wait until we are at the top of the second
      now = RTC.now();
      unsigned long unixtimenow = now.unixtime();
      while (now.unixtime() == unixtimenow){
        now = RTC.now();
      }
      // Print times before setting clock
      for (int i=0; i<5; i++){
        now = RTC.now();
        Serial.println(now.unixtime());
        if ( i<4 ){
          // No need to delay on the last time through
          delay(1000);
        }
      }
    }
  } // This is the end of the while loop for clock setting and passing the old
    // time over to the computer.
}

void ALog::set_time_main(){
  // Now set clock and returns 5 more times as part of that function
  // First thing coming in should be the time from the computer
  boolean exit_flag = 1;
  while( exit_flag ){
    if ( Serial.available() ){
      clockSet();
      exit_flag = 0; // Totally out of loop now
    }
  }
}

void ALog::announce_start(){
  Serial.println();
  name();
  Serial.println(F(" = this logger's name."));
  Serial.println();
  delay(100);
  Serial.println(\
    F("********************** Logger initializing. **********************"));
}

// Handshake function
void ALog::establishContact_Tx(){
  while (Serial.available() <= 0) {
    Serial.print('A');   // send a capital A
    delay(300);
  }
}

bool ALog::establishContact_Rx(){
  char in;
  bool setclock = false;
  in = Serial.read();   // send a capital A
  if(in == 'A'){
    Serial.println(F("ALog"));
    setclock = true;
  }
  return setclock;
}

void ALog::startup_sequence(){
  // Turn on power to all sensors: important for I2C sensors that may need
  // power to not pull on clock
  pinMode(SensorPowerPin, OUTPUT);
  digitalWrite(SensorPowerPin, HIGH);

  bool connected_to_computer = false;
  //char handshake[4];
  //char handshake_test[5] = "alog"; // 5 chars, incl. termination
  //int ntrue = 0;
  int i;
  unsigned long unixtime_at_start;

  // First, throw away any garbage on the incoming Serial line
  while(Serial.available() > 0){
    Serial.read();
  }

  // Then check if connected to computer with ALogTalk running to set the clock
  // Do so by first pinging the computer, and then waiting for a handshake.
  int millisthen = millis();
  while ( (millis() - millisthen) < 2000 && (connected_to_computer == 0)){
    connected_to_computer = establishContact_Rx();
    // Serial is available if establishContact exits on its own
    /*
    if ( Serial.available() ){
      i = 0;
      while( Serial.available() && (i < 4)){
        handshake[i] = Serial.read();
        i++;
      }
	    for (int i=0; i<4; i++){
        if (handshake[i] == handshake_test[i]){
          ntrue++;
        }
      }
      if (ntrue == 4){
        connected_to_computer = true;
      }
    }
    */
  }

  // Run through startup sequence, including clock setting if
  // connected_to_computer is true
  name();
  Serial.println(F("HELLO, COMPUTER."));
  delay(50);
  //if ( Serial.available() ){ // To allow clock setting, uncomment this and
  // comment the above section that sets "connected_to_computer"...
  // that is, unless you can make the handshake work!
  if ( connected_to_computer ){
    wdt_disable();
    delay(1000); // Give Python time to print
    name();
    Serial.print(F("LOGGING TO FILE ["));
    Serial.print(datafilename);
    Serial.println(F("]"));
    delay(1500);
    name();
    Serial.print(F("UNIX TIME STAMP ON MY WATCH IS: "));
    now = RTC.now();
    unixtime_at_start = now.unixtime();
    Serial.println(unixtime_at_start);
    delay(1500);
    if(unixtime_at_start < 1000000000){
      name();
      Serial.println(F("Uh-oh: that doesn't sound right! Clock reset to 1/1/2000?"));
      LEDtimeWrong(3);
      print_time();
      set_time_main();
      delay(2000);
      name();
      Serial.println(F("Thanks, computer! I think I'm all set now."));
    }
    else{
      name();
      //Serial.println(F("Clock is probably fine"));
      ///*
      Serial.println(F("How does that compare to you, computer?"));
      delay(1500);
      print_time();
      delay(1500);
      name();
      Serial.println(F("Would you like to set the logger's clock \
                        to the computer's time? (y/n)"));
      boolean waiting = 1;
      char yn;
      while ( waiting ){
        if ( Serial.available() ){
          yn = Serial.read();
          if ( yn == 'y' || yn == 'Y'){
            waiting = 0;
            set_time_main();
          }
          else if ( yn == 'n' || yn == 'N'){
            waiting = 0;
            name();
            Serial.println(F("Not selecting time; continuing."));
            delay(1500);
          }
          else{
            name();
            Serial.println(F("Please select <y> or <n>."));
          }
        }
      }
    }
  }
  else{
    // No serial; just blink
    now = RTC.now();
    unixtime_at_start = now.unixtime();
    // Keep Serial just in case computer is connected w/out Python terminal
    Serial.print(F("Current UNIX time stamp according to logger is: "));
    Serial.println(unixtime_at_start);

    if(unixtime_at_start < 1000000000){
      LEDtimeWrong(3);
    }
  }
  if ( connected_to_computer ){
    delay(1500);
    name();
    Serial.println(F("Now beginning to log."));
    delay(1000);
  }

  digitalWrite(SensorPowerPin, LOW);
}

void ALog::clockSet(){

  byte Year;
  byte Month;
  byte Date;
  byte DoW;
  byte Hour;
  byte Minute;
  byte Second;

  DateTime nowPreSet = RTC.now();

	GetDateStuff(Year, Month, Date, DoW, Hour, Minute, Second);

	Clock.setClockMode(false);	// set to 24h
	//setClockMode(true);	// set to 12h

	Clock.setYear(Year);
	Clock.setMonth(Month);
	Clock.setDate(Date);
	Clock.setDoW(DoW);
	Clock.setHour(Hour);
	Clock.setMinute(Minute);
	Clock.setSecond(Second);

	// Give time at next five seconds
	// Should use a DateTime object for this, b/c rollover is a potential
	// problem, but this display is not mission-critical
	for (int i=0; i<5; i++){
	    delay(1000);
	    now = RTC.now();
	    Serial.print(now.year(), DEC);
	    Serial.print(F("-"));
	    Serial.print(now.month(), DEC);
	    Serial.print(F("-"));
	    Serial.print(now.day(), DEC);
	    Serial.print(F(" "));
	    Serial.print(now.hour(), DEC); //24-hr (should be)
	    Serial.print(F(":"));
	    Serial.print(now.minute(), DEC);
	    Serial.print(F(":"));
	    Serial.println(now.second(), DEC);

	}
  delay(1000);
  unsigned long unixtime_at_receive_string = nowPreSet.unixtime();
  Serial.print(F("Logger's UNIX time at which it received the new time string: "));
  Serial.println(unixtime_at_receive_string);
  Serial.println(F("Clock set!"));
}


void ALog::GetDateStuff(byte& Year, byte& Month, byte& Day, byte& DoW,
		       byte& Hour, byte& Minute, byte& Second) {
	// Call this if you notice something coming in on
	// the serial port. The stuff coming in should be in
	// the order YYMMDDwHHMMSS, with an 'x' at the end.
	boolean GotString = false;
	char InChar;
	byte Temp1, Temp2;
	char InString[20];

	byte j=0;
	while (!GotString) {
		if (Serial.available()) {
			InChar = Serial.read();
			InString[j] = InChar;
			j += 1;
			if (InChar == 'x') {
				GotString = true;
			}
		}
	}
	Serial.println(InString);
	// Read Year first
	Temp1 = (byte)InString[0] -48;
	Temp2 = (byte)InString[1] -48;
	Year = Temp1*10 + Temp2;
	// now month
	Temp1 = (byte)InString[2] -48;
	Temp2 = (byte)InString[3] -48;
	Month = Temp1*10 + Temp2;
	// now date
	Temp1 = (byte)InString[4] -48;
	Temp2 = (byte)InString[5] -48;
	Day = Temp1*10 + Temp2;
	// now Day of Week
	DoW = (byte)InString[6] - 48;
	// now Hour
	Temp1 = (byte)InString[7] -48;
	Temp2 = (byte)InString[8] -48;
	Hour = Temp1*10 + Temp2;
	// now Minute
	Temp1 = (byte)InString[9] -48;
	Temp2 = (byte)InString[10] -48;
	Minute = Temp1*10 + Temp2;
	// now Second
	Temp1 = (byte)InString[11] -48;
	Temp2 = (byte)InString[12] -48;
	Second = Temp1*10 + Temp2;
}

////////////////////////
// Non-class function //
////////////////////////

// call back for file timestamps
// See https://forum.arduino.cc/index.php?topic=72739.0
// and https://forum.arduino.cc/index.php?topic=348562.0
void _internalDateTime(uint16_t* date, uint16_t* time) {
  DateTime now = RTC.now();
  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(now.year(), now.month(), now.day());
  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(now.hour(), now.minute(), now.second());
}
