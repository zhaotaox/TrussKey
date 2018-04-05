

#include <SoftwareSerial.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include "SIM800.h"
#include "MAX17043.h"
#include "Wire.h"


#define APN "connect"
#define WARMUP 300
#define con Serial

int mq3_analogPin = A2; // connected to the output pin of MQ3 
int solenoid_Pin = 10;

int RedLED = 7;
int GreenLED = 8;
int BlueLED = 9;
const int pwrPin = 3;  
int detSW_Pin = 2;
int shutdown_Pin = 4;


/*Variables used for MQ303 sensor*/
volatile int timer = 0;
int base_val = 0;
int max_val = 0;
int diff = 0;
int passed = 1;
volatile byte powerOn = LOW;
int readingCounter = 0;

unsigned long powerTimer = 0;

unsigned long longPressTime = 500;
int longPressActive = 0;

/*GPRS module variables*/

char* smsMSG = "TrussKey is being tampered\032";
char* smsNum = "+14125195973";
CGPRS_SIM800 gprs;
uint32_t count = 0;
uint32_t errors = 0;
int switchPin = A1;
int smsSent;
int sleep_gprs;

unsigned long time;

volatile byte state = LOW;

MAX17043 batteryMonitor;


void setup(){
  Wire.begin();
  con.begin(9600); // open serial at 9600 bps
  pinMode(mq3_analogPin,INPUT);
  pinMode(solenoid_Pin,OUTPUT);
  pinMode(BlueLED,OUTPUT);
  pinMode(GreenLED,OUTPUT);
  pinMode(RedLED,OUTPUT);
  pinMode(shutdown_Pin,OUTPUT);

  pinMode(pwrPin,INPUT);
  attachInterrupt(digitalPinToInterrupt(pwrPin),pwr_SW,RISING);
  //attachInterrupt(digitalPinToInterrupt(detSW_Pin),det_SW,FALLING);
  batteryMonitor.reset();
  batteryMonitor.quickStart();
  //turnLEDOff();
  con.println("Start");
}

void loop()
{
  turnLEDOff();
  digitalWrite(shutdown_Pin,LOW);
//  if (digitalRead(pwrPin)){
//    readingCounter++;
//    con.print("Reading counter");
//    con.println(readingCounter);
//
//    /*powerOn determines whether power pin gets pressed
//      Keep looping till the pin is set high
//      Firt press -  Power On
//      Second press - Power Off*/
//    if (powerOn == 0){
//      //powerOn = 1;
//      if (readingCounter % 2 == 1){
//        //powerTimer = millis();
//        //con.println(powerTimer);
//        //con.println(longPressActive);
//        //con.println(millis() - powerTimer);
//        turnBlueOn();
//        powerOn = 1;
//      }
//      /*Clear warm up timer for sensor*/
//      else{
//        //readingCounter = 0;
//        turnLEDOff();
//        powerOn = 0;
//        timer = 0;
//      }
//    }
//    /*Clear warm up timer for sensor*/
//    else{
//      powerOn = 0;
//      //readingCounter++;
//      //con.println("Turn Off");
//      turnLEDOff();
//      timer = 0;
//    }
//    delay(500);
//  }
  //powerOn = 1;
  con.println(powerOn);
//  if (digitalRead(pwrPin)){
//    con.println("Power Pin Pressed");
//  }
  /*After power is on*/
  //powerOn = 1;
  if (!powerOn){
    //sleepGPRS();
    //sleepNow();
  }
  if (powerOn){
    //delay(1000);
    Serial.println("Pressed");
    digitalWrite(shutdown_Pin,HIGH);
    turnBlueOn();
    //con.println(powerOn);
    float cellVoltage = batteryMonitor.getVCell();
    con.print("Voltage:\t\t");
    con.print(cellVoltage, 4);
    con.println("V");
  
    float stateOfCharge = batteryMonitor.getSoC();
    con.print("State of charge:\t");
    con.print(stateOfCharge);
    con.println("%");
    turnBlueOn();
    digitalWrite(solenoid_Pin,LOW);
    passed = 1;
    int values[15];

    /*Warm up  
      Keep track of every 15 sensor value
      The last of warm up is used as base value*/
    for (int i = 0; i < 15; i++){
      int mq3_value = 1023 - analogRead(mq3_analogPin);
      values[i] = mq3_value;
      con.print(values[i]);
      con.print("\tis\t");
      con.println(i);
      delay(100); //Just here to slow down the output.
      if ( i == 14){
        base_val = values[i];
      }
      timer = timer + 1;
      con.println(timer);
    }

    /*After warm up, light up LED to cue user to breathe*/
    if (timer > WARMUP){
      //con.println("Start");
      //digitalWrite(led2,LOW);
      for (int i = 0; i < 5;i++){
        turnBlueOn();
        delay(500);
        turnLEDOff();
        delay(500);
      }
      max_val = 0;

      /*User has about 5 seconds to breathe
        Find max value of sensor reading during this period
        If max value is 160 higher than base value, FAIL
        Otherwise if max value is at least 20 higher than base value (make sure someone breathes), PASS*/
        
      for (int i = 0; i < 15; i++){
        int mq3_value = 1023 - analogRead(mq3_analogPin);
        values[i] = mq3_value;
        con.print("Testing val is");
        con.println(values[i]);
        if (values[i] > max_val){
          max_val = values[i];
        }
        if (i > -1){
          diff = values[i] - base_val;
          con.print("diff=");
          con.println(diff);
          if (diff > 160){
            turnRedOn();
            digitalWrite(solenoid_Pin,LOW);
            delay(3000);
            passed = 0;
            con.println("FAIL");
            break;
          }
        }
        delay(500);
      }
      if ((passed == 1) && ((max_val - base_val) > 20)){ 
        turnGreenOn();
        delay(500);
        digitalWrite(solenoid_Pin,HIGH);
        passed = 2; 
        Serial.println("PASS");
      }
      if (passed != 2){
        turnRedOn();
        delay(500);
        con.println("FAIL");
      }
      delay(1000);
      /*Resets all values*/
      for (int i = 0; i < 15; i++){
        values[i] = 0;
      }
      max_val = 0;
      base_val = 0;
      timer = 0;
      turnLEDOff();
      digitalWrite(shutdown_Pin,LOW);
      powerOn = 0;
      delay(2000);
    }
  }
}

