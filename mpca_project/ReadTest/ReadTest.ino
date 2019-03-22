#include <TinyGPS.h>

#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

#include <SoftwareSerial.h>
SoftwareSerial BTserial(10, 11); // RX | TX

#define DEVICE (0x53)    //ADXL345 device address
#define TO_READ (6)        //num of bytes we are going to read each time (two bytes for each axis)
 
byte buff[TO_READ] ;    //6 bytes buffer for saving data read from the device
char str[512];
long lati,lon;
TinyGPS gps;
SoftwareSerial ss(4, 3);

void setup() {
  Serial.begin(9600);
  BTserial.begin(9600);
  
  Serial.print("Hello"); 
  ss.begin(9600); 
  while (!Serial) ; // wait for serial
  delay(200);
  Wire.begin();        // join i2c bus (address optional for master)
  //Turning on the ADXL345
 writeTo(DEVICE, 0x2D, 0);      
 writeTo(DEVICE, 0x2D, 16);
 writeTo(DEVICE, 0x2D, 8);
  Serial.println("DS1307RTC Read Test");
  Serial.println("-------------------");
}
int started=0;
int flag=0;
tmElements_t temp;
tmElements_t st;
tmElements_t journey;
time_t star;
time_t ends;
void loop() {
   bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;
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
  

    
  if (RTC.read(tm)) {
    if(abs(x-x1)>25 || abs(y-y1)>25 || abs(z-z1)>25){
       if(started==0){
          st=tm;
          //int counter=0;
          for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (ss.available())
    {
      char c = ss.read();
      // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }

  if (newData)
  {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    BTserial.print("LAT=");
    BTserial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    BTserial.print(" LON=");
    BTserial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    //BTserial.print(" SAT=");
    //BTserial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
    //BTserial.print(" PREC=");
    //BTserial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
  }
  
  gps.stats(&chars, &sentences, &failed);
  Serial.print(" CHARS=");
  Serial.print(chars);
  Serial.print(" SENTENCES=");
  Serial.print(sentences);
  Serial.print(" CSUM ERR=");
  Serial.println(failed);
  if (chars == 0)
    Serial.println("** No characters received from GPS: check wiring **");

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
        for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (ss.available())
    {
      char c = ss.read();
      // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }

  if (newData)
  {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    BTserial.write('\n');
    BTserial.print("LAT=");
    BTserial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    BTserial.print(" LON=");
    BTserial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    //BTserial.print(" SAT=");
    //BTserial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
    //BTserial.print(" PREC=");
    //BTserial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
  }
  
  gps.stats(&chars, &sentences, &failed);
  Serial.print(" CHARS=");
  Serial.print(chars);
  Serial.print(" SENTENCES=");
  Serial.print(sentences);
  Serial.print(" CSUM ERR=");
  Serial.println(failed);
  if (chars == 0)
    Serial.println("** No characters received from GPS: check wiring **");
        BTserial.write('\n');
        BTserial.print(tmYearToCalendar(st.Year));
        BTserial.write('-');
        BTserial.print(st.Month);
        BTserial.write('-');
        BTserial.print(st.Day);
        BTserial.write(' ');
        print2digits(st.Hour);
        BTserial.write(':');
        print2digits(st.Minute);
        BTserial.write(':');
        print2digits(st.Second);
        BTserial.print(", ");

        
        BTserial.print(tmYearToCalendar(temp.Year));
        BTserial.write('-');
        BTserial.print(temp.Month);
        BTserial.write('-');
        BTserial.print(temp.Day);
        BTserial.write(' ');
        print2digits(temp.Hour);
        BTserial.write(':');
        print2digits(temp.Minute);
        BTserial.write(':');
        print2digits(temp.Second);
        BTserial.print(", ");
        
        
        breakTime(makeTime(temp)-makeTime(st),journey);
        print2digits(journey.Hour);
        BTserial.write(':');
        print2digits(journey.Minute);
        BTserial.write(':');
        print2digits(journey.Second);
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
    BTserial.write('0');
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



