#include <SoftwareSerial.h>

SoftwareSerial BTserial(10, 11); // RX | TX
 String a;


void setup()
{
  Serial.begin(9600);
  BTserial.begin(9600);  
 
 
}
/*
 //cross-wire TX and RX
void loop()
{
  Serial.print(random(500));
 // Keep reading from arduino serial monitor and send to hc 05
  if (Serial.available())
    BTserial.print(Serial.read());
}
*/
void loop() { 
  
int flag = 1;//needs to be set when accelerometer reading shows a spike 

while(Serial.available()) {
if (flag==1){
a= Serial.readString();// read the incoming data as string

BTserial.println(a);
flag=0;
}

//now when the accelerometer reading becomes low, set flag as 1
}
    delay(40);

                              
}
