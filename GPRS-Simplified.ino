#include <SoftwareSerial.h>

/*************************************************************************
* Test sketch for SIM800 library
* Distributed under GPL v2.0
* Written by Stanley Huang <stanleyhuangyc@gmail.com>
* For more information, please visit http://arduinodev.com
*************************************************************************/

#include "SIM800.h"


#define APN "connect"
//#define con Serial
static const char* url = "http://arduinodev.com/datetime.php";
char* smsMSG = "TrussKey is being tampered\032";
char* sM = "TrussKey okay\032";
char* smsNum = "+14125195973";
CGPRS_SIM800 gprs;
uint32_t count = 0;
uint32_t errors = 0;
int smsSent;

void setup()
{
  for (;;) {
    //con.print("Resetting...");
    while (!gprs.init()) {
      //con.write('.');
    }
    //con.println("OK");
    
    //con.print("Setting up network...");
    byte ret = gprs.setup(APN);
    if (ret == 0)
      break;
      /*
      con.print("Error code:");
      con.println(ret);
      con.println(gprs.buffer);*/
  }
  //con.println("OK");
  delay(3000);  
  for (;;) {
    if (gprs.httpInit()) break;
    //con.println(gprs.buffer);
    gprs.httpUninit();
    delay(1000);
  }
  delay(3000);
}

void loop()
{
  //smsSent = gprs.sendSMS(smsNum,smsMSG);
  char mydata[16];
  sprintf(mydata, "t=%lu", millis());
  smsSent = gprs.sendSMS(smsNum,smsMSG);

}
