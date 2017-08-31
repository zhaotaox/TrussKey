#include "GPRS.h"

/*GPRS module variables*/
char* smsMSG = "TrussKey is being tampered\032";
char* smsNum = "+14125195973";
CGPRS_SIM800 gprs;
uint32_t count = 0;
uint32_t errors = 0;
int smsSent;


void SendGPRS(){
  
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
  
  if (gprs.getOperatorName()) {
    /*
      con.print("Operator:");
      con.println(gprs.buffer);
      */
  }
  int ret = gprs.getSignalQuality();
  if (ret) {
    /*
      con.print("Signal:");
      con.print(ret);
      con.println("dB");*/
  }
  for (;;) {
    if (gprs.httpInit()) break;
    //con.println(gprs.buffer);
    gprs.httpUninit();
    delay(1000);
  }
  delay(3000);  
  char mydata[16];
  sprintf(mydata, "t=%lu", millis());
  smsSent = gprs.sendSMS(smsNum,smsMSG);
}
