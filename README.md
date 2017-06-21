# ALog

The **ALog** library is a toolkit for open-source data logging designed for the Arduino-based ALog ([http://northernwidget.com/alog/](http://northernwidget.com/alog/)), but that will also work with any standard Arduino-Uno or -Mega-based system that is outfitted with a SD card and a DS3231 real-time clock.

This library is optimized to:
1. handle all of the basic file, system, and power management behind-the-scenes, including power consumption reduction to minimal levels through the use of the sleep functions
2. include and expose sensor functions as single-line calls, with a template for the addition of new sensors.

If you use the ALog library (and/or data logger) in a publication, please cite:

**Wickert, A. D. (2014), [The ALog: Inexpensive, Open-Source, Automated Data Collection in the Field](http://onlinelibrary.wiley.com/wol1/doi/10.1890/0012-9623-95.2.68/full), *Bull. Ecol. Soc. Am.*, *95*(2), 68–78, doi:10.1890/0012-9623-95.2.68.**

In addition to the README.md at [https://github.com/NorthernWidget/ALog](https://github.com/NorthernWidget/ALog), documentation is available as a combination of the information here and an index of logger functions in both [[HTML]](http://northernwidget.github.io/ALog) and [[PDF]](http://northernwidget.github.io/ALog/refman.pdf) format.

* * *

# Quick-start guide

***No-frills, no-pictures, as quick as possible. All the same material is covered in more detail in the*** **"Complete guide: from the basics onward,"** *** below; look there if you get stuck***

1. Download and install the Arduino IDE from [https://www.arduino.cc/en/Main/Software](https://www.arduino.cc/en/Main/Software)
2. Install all required software libraries: start the IDE, go to "Sketch --> Include Library --> Manage Libraries...", and search for and install the following:
    1. The ALog library; you can find this more easily by typing "Northern Widget" into the search box
    2. The DS3231 library (for the real-time clock); this can also be found by typing "Northern Widget"
    3. The SdFat library (for the SD card); simply typing "sdfat" into the search box will do.
    4. The BMP180 library from SparkFun electronics (**key difference:** This must be downloaded and installed separately via a copy/paste into your Arduino/libraries folder; we are looking into options to avoid this necessity)
3. Add support for the ALog boards. (Skip this step if you're using a non-ALog Arduino.) Detailed instructions, available from [https://github.com/NorthernWidget/Arduino_Boards](https://github.com/NorthernWidget/Arduino_Boards), are also included below in the Complete Guide.
    1. Go to File --> Preferences (Arduino --> Preferences on Mac) and paste this URL into the "Additional Boards Manager URLs" entry zone, in the lower right: https://raw.githubusercontent.com/NorthernWidget/Arduino_Boards/master/package_NorthernWidget_index.json
    2. Go to Tools --> Boards --> Boards Manager; type in "Northern Widget" and install these boards definitions.
4. Choose the board that you will be using. Go to tools --> board, and then (most likely) --> ALog BottleLogger v2.
5. Using a USB cable, plug your ALog data logger (or compatible Arduino device) into the computer
6. Go to File (Arduino on Mac) --> Examples --> ALog --> BasicStart. Click on it to load the file.
7. Upload the code to the logger: click on the "upload" button (right arrow) or press CTRL+u (command+u on Mac)
8. Look at the logger. You should see the blue and yellow lights flashing to show that it is communicating with the computer.
9. Open the serial monitor (top right button) and set the baud rate to 38400 bps.
10. See if the logger works; if it does, you are ready to start adding commands to read sensors. (If it doesn't, please check your progress through these steps, look through the rest of this guide, and if you really can't figure it out, email us at [info@northernwidget.com](mailto:info@northernwidget.com).

* * *

# Basic Reference

***For those already familiar with Arduino and with all of the required software already installed***

## Using the ALog library

### Structure of an Arduino sketch with the ALog library

The following code is an Ardiuno "sketch" that includes the Alog library. It works whether you are using an:

*   ALog data logger
*   Arduino Uno (with ALog shield or equivalent clock and SD card)
*   Arduino Mega (with ALog shield or equivalent clock and SD card)

*Support for ARM-series Arduino boards is currently under development.*

```cpp
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

  // If you are using a standard Arduino board (i.e. not a full ALog data logger)
  // and are not using the Arduino shield, you will have to set the proper pins for
  // the indicator LED (defualts to 9) and the SD card and RTC power (default to -1
  // to be inactive in the case of constant power supply; set these to the same
  // value if there is just one switch for both of these).
  // Replace "_pin" with your desired pin number, and uncomment the relevant line(s).
  // set_LEDpin(_pin);
  // set_SDpowerPin(_pin);
  // set_RTCpowerPin(_pin);

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
```

### Sensor commands

Sensor commands are defined in detail in the extended help, available below (PDF version) and at [http://northernwidget.github.io/ALog/classALog.html](http://northernwidget.github.io/ALog/classALog.html).

In short, they look like:

```cpp
  alog.sensorName(parameter1, parameter2, parameter3)
```
where these parameters can be the pin numbers to which the sensors are attached, sensor-specific electrical characteristics (e.g., resistance), calibration values, and quantities that affect how the sensor behaves during its operation.

One example, for a thermistor (a temperature-sensitive resistor), is:
```cpp
  logger.thermistorB(10000, 3380, 10000, 25, 0);
```
Where the parameters are, in order:

*   **R0**: Known resistance at given known temperature (here 10,000 ohms)
*   **B**: The parameter governing the curvature of the resistance-to-temperature calibration curve (here 3380 K)
*   **Rref**: The resistance of the known resistor (here also 10,000 Ω, to optimize measurement sensitivity at 10 kΩ)
*   **T0degC**: The temperature [in Celsius] at which the known resistance equals **R0** (here 25°C)
*   **thermPin**: The analog pin number to be read

This thermistor example is reiterated and expanded upon in the "Guide for first-time users: from the basics onward", below.

## Adding support for new sensors

Printed below is the template function designed to guide users about how to add support for additional sensors. You may also look at ALog.cpp and ALog.h for our current examples, and feel free to contact us ([info@northernwidget.com](mailto:info@northernwidget.com)) if you have questions about how to properly incorporate new sensors.

If you do add your sensor to our library or make an improvement, **we would really appreciate it if you woudl contact us about including the changes you've made**. We have designed the ALog library as a resource for the community, and the more of us who make it better, the bigger and better open-source field instrumentation grows!

```cpp
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
```

## Main Developers and Contact

The ALog has been developed by Andy Wickert and Chad Sandell at Northern Widget LLC and the University of Minnesota.

For questions related to the Logger library, please send a message to us at [info@northernwidget.com](mailto:info@northernwidget.com).

* * *

# Complete guide: from the basics onward

Are you new to the ALog, Arduino, and/or C/C++ programming? If so, this page is for you. We'll guide you through the steps to install a first simple program on your ALog data logger, and introduce you to a few concepts along the way.

## Materials Needed

### For the basics:

*   **1x ALog data logger**
*   **1x USB cable that fits the ALog's USB port**
    *   USB A-B for version 2.1.0 and prior
    *   USB A-miniB (like an Android cell phone) for version 2.2.0 and later
*   **A computer with a USB port**

### For the data logging exercises:

*   **1x thermistor**
    *   Our instructions are for the **CanTherm CWF1B103F3380** (available on [Digi-Key](https://www.digikey.com/product-detail/en/cantherm/CWF1B103F3380/317-1310-ND/1191085) or from us). This thermistor is 1 meter long and coated in epoxy, making it a good "out-of-the-box" tool, though we recommend calibration if you are going to try to make measurements with greater than factory precision (±1% of nominal resistance value, here 10 kiloohms (kΩ) at 25 °C).
*   **1x reference resistor**
    *   We typically use the **Vishay Dale PTF5610K000BYEB** for their high precision and temperature stability at a reasonable cost. Note that this factory precision is compounded with the precision of the thermistor when calculating error!
*   **1x screwdriver**: Slotted. We recommend a 0.4 x 2 mm blade. Good options are [this one](https://www.digikey.com/product-detail/en/wiha/26020/431-1176-ND/510465) and its [ESD-safe version](https://www.digikey.com/product-detail/en/wiha/27220/431-1202-ND/512198).

### For the field

*   **"The Basics"** (above):
    *   **ALog data logger**
    *   **Computer**
    *   **USB cable**
*   Any **sensor(s)** you need
*   **Battery pack** with power cable attached; solar panel optional; see "Power" section for more details.
*   **Housing** with any required holes drilled, cable glands and associated waterproofing, and (recommended) attachment points for logger and power supply; see "Housing and waterproofing" section for more details.
* **Tools**: my personal toolkit typically includes:
    * Screwdrivers (2-3): I recommend one standard larger one with both a Phillips head and a flat head (slotted) for general-purpose use and a small 0.4 x 2 mm blade slotted one for the loggers. Good options for the latter are [this one](https://www.digikey.com/product-detail/en/wiha/26020/431-1176-ND/510465) and its [ESD-safe version](https://www.digikey.com/product-detail/en/wiha/27220/431-1202-ND/512198). If you have lots of hose clamps, a nut driver becomes a great asset.
    * Angle cutters
    * Wire stripper
    * Spare wire
    * Multimeter
    * Adjustable wrench
    * Portable soldering iron: battery-powered (you get maybe 15-20 minutes on a 4x AA model) or butane
    * Lead-free solder
    * Electrical tape
    * Hose clamps
    * Permanent markers (I like multiple sizes of Sharpies)
    * Cable ties

PICTURE OF MY TOOL ROLL HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

## A few definitions

*   A **microcontroller** is a teeny tiny computer that has its own:
    *   Processor for performing computations
    *   Memory for storing programs and variables
    *   Metal "pins" to connect to and interact with other devices (see immediately below)
    *   An oscillator, such as the ALog BottleLogger's 8 MHz crystal, sets the speed of the microcontroller's computation. This means that the ALog BottleLogger is an 8 MHz computer.
*   A **pin** is one of the "legs" on the microcontroller chip. Electrical power or signlas flow through these. Many of these are connected to the **screw terminals** on the ALog board.
*   `C` and `C++` are **programming languages**.
    *   Programs written in these languages must be **compiled**. This means that they are turned into bytes (1s and 0s) by another piece of software before being made executable and (in our case) uploaded to a small board.
*   **Arduino** is a software and hardware standard for microcontroller projects.
    *   They are best known for the Arduino Uno board (and its predecessors).
    *   Also important -- and used by us -- are their extensive software libraries to assist us in programming AVR microcontrollers.
    *   **AVR** is the family of microcontrollers most commonly used by Arduino projects.
*   The **ALog BottleLogger** is a lightweight and inexpensive low-power open-source data logger that incorporates elements of the Arduino system, and can be programmed through the Arduino IDE.

## Looking at your board:

Your ALog board should look something like this. It has a lot of components. We'll start to look at them in more detail once you wire the board up.

![ALog BottleLogger v2.2.0 top side photo](https://github.com/NorthernWidget/ALog/raw/master/doc/figures/ALogBottleLogger220.png "ALog BottleLogger v2.2.0 top side photo")
@image latex "../figures/ALogBottleLogger220.png" "ALog BottleLogger"

***ALog BottleLogger v2.2.0 top side photo***

### Dimensions: namesake

The dimensions of the ALog BottleLogger are perfect to slip it into a Nalgene water bottle: these are often easier to find than gasketed boxes, especially if you're in the outdoors (or outdoors shops).

### "Chunky" design for the field

Yes, we use the big, chunky, old-school SD cards, USB ports, and barrel jack connectors. And we love 'em. Why? Have you ever tried to connect cables with mittens in Minnesota-frigid temperatures? Have you ever dropped a micro-SD card (or other object the size of your pinky nail) into a beautiful bed of colorful oak leaves, stand of prairie grasses, or a lake? You'd better hope that your missing item is as big and recognizable as possible. We call it designing for reality.

(An update... while we love the old-schoool cool of the big USB A-B cable, we
couldn't help but notice that tons of us were traveling with A-micro-B cables
thanks to Android phones, but had to make a special effort to remember to bring
our A-B cables to the field. So we've made a recent (2017) update to the USB
port in our design.)

### What does it have?

*   A teeny tiny computer running at 8MHz
*   An SD card to save data
*   A real-time clock and its backup battery to save the time
*   A USB port to upload programs and talk to the computer
*   A bunch of "screw terminals" to connect wires for sensors, etc.
*   Six pairs of holes for "reference resistors" to act as standards for certain types of measurements ("analog")
*   A blinky light to tell you what's going on

### Pins

The metal connections between the microcontroller (teeny tiny computer) and any external component of the system (e.g., power supply, sensors) are called "pins".

You are not attaching anything directly to the pins on the integrated circuit, obviously; your interface to the ALog data logger will be primarily through the screw terminals (which in turn are discussed immediately below) at the edges of the board. I refer to each screw terminal as a "pin" or "port", or simly as a "screw terminal"; there is no hard-and-fast language for this, except insofar as only those microcontroller pins that are exposed to the outside world via screw terminals are called "ports" in this documentation.

The Alog BottleLogger follows the Arduino Uno convention for pin numbering.

More information on pins can be found towards the end of this document, in the "Pin definitions" section.

### Those little numbers and letters by the screw terminals

Those black things on the sides with screw heads are the "screw terminals". They clamp down on wires to connect peripherals to the ALog BottleLogger, securely. There are several different sets of letters by them. These are:

#### VCC

"Voltage of the common connector": this is raw power from the batteries. Keep it above 3.6V and below 5.5V. Any higher and you fry things; any lower and the system starts shutting down. We usually use:

*   USB (5V)
*   3x AA or D batteries

#### 5V

This is 5V power, supplied by a switchable charge pump. Note that it will be closer to 5.2V if very small voltages are required. It is most often used to power 5V devices.

#### 3V3

This is 3.3V power, supplied by a switchable voltage regulator. It powers 3.3V devices and is used as a reference (and often a power source) for analog measurements.

#### GND

This is ground. Circuits that start at VCC end here. Don't touch power directly to ground unless you want to destroy something. It is the same as 0V.

#### A0, A1, A2, A3, A6, A7 (Analog pins, ALog BottleLogger)

These are the analog pins. They measure input voltages in a range between 0 and 3.3 volts. What happened to 4 and 5? They are talking to the real-time clock, and are labeled SDA and SCL on the board.

All analog pins can be used as digital pins as well!

When writing code, the analog pins can be referred to as A0, A1, A2, A3, A6, or A7 safely in all cases.

*Additional info:*

*The analog pins can be referred to by their base numeral (for the ALog BottleLogger, 0, 1, 2, 3, 6, or 7) in the `AnalogRead(analogPinNumber)` command that we'll see below; this is the legacy of early versions of Arduino. They must be elsewhere referred to as A0, A1, A2, A3, A6, or A7.*

*A bit of advanced knowledge: "A0" maps to "14", "A1" maps to 15, and soforth until A7 maps to 21; this is because there are 14 digital pins (0-14), so the analog numbers come right after these. Therefore, while I find A0...A7 easier to remember, you may prefer to use the numbers >13, which is equally valid.*

#### D3-INT1, D7, D8 (digital pins, ALog BottleLogger)

These pins can read or write TRUE or FALSE values. For writing, TRUE means full power (VCC) is delivered. False means the pin goes to GND. Reading values with these pins tells you whether the observed voltage is closer to VCC or to GND.

When programmin these pins, refer to them by their number alone, dropping the D (and anything after the number that immediately follows the D.

##### D3-INT1 (interrupt, ALog BottleLogger)

This pin can be used as an interrupt, to observe an event. We often use it with anemometers (spin counters) and tipping-bucket rain gauges.

When programming with this pin as an interrupt, use the following syntax:
`
attachInterrupt(digitalPinToInterrupt(3), ...)
`

If you insert the interrupt number (1), this will correspond to Pin 3 on the Arduino Uno or Alog BottleLogger, but will refer to Pin 1 on ARM-based devices, so this will cause compatibility problems.

##### D7 and D8: (PWM, ALog BottleLogger)

Pulse-width modulation. Actually, this isn't written on the board. It uses the analogWrite() function in Arduino and simulates a voltage between GND (0V) and VCC by turning itself on and off very quickly, repeatedly.

#### Rx and Tx

Receive and transmit: these are your serial pins. Be careful: they are also attached to the USB port (to talk to your computer), so they must be shared. For the electronics-savvy: they are connected to the computer via 1Kohm in-series resistors, so external sensors will override USB communications. See "UART", below.

#### MOSI, MISO, SCK

These are the pins for the SPI communications protocol. This protocol is used to communicate with the SD card. You may also use it with some external sensors. See section "SPI", below.

#### SDA, SCL

These are the pins used for the I2C communications protocol. This works with the real-time clock, and can connect to external peripherals as well. For the tech-heads: we are updating the design to optimize this for 3.3V design, common among I2C devices. See section "I2C", below.

### Special pins

The ALog BottleLogger has several pins that are used for dedicated functions. These are:

*   **Pin 2:** Interrupt connected to clock: triggers logger to wake
*   **Pin 4:** External (sensor) power 3.3V and 5V regulator on/off switch
*   **Pin 5:** The "LOG" (also referred to as "LOG NOW") button uses this pin to tell the logger to wake up and take a reading immediately.
*   **Pin 7:** Clock and SD card power on/off switch
*   **Pin 8:** Message-flashing LED on/off
*   **Pin 10:** The chip-select pin for the SPI communication to the SD card; more on this in the "SPI" section, below

In addition, there are three significant digital interfaces that have uses internal to the ALog but that may also be used to communicate with external sensors:

#### UART

The UART, or universal asynchronous receiver--transmitter, communicates to the computer via a USB--serial converter (model number FT231X). It transmits these signals through in-series 1 kΩ resistors that act to limit the current that passes in this signal, with the goal of allowing external sensors that communicate via the UART to take precedence over USB--serial communications to the computer. While this is available and functions, we recommend against its use as there will almost certainly be some amount of interference between the communications with the computer and with the sensor. In-development versions of ALog boards will have multiple UART ports, and these will include a dedicated port for communication with the computer and other UART ports for use with sensors.

*   **Pin 0:** Rx (external device sends, logger receives)
*   **Pin 1:** Tx (logger sends, external device receives)

#### SPI

SPI is a 4-wire communication protocol (i.e., bus): three wires/pins are shared between all SPI devices, while a fourth one is used as a "chip select" pin that is set LOW for communication with the logger and HIGH when all communications should be ignored.

It is straightforward to extend SPI to other devices by configuring one of the pins on the ALog data logger to be the new "chip select" pin. In addition to this, the pins with a set configuration are:

*   **Pin 11:** MOSI (logger sends, external device receives)
*   **Pin 12:** MISO (external device sends, logger receives)
*   **Pin 13:** SCK (serial clock)

The abbreviations stand for "Master out, slave in", "master in, slave out", and "serial clock". I'm really not OK using the term "slave" for about a million reasons, so whenever I need to refer to the device attached to the logger, I will write "agent" or "external device". This goes for any bus -- not just SPI.

Additional information on SPI and setting up additional SPI devices may be found at:[https://www.arduino.cc/en/reference/SPI](https://www.arduino.cc/en/reference/SPI).

#### I2C

I2C is a 2-wire communications protocol. It is used to communicate with the real-time-clock (RTC, model number DS3231), as well as many sensors. Each attached device has its own address, enabling **up to 128 sensors** to be attached to the same I2C port as the clock. But just be careful that your sensor code is good -- if it is not and halts the I2C system, it could affect the clock and therefore the entirety of the operations of the ALog!

*   **Pin A4** (= pin 18): SDA (Serial DAta)
*   **Pin A5** (= pin 19): SCL (Serial CLock)

### Reference resistor headers

The pair of plastic vertical "headers" -- things with holes in them for wires -- are for reference resistors to read analog resistance-based sensors. A simple example is a thermistor, and we'll cover that farther on down here. For now, just consider these resistors to be standards against which many analog sensors will be compared to measure their resistance values.

## Programming Arduino-based systems: Focus on the ALog

### Download and install the Arduino IDE

The Arduino IDE (Integrated Development Environment) is the current programming environment we use to write and upload programs to the ALog. (Other options exist, but this is the most beginner-friendly.) We haven't yet tested the brand-new web editor, so we'll be suggesting an old-fashioned download. And if you're deploying these in the field, you'll need the downloaded version! Go to [https://www.arduino.cc/en/Main/Software](https://www.arduino.cc/en/Main/Software). Get it, install it, go.

### Set up the ALog boards definitions

The ALog boards are "third-party" Arduino boards, so you'll have to set up support for these boards yourself. We've made a pretty thorough walkthrough that you can view here at [https://github.com/NorthernWidget/Arduino_Boards](https://github.com/NorthernWidget/Arduino_Boards), and have included those installation instructions here, immediately below.

Each board will be added as an entry to the Arduino **Tools** > **Board** menu.

Go to **File** > **Preferences** (or **Arduino** > **Preferences** on Mac).

<!--- [Adding a board manager list](https://cdn.sparkfun.com/assets/learn_tutorials/4/5/4/arduino-board-add.png) (link to Sparkfun's tutorial) -->

![Open the Arduino IDE preferences](https://github.com/NorthernWidget/ALog/raw/master/doc/figures/Arduino_Boards/File_Preferences.png "Open the Arduino IDE preferences.")
@image latex Arduino_Boards/File_Preferences.png "Arduino IDE preferences"

***Open the Arduino IDE preferences***


Open 'Additional Boards Manager URLs', and paste the following in either the box for **Additional Boards Manager URLs**, or, if this is populated, the window that pops up when you hit the button to the right of the **Additional Boards Manager URLs** text entry area:

	https://raw.githubusercontent.com/NorthernWidget/Arduino_Boards/master/package_NorthernWidget_index.json

![Paste the URL here](https://github.com/NorthernWidget/ALog/raw/master/doc/figures/Arduino_Boards/PreferencesWindow.png)
@image latex Arduino_Boards/PreferencesWindow.png "URL here"

***Paste the URL here.***

![Unless you already have done that for third-party boards... in that case, open this frame and paste the URL here.](https://github.com/NorthernWidget/ALog/raw/master/doc/figures/Arduino_Boards/BoardURLs_list.png "Unless you already have done that for third-party boards... in that case, open this frame and paste the URL here.")
@image latex Arduino_Boards/BoardURLs_list.png "Or here"

***Unless you already have done that for third-party boards... in that case, open this frame and paste the URL here.***

Now, go to **Tools** > **Board** > **Boards Manager...**.

![Open the boards manager here.](https://github.com/NorthernWidget/ALog/raw/master/doc/figures/Arduino_Boards/Tools_Boards_BoardManager.png "Open the boards manager here.")

@image latex Arduino_Boards/Tools_Boards_BoardManager.png "Boards"

***Open the boards manager here.***

Click it, and the following window will appear:

![Boards Manager.](https://github.com/NorthernWidget/ALog/raw/master/doc/figures/Arduino_Boards/BoardsManager.png "Boards Manager.")
@image latex Arduino_Boards/BoardsManager.png "Boards manager"

***Boards Manager.***

if you type in "Northern Widget" (or usually just "Northern" as well), you should see an option to install board files for Northern Widget Arduino compatible boards.

![Northern Widget boards.](https://github.com/NorthernWidget/ALog/raw/master/doc/figures/Arduino_Boards/BoardsManager_Northern.png "Northern Widget boards.")
@image latex Arduino_Boards/BoardsManager_Northern.png "Boards"

***Northern Widget boards.***

***If Northern Widget options do not appear.*** restart your Arduino IDE and try again.

Click "Install" to add the NorthernWidget boards to your list. At the time of writing, we support only AVR boards, but this may change soon.

![ALog (AVR) support installed.](https://github.com/NorthernWidget/ALog/raw/master/doc/figures/Arduino_Boards/BoardsManager_Northern_Done.png "ALog (AVR) support installed.")
@image latex Arduino_Boards/BoardsManager_Northern_Done.png "Done."

***ALog (AVR) support installed.***

Now, when you select the Boards list, you will see a collection of new boards for Northern Widget.

You will then want to change your chosen board to the board that you have. At the time of writing, this is probably the "ALog BottleLogger v2"; the "legacy" option is used for v2.0.0-beta and prior, and there are fewer of these boards in circulation.

![Select the proper board.](https://github.com/NorthernWidget/ALog/raw/master/doc/figures/Arduino_Boards/Tools_Boards_NorthernWidget_ALog_BottleLogger_v2.png "Now, when you go to Tools > Board, you should see the Northern Widget ALog boards. Select the proper board.")
@image latex Arduino_Boards/Tv2.png "Select board"

***Select the proper board.***

**That's it! You're done, and ready to rock and roll... er, collect data, with your ALog data logger.**

### Download and install the custom ALog libraries

The ALog also relies on some custom software libraries. These are collections of computer code that help make everything from running basic data logging utilities to interfacing with specific sensors easier. We're introducing you to these libraries up here, even though they won't be needed until quite a bit further down. While we highly recommend them and they are very helpful for field work, you can also program the ALog without them! In fact, the earlier examples (below) do not include the custom libraries.

#### Option 1: downloading and installing through the Arduino IDE

***Stable versions, but outdated during times of rapid development***

The Arduino IDE has a library manager! Here we will walk you through how to use it to obtain and install the requisite libraries. This uses the graphical interface, but can take longer than, "Option 2", below.

At the moment, you will also have to download the stable release (below) and copy the SFE_BMP180 library into your "Arduino/libraries" folder; we're working to streamline this.

First, open the libraries manager.

![Open the libraries manager.](https://github.com/NorthernWidget/ALog/raw/master/doc/figures/ArduinoScreenshots/libraries_sketch-include-manage.png "Open the libraries manager.")
@image latex ArduinoScreenshots/libraries_sketch-include-manage.png "libraries"

Then, look up each of the three core libraries:

DE3231 (real-time clock):
![DS3231 library.](https://github.com/NorthernWidget/ALog/raw/master/doc/figures/ArduinoScreenshots/libraryDS3231.png "DS3231 library.")
@image latex ArduinoScreenshots/libraryDS3231.png "DS3231 library"

SdFat (SD card):
![SdFat library.](https://github.com/NorthernWidget/ALog/raw/master/doc/figures/ArduinoScreenshots/librarySdFat.png "SdFat library.")
@image latex ArduinoScreenshots/librarySdFat.png "SdFat library"

ALog (all data logging!):
![ALog library.](https://github.com/NorthernWidget/ALog/raw/master/doc/figures/ArduinoScreenshots/libraryALog.png "ALog library.")
@image latex ArduinoScreenshots/libraryALog.png "ALog library"

Once this is done, go to the instructions for Option 2 (immediately below) to get the SFE_BMP180 library.

#### Option 2: direct installation from a prepared file archive

##### Download:

**Stable release: [[ZIP]](http://northernwidget.com/ALog_libraries/stable/ALog_libraries.zip) [[TAR.GZ]](http://northernwidget.com/ALog_libraries/stable/ALog_libraries.tar.gz) (recommended)**

Nightly build: [[ZIP]](http://northernwidget.com/ALog_libraries/nightly/ALog_libraries.zip) [[TAR.GZ]](http://northernwidget.com/ALog_libraries/nightly/ALog_libraries.tar.gz) (for only those who can tolerate things breaking once in a while)

##### Extract/Install:

Unzip the archive, and place the contents in the "Arduino/libraries" folder. This should be in your home directory or your Documents. Then restart the Arduino IDE (if it is open).

### Learn some basics of programming in the Arduino language

If you're not a programmer (yet!), you might be thinking, "yeah, right, I can't program."

But we say, **"Yeah, right! You can program!"** We'll walk you through it:

*   First, we'll point you to some nice resources
*   Then, we'll show you how to write a simple code to blink your LED.
*   Finally, you'll combine your LED blinking with serial communications with your computer.

_[For the folks already in the know: the Arduino programming language is variant of C/C++.]_

### Programming resources

#### Tutorials from Adafruit

These are serious good. You can follow along with them! [https://learn.adafruit.com/series/learn-arduino](https://learn.adafruit.com/series/learn-arduino) If you follow them, note that the ALog BottleLogger can take only 3.6 -- 5.5 volts, and that the LED is controlled by Pin 8 (not 13). Here is their version of the "blink" example that we'll do below. https://learn.adafruit.com/adafruit-arduino-lesson-1-blink?view=all Look at as many of these as you want! Yes, your ALog can:

*   Sense sensors
*   Motor motors
*   Blink blinky things
*   Textt text via the serial monitor
*   Type text onto a display screen
*   More more more!

#### The full reference guide

Yes, there **is** a manual! The full reference guide to programming Arduino devices is here [https://www.arduino.cc/en/Reference/HomePage](https://www.arduino.cc/en/Reference/HomePage).

#### The internet

We're not kidding. We type things into our favorite search engine when we can't figure them out and/or are too tired to think. 90+% of the time, this gets us to the right answer. Sometimes it takes longer than others. Arduino forums and StackExchange are pretty great: [**Arduino forums**](https://forum.arduino.cc/) **[Arduino stack exchange](https://arduino.stackexchange.com/)** [stack exchange is just about the most effective way of conveying the right answers for how to do things that I've (Andy has, that is!) ever seen on the internet]

### Basic syntax of C programming

Programming languages all have ways in which locial thoughts are placed into text that is entered into a computer. Here is a non-exhaustive list of these. It is not intended to be complete, but to give you some essential tools to start reading the code that appears below.

#### Defining variables

Each variable needs to be defined before it is used.

```cpp
bool isOn=False; // Boolean: True (1) or False (0)
int x=50; // Integer (this works for values between -32768 and 32767).
          // For broader applicability, look up uint32, int64, etc.
float _my_float = 1000.293; // "Floating point" (decimal) number
char letter='h' // This is a letter, but is represented in ASCII, so is
                // also a 1-byte number
```

Each of these goes: **variable_type variable_name = variable_value ;** For more information on types, see the Sparkfun Electronics tutorial at https://learn.sparkfun.com/tutorials/data-types-in-arduino. For what you are doing right now, a general idea of what these definitions are and what they look like should work.

#### Comments

Ignore lines that start with:

```cpp
// [compiler doensn't care what is here, but you can read it]
```

And lines that are between

```cpp
/*
 whatevs
 */
```

These are for the benefit of the humans reading the code. Comment your code clearly and thoroughly!

#### Line endings

Lines must be terminated with semicolons.

```cpp
int a=5;
```

#### Operations

Basic arithmetic operators (`+, -, *, /`) work. Division of two integers results in "floor division, or rounding down the result. There are also logical operators (`<, >, ==`, etc.)

#### Loops and Statements

These are ways to control the flow of a code. I'm afraid that this introduction to the ALog would be dilluted by the details of these. I point the interested reader to the main Arduino reference: [https://www.arduino.cc/en/Reference/HomePage](https://www.arduino.cc/en/Reference/HomePage).

## Connecting the ALog and Computer

Use a USB cable to connect the ALog to your computer. **PICTURE here** Then, using the Arduino IDE, select the name of the USB-serial port that is connected to the ALog. **PICTURE here** Now your ALog is ready for communications and programming. RETURN HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

POWER CONNECTOR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!?
SECTION FOR THAT IS NOW BELOW!!!

## Example Programs: Writing and Uploading

Arduino programs, often called "sketches", are how you tell the ALog data logger what to do. Here is some information to get you started.

### A new program

If you open a new Arduino program, it will look like this:

```cpp
void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
```

`setup()` and `loop()` are both **functions**. These are specific sets of code that are executed when their names are called in the code. These two functions are special in that:

1.  Every Arduino program much have them.
2.  They do not need to be called by code that you write: Arduino autmatically interprets and, indeed, requires that these be included.
    *   Everything inside the curly braces after `setup()` is run once, when the data logger starts running
    *   Everything inside the curly braces after `loop()` is run continuously, after `setup()`.

Before these functions, you can place all of your variable definitions.

### Uploading code to the ALog data logger

Once your code is written -- either as a copy/paste of this or as your own -- save your code. All Arduino sketches need to be within their own folder. After this, you can hit the "upload" button (right arrow) to load the code to the board. (The check mark to the left will test if your code compiles.) **Insert images** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

### Blink

Let's make a blinky light turn on and off! On the ALog, **pin 8** is attached to a red indicator LED. Here is some code to create a synocpated blinky light.

```cpp
// Syncopated LED blink

void setup() {
 // put your setup code here, to run once:
 pinMode(8, OUTPUT); // "Output" means that the maximum current possible will be available at Pin 8
                     // to drive the LED
}

void loop() {
 // put your main code here, to run repeatedly:
 digitalWrite(8, HIGH); // Turn power to light ON
 delay(750); // Wait for 750 milliseconds
 digitalWrite(8, LOW); // Turn power to light OFF
 delay(250);
 digitalWrite(8, HIGH);
 delay(250);
 digitalWrite(8, LOW);
 delay(250);
}
```

Look at the comments in the code for information on what each of the commands do; these are detailed more below in the USB/Serial communicatinos section.

### USB/Serial communications

A second important piece of your work with the Arduino is to communicate with it. We'll start with one-way communications, with the ALog talking to your computer. We will still flash the LED, and will add in information on how to define a variable outside of these two functions. The comments give you some general idea of what each section does; I will break down the importance of each section below.

*Before running this code, open the serial monitor and* ***change your baud rate to 38400 bps!***
**Insert images to open serial monitor and change baud rate** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

```cpp
// Global variables
// These variables are availalble to any function inside your code
int LEDpin = 8;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(38400);
  pinMode(LEDpin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Hi, computer.");
  delay(1000);
  digitalWrite(LEDpin, HIGH);
  delay(500);
  digitalWrite(LEDpin, LOW);
}
```

Let's break this down a little bit:

#### Global variables and pin definitions

```cpp
// Global variables
// These variables are availalble to any function inside your code
int LEDpin = 8;
```

Here we are defining the pin number that controls the LED. This is Pin 8, which is a digital pin; analog pins would generally have an "A" before their pin number, and can also be used for digital input and output.

#### Setup

```cpp
void setup() {
  // put your setup code here, to run once:
  Serial.begin(38400);
  pinMode(LEDpin, OUTPUT);
}
```

"**Serial**" is a library for USB-Serial communications between the computer and the attached Arduino. It is a core part of Arduino, and therefore does not have to be "included" at the top in the way the ALog library (farther below) must be. To view serial communications, you may open the serial monitor in the Arduino IDE (the figures immediately above describe how to do this).

Within the `Serial.begin()` command, we set the **baud rate** to "38400"; this is in bits per second. *Techincal note: We choose this rate because it is closer to being integer-divisible by our 8MHz processor clock speed than other baud rates are.*

the **pinMode** command can be set to **input** or **output**; "input" means that the pin is ready to read an incoming signal; output means that it maximizes the amount of electrical current it can source to send a strong output signal that is also enough (40 mA absolute maxumim) to power basic devices.

#### Loop

```cpp
void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Hi, computer.");
  delay(1000);
  digitalWrite(LEDpin, HIGH);
  delay(500);
  digitalWrite(LEDpin, LOW);
}
```

**Serial.println** prints the requested text and then moves ahead to a new line for the next print command (if any).

**delay** takes an argument in milliseconds that describes how long you would like the logger to do nothing but count time passing.

**digitalWrite** turns an outpu tpin ON (HIGH) or OFF (LOW). When HIGH, the pin is sending current at a voltage corresponding to VCC (i.e., the positive voltage for the system). When low, it is sending current at a voltage corresponding to GND (ground / Earth).

## Using the ALog library

The full reference to the ALog library is located at
[http://northernwidget.github.io/ALog/classALog.html](http://northernwidget.github.io/ALog/classALog.html).

The following sections walk you through the functionality.

### ALog library components

The ALog library contains:
* **Global variables** that are important across the entire library, such as pins based on the design of the board that you are using and variables that determine how frequently the logger will read sensors;
* An **object-oriented interface** that allows the user to interact with the ALog library;
* **Utility code** that carries our behind-the-scenes work to keep the ALog running well; and
* **Sensor functions** that are pre-written and allow users to connect with sensors as part of a single-line function call.

#### Object-oriented interface

The end user can ***instantiate*** the ALog class, that is, create one's own version of it for a particular purpose, as follows:
```cpp
ALog alog;
```
Here, "ALog" is the class, and "alog" is one's instance of the class. It's a bit like picking out a Lego set from a store: it becomes yours, and you can build what you like with it, within the constraints of the pieces that you have.

You may call any function within the ALog class by typing `alog.<function_name>(<parameters_to_pass>)`.

#### Utility code (both "private" and "public" functions)

The utility code is the unseen backbone of the ALog library, and much of the reason that I wrote it in the first place. It handles power consumption, device stability, communications with the user, real-time clock management, and reading and writing files to and from the SD card.

##### Sleeping and waking

The sleep mode is the most important pice of the ALog data logger. It's the superlative of when you close the lid of a laptop computer: the ALog goes from drawing at least 6.7 milliamps to just 53 microamps -- that's 0.053 milliamps, for a **greater than 100x increase in power efficiency**, and therefore battery life.

The internal sleep functions are activated by the end-user as part of this command:
```
alog.goToSleep_if_needed(); // Send logger to sleep
```

When the logger **wakes up**, it takes readings and then goes back to sleep. How does it wake up? There are two ways:

1. The real-time clock's alarm function triggers an interrupt.
2. An event activates a second interrupt and causes it to wake; an example is a bucket tip from a rain gauge.

The logger can also be run in a mode to **maximize logging speed** (around 10 Hz at top speed, but not well-measured at the time of writing) and never sleep!

##### Real-time clock

The real-time clock maintains the absolute time to within ±0.432 s/day. It also operates two alarms, which can be used to trigger the ALog.

As a safety feature, we enable the second of these alarms to go off several after the first one is triggered at the user-specified time. If the first alarm goes off, the second is disabled and advanced farther into the future. Otherwise, if the first alarm doesn't go off, the second alarm is triggered and the logger advances both alarms to the next logging interval. It also records the time at which the failure of the first alarm took place. Thus far, I have never seen this in use outside of test cases in which we force the first alarm to fail.

Interfacing with the real-time-clock is performed through the **DS3231 library**, which is a combination of RTClib (a standard real-time clock library by J.-C. Wippler) and code to run the DS3231 written by E. Ayars and edited by A. Wickert.

All times are represented as a **UNIX Epoch**, seconds since January 1st, 1970. This is a very widespread format with plenty of converters into conventional date/time, and solves the problems of (a) storage of date/time data into a complex data type on a small system, and (b) daylight savings time and leap years.

##### SD card

Utilities to read and write to and from the SD card are managed via the **SdFat** library, by Bill Greiman. The ALog will write to the following files, in rough order of how common their use is:

***Always included:***

* **[DataFileCustomName].txt**: A data file, with the name chosen by the user; for safety's sake, I always go with 8.3 format, but this is more old habit.
* **header.txt**: A header file with the names of each column listed. A new timestamped row is added each time the logger is rebooted, to ensure that the data in the data file may always be matched to their appropriate real-world significance.

***Sometimes included:***

* **bucket_tips.txt** records the times at which a tipping-bucket rain gauge's bucket tips; more tips over a time interval means heavier rain!
* **Camera.txt** records the times at which an attached camera that may lack its own clock takes photos or video
* **Oversample.txt** contains every individual analog reading that is combined into an oversampled resultant measurement, if the "debug" flag is set to `True`
* **Alarm_miss.txt**: Any time the first alarm has not gone off and the backup alarm has had to save the day, a time stamp is written to this file.

Power to the SD card is cut between logging intervals to conserve energy. This is one reason why we do not use the built-in Arduino SD library, which cannot handle losing power to the SD card (or at least could not back when I first tried, in 2011!)

##### Watchdog timer

The watchdog timer will automatically reboot the ALog data logger if it stalls out while logging for more than 8 seconds. This can help to recover from any software errors, as well as any unforeseen circumstances arising in the field (e.g., jostled while logging causing a disruption of some connectinos). This can really save the day!

##### LED messages

These messages are flashed on the large red LED. Those in bold are those that you may see most commonly.

* Syncopated: **daaaa-da-daaaa-da-daaaa-da**: Clock has reset to the year 2000! Please set the clock.
* **LONG-short-short**: All is good! Starting to log.
* 5 quick flashes: Missed first alarm; caught by backup alarm. Rebooting logger.
* **20 quick flashes**: SD card failed, or (more likely) is not present. This also happens if it is not seated properly; reseat the card and try again.
* 50 quick flashes: you have tried to reassign a pin critical to the ALog to another function; this will most likely break the system. Check your code and re-upload.
* 100 quick flashes: your Arduino model is not recognized by the ALog library!

##### EEPROM: Serial number and calibration values for voltage regulators

The EEPROM of an ALog data logger or other Arduino-based device is the memory that remains unchanged even if it is reprogrammed. It holds important **identifying and calibration information** about the ALog data logger.

* The **serial number** of the data logger is a 16-bit integer held in the first two bytes of the EEPROM (bytes 0 and 1)
* The **measured voltage of the 3.3V regulator** on the ALog BottleLogger, if recorded, is stored in bytes 2-5 of the EEPROM.
* The **measured voltage of the 5V charge pump** on the ALog BottleLogger, if recorded, is stored in bytes 6-9 of the EEPROM.

These can be read by the functions:
* get_serial_number()
* get_3V3_measured_voltage()
* get_5V_measured_voltage()

You can use these functions with sensors by passing the results from a call to `get_3V3_measured_voltage()` or `get_5V_measured_voltage()` to functions as `Vsupply` or `Vref`, these being the supply an reference voltages for the sensors, respectively.

#### Sensor functions (mostly "public" functions, exposed to the user)

The full index of the sensor functions, along with ways to use them, is here:
[http://northernwidget.github.io/ALog/classALog.html](http://northernwidget.github.io/ALog/classALog.html)

See the thermistor example (below) and the information in the "Basic Reference" (above) for a more in-depth description of how to create and use sensor functions.

### Thermistor example

Thus far, we have explored many different aspects of the ALog bottle logger. Now, let's bring many of these together in an example of source code for the ALog. In this example, we will measure a temperature-sensitive resistor, or "thermistor", using the analog measurement capabilities of the ALog. This is one of the most basic measurements that one can make with the ALog, but also one of the most important and most commonly employed.

```cpp
#include "ALog.h"

ALog alog;

// Note: Serial baud rate is set to 38400 bps

////////////////////////////
// USER-ENTERED VARIABLES //
////////////////////////////
char* dataLoggerName = "Thermistor Logger"; // Name of logger; displayed in Serial communications
char* fileName = "TempLog1.txt"; // Name of file for logged data: 8.3 format (e.g,
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

  // CanTherm small bead
  //logger.thermistorB(10000, 3950, 10000, 25, 0);
  // CanTherm expoxy bead: CWF1B103F3380
  logger.thermistorB(10000, 3380, 10000, 25, 0);

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
```

It's time to break this down into its component steps:

####

```cpp
#include "ALog.h"

ALog alog;
```

The `#include` call tells the sketch to use the ALog library.

The second line of source code creates an instance of the ALog object, called "alog". We will use "alog" as our portal into all of the tools that the ALog library offers.

```
// Note: Serial baud rate is set to 38400 bps
```

Self-explanatory!


```cpp
////////////////////////////
// USER-ENTERED VARIABLES //
////////////////////////////
char* dataLoggerName = "Thermistor Logger"; // Name of logger; displayed in Serial communications
char* fileName = "TempLog1.txt"; // Name of file for logged data: 8.3 format (e.g,
                            // ABCDEFGH.TXT); <= 8 chars before ".txt" is OK

//Setup logging interval here, may set up more than one variable.
//Minimum interval = 1 sec, maximum interval is 23 hours, 59 minutes, 59 seconds
//0 for all means that the logger will not sleep
int Log_Interval_Seconds = 10; //Valid range is 0-59 seconds
int Log_Interval_Minutes = 0; //Valid range is 0-59 minutes
int Log_Interval_Hours = 0; //Valid range is 0-23 hours
// external_interrupt is true for a tipping bucket rain gauge
bool external_interrupt = false;
```

`dataLoggerName` is an identifier for the logger. I typicallly make it relate to the field site, the task of the logger, and a code number.

`fileName` is the name of the main data file to be logged to the SD card. (See SD card section, above, for information on all of the files that may be written to the SD card.) This is noted to be in 8.3 format, but doesn't strictly have to be following updates to the `SdFat` library.

`Log_Interval_Seconds`, `Log_Interval_Minutes`, and `Log_Interval_Hours` all determine how often the logger will record data. Data recording is set to always occur on hours/minutes/seconds that are synchronous across all devices and are set to be referenced as much as possible to a the start of an hour/day/minute (more details in source code).

`external_interrupt` is true if a device that triggers an instant response is attached, and false if it is not. As noted, this is most commonly a tipping-bucket rain gauge, but it could realliy be any sensor. The appropriate sensor functions will define the response to the interrupt; the simplest case (rain gauge) is that a time-stamp is recorded to a specific file.

```cpp
void setup(){
  alog.initialize(dataLoggerName, fileName,
    Log_Interval_Hours, Log_Interval_Minutes, Log_Interval_Seconds,
    external_interrupt);
```

This is the start of the `setup()` step, which is run once when the logger turns on. Here, the variables from the previous section are passed to the "alog" object instance, which then creates the appropriate variables and their derivatives within the object. This initializes the object and prepares it for logging.

```cpp
  // If you are using a standard Arduino board (i.e. not a full ALog data logger)
  // and are not using the Arduino shield, you will have to set the proper pins for
  // the indicator LED (defualts to 9) and the SD card and RTC power (default to -1
  // if you are not programming an ALog data logger in order
  // to be inactive in the case that you are using the Arduino's 3V3 regulator to
  // power to run the RTC and SD card (as we do with the ALog shield); set these to the same
  // value if there is just one switch for both of these).
  // Replace "_pin" with your desired pin number, and uncomment the relevant line(s).
  // set_LEDpin(_pin);
  // set_SDpowerPin(_pin);
  // set_RTCpowerPin(_pin);
```

As is written in the comments, set these values for the indicator LED, SDpowerPin, and RTCpowerPin. The latter two are typically unset on non-ALog Arduino boards, and are equal to each other (but already set within the ALog library) for ALog boards.

```cpp
  alog.setupLogger();
}
```

At this point, the logger has all the information it needs from you, and completes its internal setup.

```cpp
void loop(){
  // *****************************************************

  alog.goToSleep_if_needed(); // Send logger to sleep
  alog.startLogging(); // Power up all systems, check WDT, reset alarms,
                       // and open data file(s) in write mode

  // ************ DO NOT EDIT ABOVE THIS LINE ************
```

This is the start of the loop, which repeates infinitely unless the ALog data logger is shut off or fails.

Here, the comments are self-explanatory, mostly; a couple extra explanations:

* "Sleep" is a low-power mode; it will exit this (and complete this function) when triggered by the clock, a press of the "LOG" button, or an user-defined external interrupt.
* WDT = watchdog timer; this causes the logger to reboot if it hangs for >8 seconds, and therefore allows it to recover from failures.

```cpp
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

  // CanTherm small bead
  //logger.thermistorB(10000, 3950, 10000, 25, 0);
  // CanTherm expoxy bead: CWF1B103F3380
  logger.thermistorB(10000, 3380, 10000, 25, 0);

  alog.sensorPowerOff();
```

This is it, where the magic happens!

`alog.sensorPowerOn()` and `alog.sensorPowerOff()` turn the voltage regulators that power external sensors on and off, respectively -- just like it says in the comments!

`logger.thermistorB(...)` is the command. The parameters passed, in order, are:

* Factory standard resistance of thermistor [ohms]
* Factory standard b-value (or beta-value) of thermistor [ohms]
* Reference resistor resistance [ohms]
* Standard temperature to which factory standard resistance corresponds [ohms]
* Analog pin number (this could equally be A1 or 14 for the ALog BottleLogger)

Do you want to know more about these parameters? Look up the function-specific help, which is below if you are looking at the PDF version of the help (downloadable from [http://northernwidget.github.io/ALog/refman.pdf](http://northernwidget.github.io/ALog/refman.pdf)) and available online at [http://northernwidget.github.io/ALog/classALog.html](http://northernwidget.github.io/ALog/classALog.html).

```cpp
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! //
  // INSERT COMMANDS TO READ SENSORS THAT DO NOT REQUIRE ALOG-SUPPLIED POWER HERE! //
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! //
```

We don't have any of these here, but examples would be sensors with their own power-supplies, and/or (not consistent wiht the comment above) sensors that are powered directly via the ALog BottleLogger's VCC. This is not included in the comment because it is generally not how we run sensors -- we like to be able to switch them off to conserve power!

```cpp
  // NOTE: THE BUFFER SIZE IS 512 BYTES;
  // run "alog.bufferWrite()" between your commands in this section
  // if you think you are approaching this limit.
  // Otherwise, the buffer will overflow and I'm not sure what will happen!
  // alog.bufferWrite()
```

As it says: if you think you're in danger of overflowing the buffer, run this command! You'll need 512 characters in your line for this to happen though, and I've never appraoched this in my work. But hey, someone might!

```cpp
  // ************ DO NOT EDIT BELOW THIS LINE ************

  // Wrap up files, turn off SD card, and go back to sleep
  alog.endLogging();

  // *****************************************************
}
```

As it says, the `alog.endLogging()` command finalizes everything before sending the logger back to sleep. And that's really all there is to it! Your first ALog Arduino sketch in a nutshell.

And as always, if anything here is unclear, please contact us and we'll try our best to fix it -- specific advice appreciated!

## Setting the real-time clock

The ALog's clock is a key component -- without it, all the data are sitting in an unreferenced time frame. This is why we have chosen the DS3231 real-time clock, which has the highest accuracy and stability of any commonly-available real-time clock that does not incorporate an absolute time referenge (e.g., with GPS). GPS would be great for some circumstances, and we're developing a logger with it, but this would not be the extreme low-power device that the ALog BottleLogger is.

Enough with the background -- how do you set the clock?

You will need the **ALog library** and the **DS3231 library**, both of which you should already have at this point, as well as the **ALogTalk** repository of Python scripts at [github.com/NorthernWidget/ALogTalk](github.com/NorthernWidget/ALogTalk). In order to run the Python scripts, you need either **Python 2** (including **pyserial**) or for us to have made an executable version for your platform.

### Integrated into the ALog code

When you upload the ALog code to your ALog data logger or Arduino board, it includes code to set the clock via serial communications. Plug in your data logger, run **ALogClockSet.py** (which is in the **ALogTalk** repository), and it should work -- if it doesn't, hit the reset button.

How do you run **ALogClockSet.py**?

**All operating systems:** Open a terminal, navigate to the directory that holds **ALogClockSet.py**, and type:
```
python ALogClockSet.py
```

Don't have Python? You probably will have figured it out when that failed Here are your options:

1. Install Python 2.X; I recommmend getting the **anaconda** distribution
2. Download an executable verison of **ALogClockSet.py**; email us if you can't find this (it's not always up-to-date due to the overhead required for making executables)



## Connecting sensors (and more) to the ALog

![User's layout and connection guide](https://github.com/NorthernWidget/ALog/raw/master/doc/figures/ALogUsersLayout.png "User's layout and connection guide")
@image latex ALogUsersLayout.pdf "Layout and connection guide" width=6.5in

***User's layout and connection guide***

[[PDF of User's Layout guide]](https://github.com/NorthernWidget/ALog/raw/master/doc/ALogUsersLayout_smallfile.pdf)
[(backup link)](http://northernwidget.com/wp-content/uploads/2017/05/ALogUsersLayout_smallfile.pdf)

### Pin definitions

This list includes only those pins on the ALog BottleLogger with which the user can connect.

| Pin number (Arduino)  | Label on ALog BottleLogger| Capabilities                          |
| --------------------- |-------------------------- | -------------------------             |
| 0                     | Rx                        | UART Rx*                              |
| 1                     | Tx                        | UART Tx*                              |
| 3                     | D3/INT1                   | External interrupt; digital I/O       |
| 6                     | D6                        | PWM**; digital I/O                    |
| 9                     | D9                        | PWM**; digital I/O                    |
| 11                    | MOSI                      | SPI MOSI                              |
| 12                    | MISO                      | SPI MISO                              |
| 13                    | SCK                       | SPI SCK                               |
| A0 (14)               | A0                        | Analog input (10-bit); digital I/O    |
| A1 (15)               | A1                        | Analog input (10-bit); digital I/O    |
| A2 (16)               | A2                        | Analog input (10-bit); digital I/O    |
| A3 (17)               | A3                        | Analog input (10-bit); digital I/O    |
| A4 (18) / SDA         | SDA                       | I2C SDA                               |
| A5 (19) / SCL         | SCL                       | I2C SCL                               |
| A6 (20)               | A6                        | Analog input (10-bit); digital I/O    |
| A7 (21)               | A7                        | Analog input (10-bit); digital I/O    |

*The UART is connected to the USB-Serial converter; using it with sensors is possible, but requires caution when designing new UART sensor interfaces to avoid interference with communications with the computer.

**PWM = "Pulse-width modulation"

This table does not include pin capabilities that are technically possible but not recommended. These include:

* Rx and Tx pins (UART), SPI pins, and I2C pins can all be used as standard digital I/O pins; this would cause problems communicating with the computer, communicating with the SD card (and sensors), and communicating with the clock (and sensors), respectively.
* I2C pins (A4 and A5) are not recommended to be used as analog input pins, though technically they are; this will cause problems with communication with the real-time clock and any other I2C device

### Additional ALog ports

| Label on ALog BottleLogger| Connected to...                                                       |
| ------------------------- |---------------------------------------------------------------------- |
| GND                       | Ground (Earth / 0V / battery negative (-) terminal)                   |
| 3V3                       | 3.3V ±1% precision low-dropout voltage regulator                      |
| 5V                        | 5V charge pump; 5V is nominal: for tiny loads, it can go up to 5.2V   |
| VCC                       | "Voltage of the common connector" (Power / battery (+) terminal)      |


### How to attach wires to screw terminals

Tools needed:

*   Screwdriver, slotted. We recommend a 0.4 x 2 mm blade. Good options are [this one](https://www.digikey.com/product-detail/en/wiha/26020/431-1176-ND/510465) and its [ESD-safe version](https://www.digikey.com/product-detail/en/wiha/27220/431-1202-ND/512198).
*   Sensor !!!!!!!!!!!!!!!!!!!!!!!!!

Also helpful:

*   Common sense
*   Caution

PHOTOS AND WRITING HERE!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

### Rererence resistors (what are they good for?)

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

## Field deployment

### Power

!!!!!!!!!!!!!!

### Housing and waterproofing

!!!!!!!!!!!!!!!!!

###

!!!!!!!!!!!!!!!!!!!!
