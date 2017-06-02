# ALog

**ALog** holds the **Logger** data logger library designed for the Arduino-based ALog (http://northernwidget.com/alog/), but that will also work with any standard Arduino-Uno-based system that is outfitted with a SD card and a DS3231 real-time clock.

This library is optimized to:
1. handle all of the basic file, system, and power management behind-the-scenes, including power consumption reduction to minimal levels through the use of the sleep functions
2. include and expose sensor functions as single-line calls, with a template for the addition of new sensors.

If you use the ALog library (and/or data logger) in a publication, please cite:

**Wickert, A. D. (2014), [The ALog: Inexpensive, Open-Source, Automated Data Collection in the Field](http://onlinelibrary.wiley.com/wol1/doi/10.1890/0012-9623-95.2.68/full), *Bull. Ecol. Soc. Am.*, *95*(2), 68–78, doi:10.1890/0012-9623-95.2.68.**

---

# Quick-start guide

## Using the ALog library

### With an ALog data logger

```
Example code here
```

### With an ALog shield or another system

```
Example code here
```

## Adding support for new sensors

```
Example code here
```

## Main Developers and Contact

The ALog has been developed by Andy Wickert and Chad Sandell at Northern Widget LLC and the University of Minnesota.

For questions related to the Logger library, please send a message to us at info@northernwidget.com.

---

# Guide for first-time users: from the basics onward

Are you new to the ALog, Arduino, and/or C/C++ programming? If so, this page is for you. We'll guide you through the steps to install a first simple program on your ALog data logger, and introduce you to a few concepts along the way.

## Materials Needed

### For the basics:

*   **1x ALog data logger**
*   **1x USB cable that fits the ALog's USB port**
    *   USB A-B for version 2.1.0 and prior
    *   USB A-miniB (like an Android cell phone) for version 2.2.0 and greater
*   **A computer with a USB port**

### For the data logging exercises

*   **1x thermistor**
    *   Our instructions are for the **CanTherm CWF1B103F3380** (available on [Digi-Key](https://www.digikey.com/product-detail/en/cantherm/CWF1B103F3380/317-1310-ND/1191085) or from us). This thermistor is 1 meter long and coated in epoxy for all of your out-of-the-box measurement applications
*   **1x reference resistor**
    *   We typically use the **Vishay Dale PTF5610K000BYEB** for their high precision and temperature stability at a reasonable cost.

---

## A few definitions

*   A **microcontroller** is a teeny tiny computer that has its own:
    *   Processor for performing computations
    *   Memory for storing programs and variables
    *   Metal "pins" to connect to and interact with other devices
    *   An oscillator, such as the ALog BottleLogger's 8 MHz crystal, sets the speed of the microcontroller's computation. This means that the ALog BottleLogger is an 8 MHz computer.
*   A **pin** is one of the "legs" on the microcontroller chip. Electrical power or signlas flow through these.
*   **C** and **C++** are programming languages.
    *   Programs written in these languages must be **compiled**. This means that they are turned into bytes (1s and 0s) by another piece of software before being made executable and (in our case) uploaded to a small board.
*   **Arduino** is a software and hardware standard for microcontroller projects.
    *   They are best known for the Arduino Uno board (and its predecessors).
    *   Also important -- and used by us -- are their extensive software libraries to assist us in programming AVR microcontrollers.
    *   **AVR** is the family of microcontrollers most commonly used by Arduino projects.
*   The **ALog BottleLogger** is a lightweight and inexpensive low-power open-source data logger that incorporates elements of the Arduino system, and can be programmed through the Arduino IDE.

---

## Looking at your board:

Your ALog board should look something like this. It has a lot of components. We'll start to look at them in more detail once you wire the board up. [![](http://northernwidget.com/wp-content/uploads/2017/05/ALogBottleLogger220-1024x443.png)](http://northernwidget.com/wp-content/uploads/2017/05/ALogBottleLogger220.png)

**ALog BottleLogger v2.2.0**

### Dimensions: namesake

The dimensions of the ALog BottleLogger are perfect to slip it into a Nalgene water bottle: these are often easier to find than gasketed boxes, especially if you're in the outdoors (or outdoors shops).

### "Chunky" design for the field

Yes, we use the big, chunky, old-school SD cards, USB ports, and barrel jack connectors. And we love 'em. Why? Have you ever tried to connect cables with mittens in Minnesota-frigid temperatures? Have you ever dropped a micro-SD card (or other object the size of your pinky nail) into a beautiful bed of colorful oak leaves, stand of prairie grasses, or a lake? You'd better hope that your missing item is as big and recognizable as possible. We call it designing for reality.

### What does it have?

*   A teeny tiny computer running at 8MHz
*   An SD card to save data
*   A real-time clock and its backup battery to save the time
*   A USB port to upload programs and talk to the computer
*   A bunch of "screw terminals" to connect wires for sensors, etc.
*   Six pairs of holes for "reference resistors" to act as standards for certain types of measurements ("analog")
*   A blinky light to tell you what's going on

### Pins

The Alog BottleLogger follows the Arduino Uno convention for pin numbering.

### Those little numbers and letters by the screw terminals (pins)

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

#### A0, A1, A2, A3, A6, A7

These are the analog pins. They measure input voltages in a range between 0 and 3.3 volts. What happened to 4 and 5? They are talking to the real-time clock, and are labeled SDA and SCL on the board.

#### D3-INT1, D7, D8 (digital pins)

These pins can read or write TRUE or FALSE values. For writing, TRUE means full power (VCC) is delivered. False means the pin goes to GND. Reading values with these pins tells you whether the observed voltage is closer to VCC or to GND.

#### D3-INT1 (interrupt)

This pin can be used as an interrupt, to observe an event. We often use it with anemometers (spin counters) and tipping-bucket rain gauges.

#### D7 and D8: (PWM)

Pulse-width modulation. Actually, this isn't written on the board. It uses the analogWrite() function in Arduino and simulates a voltage between GND (0V) and VCC by turning itself on and off very quickly, repeatedly.

#### Rx and Tx

Receive and transmit: these are your serial pins. Be careful: they are also attached to the USB port (to talk to your computer), so they must be shared. For the electronics-savvy: they are connected to the computer via 1Kohm in-series resistors, so external sensors will override USB communications.

#### MOSI, MISO, SCK

These are the pins for the SPI communications protocol. This protocol is used to communicate with the SD card. You may also use it with some external sensors.

#### SDA, SCL

These are the pins used for the I2C communications protocol. This works with the real-time clock, and can connect to external peripherals as well. For the tech-heads: we are updating the design to optimize this for 3.3V design, common among I2C devices.

### Special pins

The ALog BottleLogger has several pins that are used for dedicated functions. These are:

*   **Pin 2:** Interrupt connected to clock: triggers logger to wake
*   **Pin 4:** Sensor power
*   **Pin 7:** Clock and SD card 3.3V power
*   **Pin 8:** LED on/off

### Reference resistor headers

The pair of plastic vertical "headers" -- things with holes in them for wires -- are for reference resistors to read analog resistance-based sensors. A simple example is a thermistor, and we'll cover that farther on down here. ---

## Programming Arduino-based systems: Focus on the ALog

### Download and install the Arduino IDE

The Arduino IDE (Integrated Development Environment) is the current programming environment we use to write and upload programs to the ALog. (Other options exist, but this is the most beginner-friendly.) We haven't yet tested the brand-new web editor, so we'll be suggesting an old-fashioned download. And if you're deploying these in the field, you'll need the downloaded version! Go to [https://www.arduino.cc/en/Main/Software](https://www.arduino.cc/en/Main/Software). Get it, install it, go.

### Set up the ALog boards definitions

The ALog boards are "third-party" Arduino boards, so you'll have to set up support for these boards yourself. We've made a pretty thorough walkthrough that you can view here: [https://github.com/NorthernWidget/Arduino_Boards](https://github.com/NorthernWidget/Arduino_Boards)

### Download and install the custom ALog libraries

The ALog also relies on some custom software libraries. These are collections of computer code that help make everything from running basic data logging utilities to interfacing with specific sensors easier. We're introducing you to these libraries up here, even though they won't be needed until quite a bit further down. While we highly recommend them and they are very helpful for field work, you can also program the ALog without them! In fact, the earlier examples (below) do not include the custom libraries.

#### Download:

**Stable release: [[ZIP]](http://northernwidget.com/ALog_libraries/stable/ALog_libraries.zip) [[TAR.GZ]](http://northernwidget.com/ALog_libraries/stable/ALog_libraries.tar.gz) (recommended)** Nightly build: [[ZIP]](http://northernwidget.com/ALog_libraries/nightly/ALog_libraries.zip) [[TAR.GZ]](http://northernwidget.com/ALog_libraries/nightly/ALog_libraries.tar.gz) (for only those who can tolerate things breaking once in a while)

#### Extract/Install:

Unzip the archive, and place the contents in the "Arduino" folder. This should be in your home directory or your Documents. Then restart the Arduino IDE (if it is open).

#### Easier ways in the future?

Yes, we're going to integrate our libraries with the automated library manager in the Arduino IDE. Yippee! **EXPECT MORE HERE IN THE FUTURE**

### Learn some basics of programming in the Arduino language

If you're not a programmer (yet!), you might be thinking, "yeah, right." Yeah, right! You can program! We'll walk you through it:

*   First, we'll point you to some nice resources
*   Then, we'll show you how to write a simple code to blink your LED.
*   Finally, you'll combine your LED blinking with serial communications with your computer.

_[For the technical folks: The Arduino programming language is variant of C/C++.]_

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

Yes, there **is** a manual! The full reference guide is here [https://www.arduino.cc/en/Reference/HomePage](https://www.arduino.cc/en/Reference/HomePage).

#### The internet

We're not kidding. We type things into our favorite search engine when we can't figure them out and/or are too tired to think. 90+% of the time, this gets us to the right answer. Sometimes it takes longer than others. Arduino forums and StackExchange are pretty great: [**Arduino forums**](https://forum.arduino.cc/) **[Arduino stack exchange](https://arduino.stackexchange.com/)** [stack exchange is just about the most effective way of conveying the right answers for how to do things that I've (Andy has, that is!) ever seen on the internet]

### Basic syntax of C programming

Programming languages all have ways in which locial thoughts are placed into text that is entered into a computer. Here is a non-exhaustive list of these. It is not intended to be complete, but to give you some essential tools to start reading the code that appears below.

#### Defining variables

Each variable needs to be defined before it is used.

<pre class="lang:default decode:true">bool isOn=False; // Boolean: True (1) or False (0)
int x=50; // Integer (this works for values between -32768 and 32767).
          // For broader applicability, look up uint32, int64, etc.
float _my_float = 1000.293; // "Floating point" (decimal) number
char letter='h' // This is a letter, but is represented in ASCII, so is
                // also a 1-byte number</pre>

Each of these goes: **variable_type variable_name = variable_value ;** For more information on types, see the Sparkfun Electronics tutorial at https://learn.sparkfun.com/tutorials/data-types-in-arduino. For what you are doing right now, a general idea of what these definitions are and what they look like should work.

#### Comments

Ignore lines that start with:

<pre class="lang:default decode:true">// [compiler doensn't care what is here, but you can read it
</pre>

And lines that are between

<pre class="lang:default decode:true">/*
 whatevs
 */</pre>

These are for the benefit of the humans reading the code. Comment your code clearly and thoroughly!

#### Line endings

Lines must be terminated with semicolons.

<pre class="lang:default decode:true">int a=5;
</pre>

#### Operations

Basic arithmetic operators (<span class="lang:c++ decode:true crayon-inline">+, -, *, /</span>) work. Division of two integers results in "floor division, or rounding down the result. There are also logical operators (<span class="lang:c++ decode:true crayon-inline"><, >, ==</span>, etc.)

#### Loops and Statements

These are ways to control the flow of a code. I'm afraid that this introduction to the ALog would be dilluted by the details of these. I point the interested reader to the main Arduino reference: [https://www.arduino.cc/en/Reference/HomePage](https://www.arduino.cc/en/Reference/HomePage).

* * *

## Connecting the ALog and Computer

Use a USB cable to connect the ALog to your computer. <PICTURE here> Then, using the Arduino IDE, select the name of the USB-serial port that is connected to the ALog. <PICTURE here> Now your ALog is ready for communications and programming. ---

## Example Programs: Writing and Uploading

Arduino programs, often called "sketches", are how you tell the ALog data logger what to do. Here is some information to get you started.

### A new program

If you open a new Arduino program, it will look like this:

<pre class="lang:c++ decode:true" title="Default Arduino code">void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
</pre>

<span class="lang:c++ decode:true crayon-inline">setup()</span> and <span class="lang:c++ decode:true crayon-inline">loop()</span> are both **functions**. These are specific sets of code that are executed when their names are called in the code. These two functions are special in that:

1.  Every Arduino program much have them.
2.  They do not need to be called by code that you write: Arduino autmatically interprets and, indeed, requires that these be included.
    *   Everything inside the curly braces after <span class="lang:c++ decode:true crayon-inline">setup()</span> is run once, when the data logger starts running
    *   Everything inside the curly braces after <span class="lang:c++ decode:true crayon-inline">loop()</span> is run continuously, after <span class="lang:c++ decode:true crayon-inline">setup()</span>.

Before these functions, you can place all of your variable definitions.

## Blink

Let's make a blinky light turn on and off! On the ALog, **pin 8** is attached to a red indicator LED. Here is some code to create a synocpated blinky light.

<pre class="lang:c++ decode:true" title="Default Arduino code">// Syncopated LED blink

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
</pre>

#### Uploading code to the ALog data logger

Once your code is written -- either as a copy/paste of this or as your own -- save your code. All Arduino sketches need to be within their own folder. After this, you can hit the "upload" button (right arrow) to load the code to the board. (The check mark to the left will test if your code compiles.) <Insert image>

### USB/Serial communications

A second important piece of your work with the Arduino is to communicate with it. We'll start with one-way communications, with the ALog talking to your computer. We will still flash the LED, and will add in information on how to define a variable outside of these two functions. I break this down in the comments.

<pre class="lang:c++ decode:true">// Global variables
// These variables are availalble 
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
}</pre>

Let's break this down a little bit:   **Change your baud rate to 38400 bps!**  

* * *

## Using the ALog library

#### Logger library

https://htmlpreview.github.io/?https://raw.githubusercontent.com/NorthernWidget/Logger/master/doc/html/classLogger.html  

#### Thermistor example

For now, we'll go through this as a group, but text will go here eventually!

<pre class="lang:c++ decode:true">#include "Logger.h"

Logger logger;

////////////////////////////
// USER-ENTERED VARIABLES //
////////////////////////////
char* dataLoggerName = "Thermistor Logger";
char* fileName = "TempLog.txt"; // Name of file for logged data: 8.3 format (e.g, 
                             // ABCDEFGH.TXT); <= 8 chars before ".txt" is OK

//Setup logging interval here, may setup more than one variable. 
//Minimum interval = 1 sec, maximum interval is 23 hours, 59 minutes, 59 seconds
//0 for all means that the logger will not sleep
int Log_Interval_Seconds = 10; //Valid range is 0-59 seconds
int Log_Interval_Minutes = 0; //Valid range is 0-59 minutes
int Log_Interval_Hours = 0; //Valid range is 0-23 hours
bool external_interrupt = false; // e.g., rain gage

void setup(){  //Serial baud rate is set to 38400
  logger.initialize(dataLoggerName, fileName,
    Log_Interval_Hours, Log_Interval_Minutes, Log_Interval_Seconds,
    external_interrupt);logger.setupLogger();
}

void loop(){
// ***************************************** 
logger.goToSleep_if_needed(); // Send logger to sleep
logger.startLogging();  // Wake up and initialize
// ****** DO NOT EDIT ABOVE THIS LINE ****** 

//////////////////////////////////
// READ SENSORS: EDIT THIS PART //
//////////////////////////////////

// Analog sensors: place all analog sensor commands between
// startAnalog() and endAnalog().
// If you have no analog sensors, you should comment out the 
// startAnalog() and endAnalog() commands

logger.startAnalog();

// CanTherm small bead
//logger.thermistorB(10000, 3950, 10000, 25, 0);
// CanTherm expoxy bead: CWF1B103F3380
logger.thermistorB(10000, 3380, 10000, 25, 0);

logger.endAnalog();

//
// INSERT DIGITAL SENSOR READING COMMANDS HERE!
//

// NOTE: THE BUFFER SIZE IS 512 BYTES;
// run "logger.bufferWrite" if you think you are approaching this limit.
// Otherwise, the buffer will overflow and I'm not sure what will happen.

// ****** DO NOT EDIT BELOW THIS LINE ****** 

// Wrap up files, turn off SD card, and go back to sleep
logger.endLogging();

// ***************************************** 

}
</pre>

## Connecting sensors

[![](http://northernwidget.com/wp-content/uploads/2017/05/ALogUsersLayout-791x1024.png)](http://northernwidget.com/wp-content/uploads/2017/05/ALogUsersLayout.png) [[PDF of User's Layout guide]](http://northernwidget.com/wp-content/uploads/2017/05/ALogUsersLayout_smallfile.pdf)
