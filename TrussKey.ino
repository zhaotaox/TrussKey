#include "TrussKey.h"


// connected to the output pin of MQ3 
int mq3_analogPin = A2; 
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

volatile byte state = LOW;

void setup(){
  Serial.begin(9600); // open serial at 9600 bps
  pinMode(mq3_analogPin,INPUT);
  pinMode(solenoid_Pin,OUTPUT);
  pinMode(led1,OUTPUT);
  pinMode(led2,OUTPUT);
  pinMode(13,OUTPUT);
  pinMode(pwrPin,INPUT);
  attachInterrupt(digitalPinToInterrupt(detSW_Pin),det_SW,LOW);
}

void loop()
{
  //digitalWrite(led1,HIGH);
  // providing enough warmup time for readings to stabilize

  //int reading = digitalRead(pwrPin);
  digitalWrite(13,state);
  if (digitalRead(pwrPin)){
    readingCounter++;
    Serial.print("Reading counter");
    Serial.println(readingCounter);

    /*powerOn determines whether power pin gets pressed
      Keep looping till the pin is set high
      Firt press -  Power On
      Second press - Power Off*/
    if (powerOn == 0){
      //powerOn = 1;
      if (readingCounter % 2 == 1){
        //powerTimer = millis();
        //Serial.println(powerTimer);
        //Serial.println(longPressActive);
        //Serial.println(millis() - powerTimer);
        digitalWrite(led1,HIGH);
        powerOn = 1;
      }
      else{
        //readingCounter = 0;
        digitalWrite(led1,LOW);
        powerOn = 0;
      }
    }
    else{
      powerOn = 0;
      //readingCounter++;
      Serial.println("Turn Off");
      digitalWrite(led1,LOW);
    }
    delay(500);
  }
  powerOn = 1;

  /*After power is on*/
  if (powerOn){
    digitalWrite(led1,HIGH);
    digitalWrite(solenoid_Pin,LOW);
    digitalWrite(led2,LOW);
    passed = 1;
    int values[15];

    /*Warm up  
      Keep track of every 15 sensor value
      The last of warm up is used as base value*/
    for (int i = 0; i < 15; i++){
      int mq3_value = 1023 - analogRead(mq3_analogPin);
      values[i] = mq3_value;
      Serial.print(values[i]);
      Serial.print("\tis\t");
      Serial.println(i);
      delay(100); //Just here to slow down the output.
      if ( i == 14){
        base_val = values[i];
      }
      timer = timer + 1;
    }

    /*After warm up, light up LED to cue user to breathe*/
    if (timer > WARMUP){
      Serial.println("Start");
      digitalWrite(led2,LOW);
      for (int i = 0; i < 5;i++){
        digitalWrite(led1,HIGH);
        delay(500);
        digitalWrite(led1,LOW);
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
        Serial.print("Testing val is");
        Serial.println(values[i]);
        if (values[i] > max_val){
          max_val = values[i];
        }
        if (i > -1){
          diff = values[i] - base_val;
          Serial.print("diff=");
          Serial.println(diff);
          if (diff > 160){
            digitalWrite(led1, HIGH);
            digitalWrite(solenoid_Pin,LOW);
            delay(3000);
            passed = 0;
            Serial.println("FAIL");
            break;
          }
        }
        delay(500);
      }
      if ((passed == 1) && ((max_val - base_val) > 20)){ 
        digitalWrite(solenoid_Pin,HIGH);
      }

      /*Resets all values*/
      for (int i = 0; i < 15; i++){
        values[i] = 0;
      }
      max_val = 0;
      base_val = 0;
      timer = 0;
      digitalWrite(led1,LOW);
      digitalWrite(led2,LOW);
      delay(3000);
    }
  }
}

void det_SW()
{
  /*Initiate GPRS*/
  /*
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
  */
  /*
  state = !state;
  Serial.print("Det Tampered\n");
  */
  //SendGPRS();
}

