#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
SoftwareSerial BTserial(10, 11); // RX | TX

#define DEVICE (0x53)    //ADXL345 device address
#define TO_READ (6)        //num of bytes we are going to read each time (two bytes for each axis)

static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 4800;

byte buff[TO_READ] ;    //6 bytes buffer for saving data read from the device
char str[512];
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

void setup() {
  Serial.begin(9600);
  BTserial.begin(9600);
  ss.begin(GPSBaud); 
  while (!Serial) ; // wait for serial
  delay(200);
  Wire.begin();        // join i2c bus (address optional for master)
  //Turning on the ADXL345
 writeTo(DEVICE, 0x2D, 0);      
 writeTo(DEVICE, 0x2D, 16);
 writeTo(DEVICE, 0x2D, 8);
  Serial.println("DS1307RTC Read Test");
  Serial.print(F("Testing TinyGPS++ library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println("-------------------");
}
int started=0;
int flag=0;
tmElements_t temp;


void loop() {
  tmElements_t tm;
  int regAddress = 0x32; 
      //first axis-acceleration-data register on the ADXL345
  int x, y, z;
  int x1, y1, z1;
 
  readFrom(DEVICE, regAddress, TO_READ, buff); //read the acceleration data from the ADXL345
 
   //each axis reading comes in 10 bit resolution, ie 2 bytes.  Least Significat Byte first!!
   //thus we are converting both bytes in to one int
  x = (((int)buff[1]) << 8) | buff[0];   
  y = (((int)buff[3]) << 8) | buff[2];
  z = (((int)buff[5]) << 8) | buff[4];

  delay(500);
  readFrom(DEVICE, regAddress, TO_READ, buff); //read the acceleration data from the ADXL345
 
   //each axis reading comes in 10 bit resolution, ie 2 bytes.  Least Significat Byte first!!
   //thus we are converting both bytes in to one int
  x1 = (((int)buff[1]) << 8) | buff[0];   
  y1 = (((int)buff[3]) << 8) | buff[2];
  z1 = (((int)buff[5]) << 8) | buff[4];
 
  //we send the x y z values as a string to the serial port
  sprintf(str, "%d %d %d", x-x1, y-y1, z-z1);  
  Serial.print(str);
  Serial.write(byte(10));
  BTserial.print("x= ");
    BTserial.print(abs(x-x1));
    BTserial.print(" y= ");
    BTserial.print(abs(y-y1));
    BTserial.print(" z= ");
    BTserial.println(abs(z-z1));

    
  if (RTC.read(tm)) {
    if(abs(x-x1)>20){
      if(flag>40){
        if(started==0){
          temp=tm;
          if (gps.encode(ss.read()))
            displayInfo();
          else
          BTserial.print("Configure GPS");
        }
    Serial.print("Ok, Time = ");
    print2digits(temp.Hour);
    Serial.write(':');
    print2digits(temp.Minute);
    Serial.write(':');
    print2digits(temp.Second);
    Serial.print(", Date (D/M/Y) = ");
    Serial.print(temp.Day);
    Serial.write('/');
    Serial.print(temp.Month);
    Serial.write('/');
    Serial.print(tmYearToCalendar(temp.Year));
    Serial.println();
    

    
    BTserial.print("Journey Started, Time = ");
    print2digits(temp.Hour);
    BTserial.write(':');
    print2digits(temp.Minute);
    BTserial.write(':');
    print2digits(temp.Second);
    BTserial.print(", Date (D/M/Y) = ");
    BTserial.print(temp.Day);
    BTserial.write('/');
    BTserial.print(temp.Month);
    BTserial.write('/');
    BTserial.print(tmYearToCalendar(temp.Year));
    BTserial.println();
    started=1;
    flag=0;
    }
    else
      flag=0;
    }
    else{
      flag++;
      if(flag==1)
        RTC.read(temp);
      if(flag>40 && started==1){
        if (gps.encode(ss.read()))
          displayInfo();
        else
          BTserial.print("Configure GPS");
        BTserial.print("Ok, Time = ");
        print2digits(temp.Hour);
        BTserial.write(':');
        print2digits(temp.Minute);
        BTserial.write(':');
        print2digits(temp.Second);
        BTserial.print(", Date (D/M/Y) = ");
        BTserial.print(temp.Day);
        BTserial.write('/');
        BTserial.print(temp.Month);
        BTserial.write('/');
        BTserial.print(tmYearToCalendar(temp.Year));
        BTserial.println();
        started=0;
      }
    }
  } else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    delay(9000);
  }
  delay(10);
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
  BTserial.print(number);
}

void writeTo(int device, byte address, byte val) {
   Wire.beginTransmission(device); //start transmission to device 
   Wire.write(address);        // send register address
   Wire.write(val);        // send value to write
   Wire.endTransmission(); //end transmission
}
 
//reads num bytes starting from address register on device in to buff array
 
void readFrom(int device, byte address, int num, byte buff[]) 
{
  Wire.beginTransmission(device); //start transmission to device 
  Wire.write(address);        //sends address to read from
  Wire.endTransmission(); //end transmission
 
  Wire.beginTransmission(device); //start transmission to device
  Wire.requestFrom(device, num);    // request 6 bytes from device
 
  int i = 0;
  while(Wire.available())    //device may send less than requested (abnormal)
  { 
    buff[i] = Wire.read(); // receive a byte
    i++;
  }
  Wire.endTransmission(); //end transmission
}

void displayInfo()
{
  BTserial.print(F("Location: "));
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);

    BTserial.print(gps.location.lat(), 6);
    BTserial.print(F(","));
    BTserial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
    BTserial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}
