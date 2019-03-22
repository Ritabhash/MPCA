//#include <LiquidCrystal.h> 
#include <SoftwareSerial.h> 
#include <TinyGPS.h> 
float lat,lon; // create variable for latitude and longitude object  
int year;
byte month, day, hour, minutes, second, hundredths;
unsigned long fix_age;
int flag = 0;
int counter = 0;
SoftwareSerial gpsSerial(3,4);//rx,tx 
TinyGPS gps; // create gps object 

void setup(){ 
  Serial.begin(9600); // connect serial  
  gpsSerial.begin(9600); // connect gps sensor 
} 

void loop(){ 
  while(gpsSerial.available()){ // check for gps data 
  if(gps.encode(gpsSerial.read()))// encode gps data 
  {  
    gps.f_get_position(&lat,&lon); // get latitude and longitude 
    //gps.crack_datetime(&year, &month, &day, &hour, &minutes,&second, &fix_age); 
    Serial.print("GPS Signal"); 
    Serial.print("Position: "); 
    Serial.print("Latitude:"); 
    Serial.print(lat,6); 
    Serial.print(" ; "); 
    Serial.print("Longitude:"); 
    Serial.println(lon,6); 
    Serial.print("Hour: ");
    Serial.println(hour,6); 
    Serial.print("Minute: ");
    Serial.println(minutes,6); 
    Serial.print("Second: ");
    Serial.println(second,6); 
    flag = 1;
 } 
} 
if (flag == 1)
  counter = counter+1;

if(counter >10)
  exit(0);
/*
String latitude = String(lat,6); 
String longitude = String(lon,6); 
Serial.println(latitude+" , "+longitude);
*/
delay(1000); 
} 
