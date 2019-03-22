#include <SoftwareSerial.h>
SoftwareSerial BTserial(0,1);
//const int rxpin = 0;
//const int txpin = 1;
//Variable to store input value
//initialized with arbitrary value

//Connect the BTserial module
//SoftwareSerial BTserial(rxpin, txpin);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //Serial.write(c);
  //Serial.println(c);
  BTserial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
char c;
BTserial.print("hello");
delay(1000);
//Serial.println(random(500));
if(Serial.available())
{
  c=Serial.read();
  Serial.println(c);
  //blueTooth.write(c);
 
}
}