void turnLEDOff()
{
  digitalWrite(BlueLED,HIGH);
  digitalWrite(RedLED,HIGH);
  digitalWrite(GreenLED,HIGH);
}
void turnBlueOn()
{
  digitalWrite(BlueLED,LOW);
  digitalWrite(RedLED,HIGH);
  digitalWrite(GreenLED,HIGH);
}
void turnRedOn()
{
  digitalWrite(BlueLED,HIGH);
  digitalWrite(RedLED,LOW);
  digitalWrite(GreenLED,HIGH);
}
void turnGreenOn()
{
  digitalWrite(BlueLED,HIGH);
  digitalWrite(RedLED,HIGH);
  digitalWrite(GreenLED,LOW);
}
// watchdog interrupt
ISR(WDT_vect) 
{
  wdt_disable();  // disable watchdog
}
void sleepNow()
{
  power_adc_disable(); // Disable ADC before going to sleep
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);  // Power down sleep mode
  sleep_enable(); //Enables sleep bit in the MCUCR register
  attachInterrupt(digitalPinToInterrupt(pwrPin),pwr_SW,RISING);
  
  sleep_mode();            // now goes to Sleep and waits for the interrupt


  sleep_disable();  //  First thing after waking up  (Comes back to ISR)
  //detachInterrupt(0);
}
//Put gprs to sleep
void sleepGPRS()
{
  sleep_gprs = gprs.sleep(1);
}
// Detector switch de-pressed interrupt routine
void det_SW()
{
  /*Initiate GPRS*/
  interrupts();
  con.print("Det Tampered\n");
  gprs.sleep(0);
  delay(500);
  for (;;) {
    while (!gprs.init()) {
    }
    byte ret = gprs.setup(APN);
    if (ret == 0)
      break;
  }

  for (;;) {
    if (gprs.httpInit()) break;
    //con.println(gprs.buffer);
    gprs.httpUninit();
    delay(1000);
  }

  smsSent = gprs.sendSMS(smsNum,smsMSG);

  noInterrupts();
  //state = !state;
}

/*Interrupt routine for power switch*/
void pwr_SW()
{
  //interrupts();
  powerOn = !powerOn;
  timer = 0;
  con.print("Power On:");
  con.println(powerOn);
  con.println("Interrupt initiated");
  //noInterrupts();
}
