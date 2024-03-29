#include <Wire.h>
#include <RealTimeClockDS1307.h>

#define Display_Clock_Every_N_Seconds 10           // n.secs to show date/time
#define Display_ShortHelp_Every_N_Seconds 60       // n.secs to show hint for help
//#define TEST_Squarewave
//#define TEST_StopStart
//#define TEST_1224Switch

int count=0;
char formatted[] = "00-00-00 00:00:00x";

void setup() {
//  Wire.begin();
  Serial.begin(9600);
    pinMode(A5, OUTPUT);     //*** pin 16 (Analog pin 2) as OUTPUT   ***
  digitalWrite(A5, HIGH);   //*** pin 16 (Analog pin 2) set to LOW  ***
  pinMode(A4, OUTPUT);     //*** pin 17 (Analog pin 3) as OUTPUT   ***
  digitalWrite(A4, LOW);  //*** pin 17 (Analog pin 3) set to HIGH ***
  //*** Analog Pin settings to power RTC module ***
}
 
void loop() {
  if(Serial.available())
  {
    processCommand();
  }

  RTC.readClock();
  count++;
  if(count % Display_Clock_Every_N_Seconds == 0){
    Serial.print(count);
    Serial.print(": ");
    RTC.getFormatted(formatted);
    Serial.print(formatted);
    Serial.println();
  }
  
  if(count % Display_ShortHelp_Every_N_Seconds == 0) {
    Serial.println("Send ? for a list of commands.");
  }
#ifdef TEST_Squarewave
if(count%10 == 0)
{
  switch(count/10 % 6)
  {
    case 0:
    Serial.print("Squarewave disabled (low impedance): ");
    RTC.sqwDisable(0);
    Serial.println((int) RTC.readData(7));
    break;
    case 1:
    Serial.print("Squarewave disabled (high impedance): ");
    RTC.sqwDisable(1);
    Serial.println((int) RTC.readData(7));
    break;
    case 2:
    Serial.println("Squarewave enabled at 1 Hz");
    RTC.sqwEnable(RTC.SQW_1Hz);
    break;
    case 3:
    Serial.println("Squarewave enabled at 4.096 kHz");
    RTC.sqwEnable(RTC.SQW_4kHz);
    break;
    case 4:
    Serial.println("Squarewave enabled at 8.192 kHz");
    RTC.sqwEnable(RTC.SQW_8kHz);
    break;
    case 5:
    Serial.println("Squarewave enabled at 32.768 kHz");
    RTC.sqwEnable(RTC.SQW_32kHz);
    break;
    default:
    Serial.println("Squarewave test not defined");
  }//switch
}
#endif

#ifdef TEST_StopStart
if(count%10 == 0)
{
  if(!RTC.isStopped()) 
  {
    if(RTC.getSeconds() < 45) 
    {
      Serial.println("Stopping clock for 10 seconds");
      RTC.stop();
    }//if we have enough time
  } else {
    RTC.setSeconds(RTC.getSeconds()+11);
    RTC.start();
    Serial.println("Adding 11 seconds and restarting clock");
  }
}//if on a multiple of 10 counts
#endif

#ifdef TEST_1224Switch  
  if(count%10 == 0)
  {
    if(count %20 == 0)
    {
      Serial.println("switching to 12-hour time");
      RTC.switchTo12h();
      RTC.setClock();
    }
    else
    {
      Serial.println("switching to 24-hour time");
      RTC.switchTo24h();
      RTC.setClock();
    }
  }
#endif
}

