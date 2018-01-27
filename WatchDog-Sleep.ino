#include <avr/sleep.h>
#include <avr/wdt.h>

#define LED 13
unsigned long time;
// watchdog interrupt
ISR(WDT_vect) 
{
  wdt_disable();  // disable watchdog
}

void myWatchdogEnable(const byte interval) 
{  
  MCUSR = 0;                          // reset various flags
  WDTCSR |= 0b00011000;               // see docs, set WDCE, WDE
  WDTCSR =  0b01000000 | interval;    // set WDIE, and appropriate delay

  wdt_reset();
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
  sleep_mode();            // now goes to Sleep and waits for the interrupt
} 

void setup()
{
  pinMode (LED, OUTPUT);
  Serial.begin(9600);
}  // end of setup

void loop()
{
  //digitalWrite (LED, HIGH);  // awake
  Serial.print("Time: ");
  time = millis();
  //prints time since program started
  Serial.println(time);
  delay(2000);  //do stuff here
  //digitalWrite (LED, LOW);  // asleep

  // sleep for a total of 20 seconds
  myWatchdogEnable (0b100001);  // 8 seconds
  myWatchdogEnable (0b100001);  // 8 seconds
  myWatchdogEnable (0b100000);  // 4 seconds

}  // end ofloop

// sleep bit patterns:
//  1 second:  0b000110
//  2 seconds: 0b000111
//  4 seconds: 0b100000
//  8 seconds: 0b100001
