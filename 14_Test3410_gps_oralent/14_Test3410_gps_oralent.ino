
#include <Arduino.h>
#include <U8g2lib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GPS.h> //Using AdaFruit GPS lib
//#include <SoftwareSerial.h> //Serial com

//#include "RTClib.h"
#include <OneWire.h>
#include <DallasTemperature.h>
//#include "RTClib.h"  //Adafruit library https://github.com/adafruit/RTClib
//RTC_DS1307 RTC;




#define ONE_WIRE_BUS 39

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
U8G2_PCF8812_96X65_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 53, /* data=*/ 51, /* cs=*/ 47, /* dc=*/ 49, /* reset=*/ 45);  // Could be also PCF8814(53, 51, 47, 49, 45)

#include "grafik.h"
#include "graf2.h"
#include "vizlogo.h"

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

int BTNpin = 2;    //Button input
volatile int state = LOW;
float  tempC ; //ha van DS szenzor
//unsigned char  tempC = 80;   
 //float  tempCk = tempC; // a külső hő szenzor jön ide
char temperatureC[6];
//char dateBuffer[12];
int LCDfenyero = 250;    // how bright the LED is
int LCDkontrast = 10;
//int led = 9;           // the PWM pin the LED is attached to
//int brightness = 10;    // how bright the LED is
 //tempC = 94;    //ennyit mér a szonda how many points to fade the LED by 98 fok a max de 45-50 fokot jelenitünk meg tempC - 40 fok = rajzoltC
unsigned int rajzoltC  = (tempC - 98 ) * -1 ;// 100 fok -100= 0 tkarunk ekorát ki 50 fok-100= 50 et tkarunk ki *-1 hogy poztiv legyen
//GPS
//Serial1 mySerial(1, 0);     //RX and TX of GPS
//Serial1.begin(9600);
Adafruit_GPS GPS(&Serial1);   //Setting Siral
#define GPSECHO  false        //Define GPSecho, this prints the output to serial. 
boolean usingInterrupt = false;   //Define GPS Interrupt mode 
void useInterrupt(boolean);       //Define GPS Interrupt mode 

 
void draw1(void) {

u8g2.drawXBMP(0,0,Peugeotlogo_width,Peugeotlogo_height,Peugeotlogo_bits);

}

void draw2(void) {

  u8g2.drawXBMP(0,5,cross_width,cross_height,cross_bits);
} 

void draw3(void) {

  u8g2.drawXBMP(52,40,vizlogo_width,vizlogo_height,vizlogo_bits);
} 

void bar(void) {

 u8g2.drawBox(76, 13, 20, 40);      // bárt rajzolunk 41 fob majd a hőmérséklet függvényében változni

} 


void irjkiH_CH(){ 

u8g2.setDrawColor(1); /* color 1 for the box 0 az fehér kkirunk H és C betűt*/
//u8g2.drawBox(1, 1, 50, 64);
u8g2.setFont(u8g2_font_profont17_tr);

    u8g2.drawStr(81,12,"H");
    u8g2.drawStr(81,65,"C");


  
}

void kmora(){ 

u8g2.setDrawColor(1); /* color 1 for the box 0 az fehér kkirunk H és C betűt*/
//u8g2.drawBox(1, 1, 50, 64);
u8g2.setFont(u8g2_font_px437wyse700b_mn); 

    //u8g2.setFont(u8g2_font_helvB08_tf);  
u8g2.setCursor(0, 25);
u8g2.print( String((int)GPS.speed));
//u8g2.print("km/h");
  
}

void doboz(void) {
u8g2.setDrawColor(0); /* color 1 for the box 0 az fehér*/
u8g2.drawBox(76, 13, 20, ( rajzoltC) );
}

void takarjki(){ 
  // kitakarunk hogy szaggatott legyen a hőmérő oszlop
  u8g2.setDrawColor(0); /* color 1 for the box 0 az fehér*/
 u8g2.drawBox(76,20,20,2);
 u8g2.drawBox(76,30,20,2);
 u8g2.drawBox(76,40,20,2);
 u8g2.setDrawColor(0);
 
  
   

}