void processCommand() {
  if(!Serial.available()) { return; }
  char command = Serial.read();
  int in,in2;
  switch(command)
  {
    case 'H':
    case 'h':
    in=SerialReadPosInt();
    RTC.setHours(in);
    RTC.setClock();
    Serial.print("Setting hours to ");
    Serial.println(in);
    break;
    case 'I':
    case 'i':
    in=SerialReadPosInt();
    RTC.setMinutes(in);
    RTC.setClock();
    Serial.print("Setting minutes to ");
    Serial.println(in);
    break;
    case 'S':
    case 's':
    in=SerialReadPosInt();
    RTC.setSeconds(in);
    RTC.setClock();
    Serial.print("Setting seconds to ");
    Serial.println(in);
    break;
    case 'Y':
    case 'y':
    in=SerialReadPosInt();
    RTC.setYear(in);
    RTC.setClock();
    Serial.print("Setting year to ");
    Serial.println(in);
    break;
    case 'M':
    case 'm':
    in=SerialReadPosInt();
    RTC.setMonth(in);
    RTC.setClock();
    Serial.print("Setting month to ");
    Serial.println(in);
    break;
    case 'D':
    case 'd':
    in=SerialReadPosInt();
    RTC.setDate(in);
    RTC.setClock();
    Serial.print("Setting date to ");
    Serial.println(in);
    break;
    case 'W':
    Serial.print("Day of week is ");
    Serial.println((int) RTC.getDayOfWeek());
    break;
    case 'w':
    in=SerialReadPosInt();
    RTC.setDayOfWeek(in);
    RTC.setClock();
    Serial.print("Setting day of week to ");
    Serial.println(in);
    break;
    
    case 't':
    case 'T':
    if(RTC.is12hour()) {
      RTC.switchTo24h();
      Serial.println("Switching to 24-hour clock.");
    } else {
      RTC.switchTo12h();
      Serial.println("Switching to 12-hour clock.");
    }
    RTC.setClock();
    break;
    
    case 'A':
    case 'a':
    if(RTC.is12hour()) {
      RTC.setAM();
      RTC.setClock();
      Serial.println("Set AM.");
    } else {
      Serial.println("(Set hours only in 24-hour mode.)");
    }
    break;
    
    case 'P':
    case 'p':
    if(RTC.is12hour()) {
      RTC.setPM();
      RTC.setClock();
      Serial.println("Set PM.");
    } else {
      Serial.println("(Set hours only in 24-hour mode.)");
    }
    break;

    case 'q':
    RTC.sqwEnable(RTC.SQW_1Hz);
    Serial.println("Square wave output set to 1Hz");
    break;
    case 'Q':
    RTC.sqwDisable(0);
    Serial.println("Square wave output disabled (low)");
    break;
    
    case 'z':
    RTC.start();
    Serial.println("Clock oscillator started.");
    break;
    case 'Z':
    RTC.stop();
    Serial.println("Clock oscillator stopped.");
    break;
    
    case '>':
    in=SerialReadPosInt();
    in2=SerialReadPosInt();
    RTC.writeData(in, in2);
    Serial.print("Write to register ");
    Serial.print(in);
    Serial.print(" the value ");
    Serial.println(in2);
    break;    
    case '<':
    in=SerialReadPosInt();
    in2=RTC.readData(in);
    Serial.print("Read from register ");
    Serial.print(in);
    Serial.print(" the value ");
    Serial.println(in2);
    break;

    default:
    Serial.println("Unknown command. Try these:");
    Serial.println(" h## - set Hours [range 1..12 or 0..24]");
  Serial.println(" i## - set mInutes [range 0..59]");
  Serial.println(" s## - set Seconds [range 0..59]");
  Serial.println(" d## - set Date [range 1..31]");
    Serial.println(" m## - set Month [range 1..12]");
    Serial.println(" y## - set Year [range 0..99]");
    Serial.println(" w## - set arbitrary day of Week [range 1..7]");
    Serial.println(" t   - toggle 24-hour mode");
    Serial.println(" a   - set AM          p   - set PM");
    Serial.println();
    Serial.println(" z   - start clock     Z   - stop clock");
    Serial.println(" q   - SQW/OUT = 1Hz   Q   - stop SQW/OUT");
    Serial.println();
    Serial.println(" >##,###  - write to register ## the value ###");
    Serial.println(" <##      - read the value in register ##");
    
  }//switch on command
  
}

//read in numeric characters until something else
//or no more data is available on serial.
int SerialReadPosInt() {
  int i = 0;
  boolean done=false;
  while(Serial.available() && !done)
  {
    char c = Serial.read();
    if (c >= '0' && c <='9')
    {
      i = i * 10 + (c-'0');
    }
    else 
    {
      done = true;
    }
  }
  return i;
}
