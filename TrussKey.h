#include <SoftwareSerial.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include "SIM800.h"

#define APN "connect"
#define WARMUP 300


int mq3_analogPin = A2; // connected to the output pin of MQ3 
int solenoid_Pin = 9;
int led1 = 5;
int led2 = 3;
const int pwrPin = 6;  
int detSW_Pin = 3;

/*Variables used for MQ303 sensor*/
int timer = 0;
int base_val = 0;
int max_val = 0;
int diff = 0;
int passed = 1;
volatile int powerOn = 0;
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

unsigned long time;

volatile byte state = LOW;
