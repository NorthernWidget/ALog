/*
  Blink (modified for the ALog BottleLogger)
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// LED pin
int led;

// the setup routine runs once when you press reset:
void setup() {                
// Pin 13 has an LED connected on most Arduino boards.
// But Pin 9 (or 8, v 2.0.0) is used on the ALog BottleLogger
  #if defined ARDUINO_AVR_ALOG_BOTTLELOGGER_V2
    led=8;
  #else
    led=9;
  #endif
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);     
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);               // wait for a second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);               // wait for a second
}