void ora(){ //ora és külső hő
  u8g2.enableUTF8Print();
  u8g2.setDrawColor(1); /* color 1 for the box 0 az fehér*/
  u8g2.setFont(u8g2_font_profont17_tr);
  u8g2.setCursor(0, 50);
   u8g2.print(String((GPS.hour) + 1) + ':' + String(GPS.minute));//ora kiirása  + ':' + String(GPS.seconds)
  //u8g2.print(dateBuffer);//ora kiirása
  //u8g2.setFont(u8g2_font_profont22_tr);
   u8g2.setCursor(0, 64);
   u8g2.print(tempC, 1); //1 tizedesig irjuk ki
//   u8g2.setCursor(55, 35);
//    //u8g2.print("C");

u8g2.setFont(u8g2_font_helvB08_tf);  
u8g2.setCursor(38, 64);
u8g2.print("°C");

  u8g2.setFont(u8g2_font_profont17_tr);
   u8g2.setCursor(76, 25);
   u8g2.print(tempC); // mért h)őfokot teszt idejére kiirom bele a barba
   

  
   

}
void setup(void) {
  Serial.begin(115200); // start serial port
  Serial1.begin(9600);   // start HW GPS serial port
  sensors.begin();
  //delay(500); 
  Wire.begin();
  //RTC.begin();
   //RTC.adjust(DateTime(__DATE__, __TIME__)); // elso hasznalat utan kikommentelni
//GPS
  GPS.begin(9600);                  //Starting GPS at a 9600 buad rate
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);     //Sending command for RMCGGA output: (recommended minimum) and GGA (fix data)
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);    //Sending command for 1Hz update rate
  GPS.sendCommand(PGCMD_ANTENNA);         //Setting External Antanna
  useInterrupt(true);               //Setting useInterrupt true true
  delay(100);                    //wait a sec for GPS to setup
  Serial1.println(PMTK_Q_RELEASE);          //?
  //Serial.println(PMTK_Q_RELEASE);
  
  {
     sensors.requestTemperatures();
      tempC = sensors.getTempCByIndex(0);
   
  
  //sensors.begin();    // Start up the temperature measurement library
}//--(end setup   

u8g2.begin();
u8g2.setContrast(50);
    u8g2.clearBuffer();
     draw1();                   //1 logo kirajzol
    u8g2.sendBuffer();
    delay(1000);          //1000              
    u8g2.clearBuffer();
     draw2();                   //2. logo kirajzol
    u8g2.sendBuffer();
    delay(1000);          //1000 


    sensors.begin();
    delay(50); 

}

void loop(void) {
 
 // DateTime now = RTC.now();  
  //sprintf(dateBuffer,"%02u:%02u ",now.hour(),now.minute());
 // Serial.println(dateBuffer);
 
  if (! usingInterrupt) {
    char c = GPS.read();     //Read GPS op Char
    if (GPSECHO)
      if (c) Serial.print(c);   
  }

  if (GPS.newNMEAreceived()) {        // If new Data is received
    if (GPS.parse(GPS.lastNMEA()))    // Parse the data
      return;
  }


  if(GPS.fix) //If there is a GPS FIX
  {
  u8g2.firstPage();
  do {
   
u8g2.setDrawColor(1);

   
  bar();
  draw3();
  irjkiH_CH();
  takarjki ();
  doboz ();
  irjkiH_CH();
  ora();
  kmora(); 
  } while ( u8g2.nextPage() );
  //sprintf(dateBuffer,"%02u:%02u ",now.hour(),now.minute());
  delay(10);
   //Serial.print(" C");
  //Serial.println("Time  " + String((GPS.hour) + 1) + ':' + String(GPS.minute) + ':' + String(GPS.seconds));
//Serial.println("Datum  20" + String(GPS.year) + '-' + String(GPS.month) + '-' + String(GPS.day));//GPS.day, GPS.month and GPS.year
  }
   //Serial.println("Time  " + String((GPS.hour) + 1) + ':' + String(GPS.minute) + ':' + String(GPS.seconds));
//   Serial.println("Sats: " + String((int)GPS.satellites));
//   Serial.println("Speed: " + String((int)GPS.speed*1.85));
//   Serial.println("homerseklet");
//  Serial.println(tempC, 2);    // "2" sets the number of decimal places

  
 // delay(100);
}
SIGNAL(TIMER0_COMPA_vect) {
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
#ifdef UDR0
  if (GPSECHO)
    if (c) UDR0 = c;  
  // writing direct to UDR0 is much much faster than Serial.print 
  // but only one character can be written at a time. 
#endif
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } 
  else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}
