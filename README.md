ALog
======

"ALog" holds the "Logger" data logger library for the Arduino-based ALog (http://northernwidget.com/alog/) developed by Andy Wickert and Chad Sandell at Northern Widget LLC and the University of Minnesota.

While it is developed to work specifically with the ALog, it will also work with a standard Arduino that is outfitted with a SD card and a DS3231 real-time clock.

"Logger" is optimized to handle all of the basic file, system, and power management behind-the-scenes, and to reduce power consumption to minimal levels through the use of the sleep functions. It exposes sensor functions as single-line calls, and includes a template for the addition of new sensors.

For questions related to the Logger library, please send a message to us at info@northernwidget.com.


# Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`define `[`cbi`](#Logger_8cpp_1ae70baf5399951da1e7ad45a0ed890832)            | 
`define `[`sbi`](#Logger_8cpp_1ac4a5536d9bf092116f88b94797ddc882)            | 
`public void `[`wakeUpNow`](#Logger_8cpp_1adee29828901ea1b4f99ce305fa3e17bd)`()`            | 
`public void `[`wakeUpNow_tip`](#Logger_8cpp_1a1a3b380e75d68eef4c3913816773e1b5)`()`            | 
`public void `[`_ISR_void`](#Logger_8cpp_1a9c215a00c214c880d04481d256d8d522)`()`            | 
`public void `[`_anemometer_count_increment`](#Logger_8cpp_1aa2e4ccb5b638347db4da535aac7ac209)`()`            | 
`public void `[`save_Aref`](#Logger_8cpp_1a144189fccfe80979cc2fcafefdf2728a)`(float _V)`            | 
`public float `[`read_Aref`](#Logger_8cpp_1ae9bf4d0d2809a902be80dbf825844636)`()`            | 
`public void `[`_internalDateTime`](#Logger_8cpp_1a7551515cf6f018df3d8d942710936aef)`(uint16_t * date,uint16_t * time)`            | 
`public void `[`wakeUpNow`](#Logger_8h_1adee29828901ea1b4f99ce305fa3e17bd)`()`            | 
`public void `[`wakeUpNow_tip`](#Logger_8h_1a1a3b380e75d68eef4c3913816773e1b5)`()`            | 
`public void `[`_ISR_void`](#Logger_8h_1a9c215a00c214c880d04481d256d8d522)`()`            | 
`public void `[`_anemometer_count_increment`](#Logger_8h_1aa2e4ccb5b638347db4da535aac7ac209)`()`            | 
`public void `[`_internalDateTime`](#Logger_8h_1a7551515cf6f018df3d8d942710936aef)`(uint16_t * date,uint16_t * time)`            | 
`class `[`Logger`](#classLogger) | 

## Members

#### `define `[`cbi`](#Logger_8cpp_1ae70baf5399951da1e7ad45a0ed890832) 

#### `define `[`sbi`](#Logger_8cpp_1ac4a5536d9bf092116f88b94797ddc882) 

#### `public void `[`wakeUpNow`](#Logger_8cpp_1adee29828901ea1b4f99ce305fa3e17bd)`()` 

#### `public void `[`wakeUpNow_tip`](#Logger_8cpp_1a1a3b380e75d68eef4c3913816773e1b5)`()` 

#### `public void `[`_ISR_void`](#Logger_8cpp_1a9c215a00c214c880d04481d256d8d522)`()` 

#### `public void `[`_anemometer_count_increment`](#Logger_8cpp_1aa2e4ccb5b638347db4da535aac7ac209)`()` 

#### `public void `[`save_Aref`](#Logger_8cpp_1a144189fccfe80979cc2fcafefdf2728a)`(float _V)` 

#### `public float `[`read_Aref`](#Logger_8cpp_1ae9bf4d0d2809a902be80dbf825844636)`()` 

#### `public void `[`_internalDateTime`](#Logger_8cpp_1a7551515cf6f018df3d8d942710936aef)`(uint16_t * date,uint16_t * time)` 

#### `public void `[`wakeUpNow`](#Logger_8h_1adee29828901ea1b4f99ce305fa3e17bd)`()` 

#### `public void `[`wakeUpNow_tip`](#Logger_8h_1a1a3b380e75d68eef4c3913816773e1b5)`()` 

#### `public void `[`_ISR_void`](#Logger_8h_1a9c215a00c214c880d04481d256d8d522)`()` 

#### `public void `[`_anemometer_count_increment`](#Logger_8h_1aa2e4ccb5b638347db4da535aac7ac209)`()` 

#### `public void `[`_internalDateTime`](#Logger_8h_1a7551515cf6f018df3d8d942710936aef)`(uint16_t * date,uint16_t * time)` 

# class `Logger` 

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public  `[`Logger`](#classLogger_1abc41bfb031d896170c7675fa96a6b30c)`()` | [Logger](#classLogger) library for the Arduino-based ALog data logger.
`public void `[`initialize`](#classLogger_1a635c5dc0046646bec7023ef7133f0eb3)`(char * _logger_name,char * _datafilename,int _hourInterval,int _minInterval,int _secInterval,bool _ext_int,bool _LOG_ON_BUCKET_TIP)` | 
`public void `[`setupLogger`](#classLogger_1ab5e0bd543758c65a17b77553a0e9f0c9)`()` | 
`public void `[`sleep`](#classLogger_1ad90ff8f29410f6b70cc6334391400a4e)`()` | 
`public void `[`goToSleep_if_needed`](#classLogger_1ad28cf6450ada04f0e1475998bede5b88)`()` | 
`public void `[`startLogging`](#classLogger_1a4a6c78dd1715b33ae4bbd6f66f116f77)`()` | 
`public void `[`endLogging`](#classLogger_1aa82814d61687debcf3b8dd6f46c9d549)`()` | 
`public void `[`startAnalog`](#classLogger_1af936c7f58e23316abb5614cbd31c7ced)`()` | 
`public void `[`endAnalog`](#classLogger_1adca7be8a63592263c67f63766680d16f)`()` | 
`public bool `[`get_use_sleep_mode`](#classLogger_1acc758b6fdaac8099c492929aa7f1691d)`()` | 
`public float `[`readPin`](#classLogger_1a343fcabefb37e06429865a2e6a6e708a)`(int pin)` | 
`public float `[`readPinOversample`](#classLogger_1a4e67526c65fa865f276a515a200af4aa)`(int pin,int bits)` | 
`public float `[`analogReadOversample`](#classLogger_1ad8296890a14a0df83c2433a20f25b899)`(int pin,uint8_t adc_bits,int nsamples,bool debug)` | 
`public float `[`thermistorB`](#classLogger_1a55d923b98a6c503fccb25bfd4af32f3d)`(float R0,float B,float Rref,float T0degC,int thermPin,uint8_t ADC_resolution_nbits,bool Rref_on_GND_side,bool oversample_debug,bool record_results)` | 
`public void `[`ultrasonicMB_analog_1cm`](#classLogger_1a362a1462166d63ddc613eaa1e86f9854)`(int nping,int EX,int sonicPin,bool writeAll)` | 
`public float `[`maxbotixHRXL_WR_Serial`](#classLogger_1a87ce56cb9c3dfc7abfd6308b2ee7dc10)`(int Ex,int nping,bool writeAll,int maxRange,bool RS232)` | 
`public void `[`maxbotixHRXL_WR_analog`](#classLogger_1a3ce2869bbd48fdebbf44e155981c85b0)`(int nping,int sonicPin,int EX,bool writeAll,uint8_t ADC_resolution_nbits)` | 
`public void `[`Decagon5TE`](#classLogger_1a40ae372dee7f672a6d6f33ab441e4da1)`(int excitPin,int dataPin)` | 
`public void `[`DecagonGS1`](#classLogger_1a84da6a9ec3d4d56fdc32d950b71f1a26)`(int pin,float Vref,uint8_t ADC_resolution_nbits)` | 
`public void `[`vdivR`](#classLogger_1ab1ae31b2bdb77c86fb6851907258171b)`(int pin,float Rref,uint8_t ADC_resolution_nbits,bool Rref_on_GND_side)` | 
`public void `[`linearPotentiometer`](#classLogger_1a95670d06ec3b68300895cd7bf8c37999)`(int linpotPin,float Rref,float slope,char * _distance_units,float intercept,uint8_t ADC_resolution_nbits,bool Rref_on_GND_side)` | 
`public void `[`HTM2500LF_humidity_temperature`](#classLogger_1a4ccff7a14a6bddc8bb28e22b3b36d3cc)`(int humidPin,int thermPin,float Rref_therm,uint8_t ADC_resolution_nbits)` | 
`public void `[`HM1500LF_humidity_with_external_temperature`](#classLogger_1a62b74ddb3cf9fdd7dae2394c1b245ed4)`(int humidPin,float R0_therm,float B_therm,float Rref_therm,float T0degC_therm,int thermPin_therm,uint8_t ADC_resolution_nbits)` | 
`public void `[`Inclinometer_SCA100T_D02_analog_Tcorr`](#classLogger_1a80fdea5a339573980f9544ac64678411)`(int xPin,int yPin,float Vref,float Vsupply,float R0_therm,float B_therm,float Rref_therm,float T0degC_therm,int thermPin_therm,uint8_t ADC_resolution_nbits)` | 
`public void `[`Anemometer_reed_switch`](#classLogger_1a6c6a43a1b86f88c2a5e33d14c992e510)`(int interrupt_pin_number,unsigned long reading_duration_milliseconds,float meters_per_second_per_rotation)` | 
`public void `[`Wind_Vane_Inspeed`](#classLogger_1a31c3cba5ff5722fb66bf540bfbe8b25d)`(int vanePin)` | 
`public void `[`Pyranometer`](#classLogger_1ae4190ce7ccfd7b148a6151102a3bf93d)`(int analogPin,float raw_mV_per_W_per_m2,float gain,float V_ref,uint8_t ADC_resolution_nbits)` | 
`public void `[`Barometer_BMP180`](#classLogger_1a40588117a274f0c6de926ef6ce1f0ba8)`()` | 
`public void `[`_sensor_function_template`](#classLogger_1a98f3cc370c87d1e1eaf3eb6a7e0f1623)`(int pin,float param1,float param2,int ADC_bits,bool flag)` | 
`public void `[`HackHD`](#classLogger_1a923b296832bd4222da649ebc66427ac1)`(int control_pin,bool want_camera_on)` | 
`public float `[`Honeywell_HSC_analog`](#classLogger_1a9808967fdf91f10602aa883df35145b3)`(int pin,float Vsupply,float Vref,float Pmin,float Pmax,int TransferFunction_number,int units,uint8_t ADC_resolution_nbits)` | 

## Members

#### `public  `[`Logger`](#classLogger_1abc41bfb031d896170c7675fa96a6b30c)`()` 

[Logger](#classLogger) library for the Arduino-based ALog data logger.

ALog data logger library: methods to:

* Initialize the data logger

* Sleep and wake

* Interact with the real-time clock (RTC)

* Write data to the SD card

* Manage power

* Interact with a range of sensors

All help documentation here assumes you have created an instance of the "Logger" 
```cpp
logger Logger();
```

#### `public void `[`initialize`](#classLogger_1a635c5dc0046646bec7023ef7133f0eb3)`(char * _logger_name,char * _datafilename,int _hourInterval,int _minInterval,int _secInterval,bool _ext_int,bool _LOG_ON_BUCKET_TIP)` 

#### `public void `[`setupLogger`](#classLogger_1ab5e0bd543758c65a17b77553a0e9f0c9)`()` 

#### `public void `[`sleep`](#classLogger_1ad90ff8f29410f6b70cc6334391400a4e)`()` 

#### `public void `[`goToSleep_if_needed`](#classLogger_1ad28cf6450ada04f0e1475998bede5b88)`()` 

#### `public void `[`startLogging`](#classLogger_1a4a6c78dd1715b33ae4bbd6f66f116f77)`()` 

#### `public void `[`endLogging`](#classLogger_1aa82814d61687debcf3b8dd6f46c9d549)`()` 

#### `public void `[`startAnalog`](#classLogger_1af936c7f58e23316abb5614cbd31c7ced)`()` 

#### `public void `[`endAnalog`](#classLogger_1adca7be8a63592263c67f63766680d16f)`()` 

#### `public bool `[`get_use_sleep_mode`](#classLogger_1acc758b6fdaac8099c492929aa7f1691d)`()` 

#### `public float `[`readPin`](#classLogger_1a343fcabefb37e06429865a2e6a6e708a)`(int pin)` 

#### `public float `[`readPinOversample`](#classLogger_1a4e67526c65fa865f276a515a200af4aa)`(int pin,int bits)` 

#### `public float `[`analogReadOversample`](#classLogger_1ad8296890a14a0df83c2433a20f25b899)`(int pin,uint8_t adc_bits,int nsamples,bool debug)` 

#### `public float `[`thermistorB`](#classLogger_1a55d923b98a6c503fccb25bfd4af32f3d)`(float R0,float B,float Rref,float T0degC,int thermPin,uint8_t ADC_resolution_nbits,bool Rref_on_GND_side,bool oversample_debug,bool record_results)` 

#### `public void `[`ultrasonicMB_analog_1cm`](#classLogger_1a362a1462166d63ddc613eaa1e86f9854)`(int nping,int EX,int sonicPin,bool writeAll)` 

#### `public float `[`maxbotixHRXL_WR_Serial`](#classLogger_1a87ce56cb9c3dfc7abfd6308b2ee7dc10)`(int Ex,int nping,bool writeAll,int maxRange,bool RS232)` 

#### `public void `[`maxbotixHRXL_WR_analog`](#classLogger_1a3ce2869bbd48fdebbf44e155981c85b0)`(int nping,int sonicPin,int EX,bool writeAll,uint8_t ADC_resolution_nbits)` 

#### `public void `[`Decagon5TE`](#classLogger_1a40ae372dee7f672a6d6f33ab441e4da1)`(int excitPin,int dataPin)` 

#### `public void `[`DecagonGS1`](#classLogger_1a84da6a9ec3d4d56fdc32d950b71f1a26)`(int pin,float Vref,uint8_t ADC_resolution_nbits)` 

#### `public void `[`vdivR`](#classLogger_1ab1ae31b2bdb77c86fb6851907258171b)`(int pin,float Rref,uint8_t ADC_resolution_nbits,bool Rref_on_GND_side)` 

#### `public void `[`linearPotentiometer`](#classLogger_1a95670d06ec3b68300895cd7bf8c37999)`(int linpotPin,float Rref,float slope,char * _distance_units,float intercept,uint8_t ADC_resolution_nbits,bool Rref_on_GND_side)` 

#### `public void `[`HTM2500LF_humidity_temperature`](#classLogger_1a4ccff7a14a6bddc8bb28e22b3b36d3cc)`(int humidPin,int thermPin,float Rref_therm,uint8_t ADC_resolution_nbits)` 

#### `public void `[`HM1500LF_humidity_with_external_temperature`](#classLogger_1a62b74ddb3cf9fdd7dae2394c1b245ed4)`(int humidPin,float R0_therm,float B_therm,float Rref_therm,float T0degC_therm,int thermPin_therm,uint8_t ADC_resolution_nbits)` 

#### `public void `[`Inclinometer_SCA100T_D02_analog_Tcorr`](#classLogger_1a80fdea5a339573980f9544ac64678411)`(int xPin,int yPin,float Vref,float Vsupply,float R0_therm,float B_therm,float Rref_therm,float T0degC_therm,int thermPin_therm,uint8_t ADC_resolution_nbits)` 

#### `public void `[`Anemometer_reed_switch`](#classLogger_1a6c6a43a1b86f88c2a5e33d14c992e510)`(int interrupt_pin_number,unsigned long reading_duration_milliseconds,float meters_per_second_per_rotation)` 

#### `public void `[`Wind_Vane_Inspeed`](#classLogger_1a31c3cba5ff5722fb66bf540bfbe8b25d)`(int vanePin)` 

#### `public void `[`Pyranometer`](#classLogger_1ae4190ce7ccfd7b148a6151102a3bf93d)`(int analogPin,float raw_mV_per_W_per_m2,float gain,float V_ref,uint8_t ADC_resolution_nbits)` 

#### `public void `[`Barometer_BMP180`](#classLogger_1a40588117a274f0c6de926ef6ce1f0ba8)`()` 

#### `public void `[`_sensor_function_template`](#classLogger_1a98f3cc370c87d1e1eaf3eb6a7e0f1623)`(int pin,float param1,float param2,int ADC_bits,bool flag)` 

#### `public void `[`HackHD`](#classLogger_1a923b296832bd4222da649ebc66427ac1)`(int control_pin,bool want_camera_on)` 

#### `public float `[`Honeywell_HSC_analog`](#classLogger_1a9808967fdf91f10602aa883df35145b3)`(int pin,float Vsupply,float Vref,float Pmin,float Pmax,int TransferFunction_number,int units,uint8_t ADC_resolution_nbits)` 

Generated by [Moxygen](https://sourcey.com/moxygen)