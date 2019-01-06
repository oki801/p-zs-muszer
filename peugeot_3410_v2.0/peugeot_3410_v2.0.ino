
#include <Arduino.h>
#include <U8g2lib.h>
//#include <TinyGPS++.h>
#include <OneWire.h>
#include <DallasTemperature.h>
//#include <SoftwareSerial.h> //Serial com

#define ONE_WIRE_BUS1 11 // ds belső hő szenzor port 
#define ONE_WIRE_BUS2 10 // ds víz szenzor port 


#include <NMEAGPS.h>
static NMEAGPS         gps;
static gps_fix         fix;
static NeoGPS::time_t  localTime;

//TinyGPSPlus gps;
// The serial connection to the GPS device
static const int RXPin = 8, TXPin = 9;

#include <NeoSWSerial.h>// több memória marad
static NeoSWSerial gpsPort(RXPin, TXPin);
static const int GPSBaud = 9600; // gps kom sebessége


//#ifdef U8X8_HAVE_HW_SPI
//#include <SPI.h>
//#endif
//#ifdef U8X8_HAVE_HW_I2C
//#include <Wire.h>
//#endif

U8G2_PCF8812_96X65_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 3, /* data=*/ 4, /* cs=*/ 6, /* dc=*/ 5, /* reset=*/ 7);  // 3410 LCD 

static const int timezonehr = +1; //időzona korrekció 
static const int timezonemn = 00; //időzona korrekció perc
#include "grafik.h"
#include "graf2.h"
#include "vizlogo.h"

OneWire oneWire1(ONE_WIRE_BUS1);// 1. DS szenzor 
OneWire oneWire2(ONE_WIRE_BUS2); // 2. DS szenzor 
DallasTemperature sensor1(&oneWire1);
DallasTemperature sensor2(&oneWire2);


float  tempC ; //ha van DS szenzor
int tempC2 ;   //víz szenzor
unsigned int rajzoltC ; //amit rjzolunk bárt
 //float  tempCk = tempC; // a külső hő szenzor jön ide
//char temperatureC[6];
//char dateBuffer[12];
//int LCDfenyero = 250;    // how bright the LED is
//int LCDkontrast = 10;
//int led = 9;           // the PWM pin the LED is attached to
//int brightness = 10;    // how bright the LED is
//int tempC = 94 ;    //ennyit mér a szonda how many points to fade the LED by 98 fok a max de 45-50 fokot jelenitünk meg tempC - 40 fok = rajzoltC
//unsigned int rajzoltC  = (tempC2 - 98 ) * -1 ;// 100 fok -100= 0 tkarunk ekorát ki 50 fok-100= 50 et tkarunk ki *-1 hogy poztiv legyen


 
static void draw1(void) {

u8g2.drawXBMP(0,0,Peugeotlogo_width,Peugeotlogo_height,Peugeotlogo_bits);

}

static void draw2(void) {

  u8g2.drawXBMP(0,5,cross_width,cross_height,cross_bits);
} 

static void draw3(void) {

  u8g2.drawXBMP(52,40,vizlogo_width,vizlogo_height,vizlogo_bits);
} 

static void bar(void) {

 u8g2.drawBox(76, 13, 21, 40);      // bárt rajzolunk 41 fob majd a hőmérséklet függvényében változni
//u8g2.drawFrame(0, 0, 76, 38); // bekeretzzük a km mutatót+feliratát
} 


static void irjkiH_CH(){ 


u8g2.setDrawColor(1); /* color 1 for the box 0 az fehér kkirunk H és C betűt*/
u8g2.drawLine(71, 0, 71, 34);//függő vonal
u8g2.drawLine(0, 34, 71, 34);//viz vonal
u8g2.setFont(u8g2_font_profont17_tr);

    u8g2.drawStr(81,12,"H");
    u8g2.drawStr(81,65,"C");
   //u8g2.setFont(u8g2_font_helvB10_te);
   
   u8g2.drawStr(43,31,"kmh");
//u8g2.drawStr(62,14,"k");
//u8g2.drawStr(62,23,"m"); 
//u8g2.drawStr(62,35,"h"); 

}
static void st(void) {//idő és sebességmérés kijelzése
  // graphic commands to redraw the complete screen should be placed here


  //Separator (Vertical Line)
 // u8g2.drawLine(69,27,69,64);


  //GPS:: Checks if GPS has valid fix, then display selective elements
  if (fix.valid.location) {

 // u8g2.drawCircle(35,25,5, U8G2_DRAW_ALL); //csak kört rajzol
  }
  
//  u8g2.setCursor(25, 30);
//  u8g2.print( F(" SAT:") );
  if (fix.valid.satellites)
    u8g2.print( fix.satellites );
  

  if (fix.valid.time) {
    // TIME:: Time Elements including conversion of UTC to Local (IST +5.30)
    u8g2.setFont(u8g2_font_profont17_tr); // instead of profont22r?
    u8g2.setCursor(0, 50);

    if (localTime.hours < 10)
      u8g2.print( '0' );
    u8g2.print( localTime.hours );
    u8g2.print( ':' );
    if (localTime.minutes < 10)
      u8g2.print( '0' );
    u8g2.print( localTime.minutes );
//    u8g2.print( ':' );
//    if (localTime.seconds < 10)
//      u8g2.print( '0' );
//    u8g2.print( localTime.seconds );
  }
  

  
  //u8g2.setCursor(25, 11);

  if (fix.valid.speed) {
    //SPEED:: Displays Speed and other related graphics
    //u8g2.drawBox(0,20,(gps.speed.kmph()+0),8);
    int speed = fix.speed_kph();
//    u8g2.drawFrame(2,59,48,4);
//    u8g2.drawBox(2,59,(speed/2.5),4);
//    
    if (speed < 5) 
    


{
     speed = 0;
   }
   
  //  {
    //  u8g2.drawBox(2,22,63,35);
   //   u8g2.setColorIndex(0);
      //u8g2.setFont(u8g_font_fur30n);
   //   u8g2.setCursor(4, 55);
    //  u8g2.print( (int)speed );
  //  }
  //  else
 //   {
      //u8g2.setFont(u8g_font_fur30n);
    //  u8g2.setCursor(0, 56);
    //  u8g2.print( (int)speed );
   // }
   // u8g2.setColorIndex(1);
    
    //u8g2.setFont(u8g_font_orgv01r);
    //u8g2.setCursor(0, 22);
    //u8g2.print("SPEED >>");
    //u8g2.setFont(u8g_font_fur30n);
    u8g2.setFont(u8g2_font_freedoomr25_tn); //nagy font
    u8g2.setCursor(2, 32);
    //u8g2.print(( (int)speed )+88); // 88-at törölni használat előtt
    u8g2.print( (int)speed ); // 88-at törölni használat előtt
  
    //u8g2.setFont(u8g_font_orgv01r);
//    u8g2.setCursor(23, 50);
//    u8g2.print(F("KM"));
  }  

  //GPS DATA:: Other GPS related data and statistics
  //u8g2.setFont(u8g_font_profont10r);
 }
//static void kmora(){ 
//
//u8g2.setDrawColor(1); /* color 1 for the box 0 az fehér kkirunk H és C betűt*/
////u8g2.drawBox(1, 1, 50, 64);
//
////u8g2.setFont(u8g2_font_px437wyse700b_mn); 
////u8g2.setFont(u8g2_font_freedoomr25_tn); 
//    //u8g2.setFont(u8g2_font_helvB08_tf);  
//u8g2.setCursor(2, 32);
////u8g2.print(gps.speed.kmph());  // teszt miatt a +100 sebesség 
//
//
//  
//}

static void doboz(void) {
  rajzoltC  = (tempC2 - 98 ) * -1 ;// 100 fok -100= 0 tkarunk ekorát ki 50 fok-100= 50 et tkarunk ki *-1 hogy poztiv legyen
u8g2.setDrawColor(0); /* color 1 for the box 0 az fehér*/
u8g2.drawBox(76, 13, 20, ( rajzoltC ));


}

static void takarjki(){ // kitakarunk hogy szaggatott legyen a hőmérő oszlop
  u8g2.setDrawColor(0); /* color 1 for the box 0 az fehér*/
 u8g2.drawBox(76,20,20,2);
 u8g2.drawBox(76,30,20,2);
 u8g2.drawBox(76,40,20,2);
 u8g2.setDrawColor(0);
 
  
   

}

void ora(){ // külső hő szenzor
  u8g2.enableUTF8Print();
  u8g2.setDrawColor(1); /* color 1 for the box 0 az fehér*/
  //u8g2.setFont(u8g2_font_profont17_tr);
  u8g2.setCursor(0, 50);
  //u8g2.print(sz);
//   u8g2.print( sprintf(sz, "%02d:%02d:%02d ", t.hour()+1, t.minute(), t.second());
//u8g2.print(tempC);//ora kiirása
  //u8g2.setFont(u8g2_font_profont22_tr);
   u8g2.setCursor(0, 64);
   u8g2.print(tempC, 1); //1 tizedesig irjuk ki
//   u8g2.setCursor(55, 35);
//    //u8g2.print("C");

//u8g2.setFont(u8g2_font_helvB08_tf);  
u8g2.setCursor(39, 64);
u8g2.print("°C");

  //u8g2.setFont(u8g2_font_profont17_tr);
//   u8g2.setCursor(76, 25);
//   u8g2.print(tempC); // mért h)őfokot teszt idejére kiirom bele a barba
  

}



void setup(void) {
   Serial.begin(115200);
  gpsPort.begin(GPSBaud);
 // u8g2.begin();
 // sensors.begin();
  //delay(500); 
 // Wire.begin();
 
//}
  
  
 

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


    sensor1.begin();
      sensor2.begin();
    delay(50); 

}

void loop(void){
  if (gps.available( gpsPort )) {
    fix = gps.read(); // get the entire, assembled fix structure

    // Shift the date/time to local time
    NeoGPS::clock_t localSeconds;
    {
      using namespace NeoGPS; // save a little typing below...

      localSeconds  = (clock_t) fix.dateTime;  // convert structure to a second count
      localSeconds +=
      timezonehr * SECONDS_PER_HOUR + 
      timezonemn * SECONDS_PER_MINUTE;       // shift to desired timezone
      localTime     = localSeconds;            // convert back to a structure
    }
 {
     sensor1.requestTemperatures();
      sensor2.requestTemperatures();
      //tempC = sensors.getTempCByIndex(0);
   //unsigned int rajzoltC  = (tempC2 - 98 ) * -1 ;// 100 fok -100= 0 tkarunk ekorát ki 50 fok-100= 50 et tkarunk ki *-1 hogy poztiv legyen
  

}//--(end setup   
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
            st (); 
tempC = sensor1.getTempCByIndex(0);
tempC2 = sensor2.getTempCByIndex(0);   
Serial.println (tempC);  
Serial.println (tempC2);  
Serial.println (rajzoltC);             
    } while ( u8g2.nextPage()) ;
    
  
  //sprintf(dateBuffer,"%02u:%02u ",now.hour(),now.minute());
  //delay(10);
  
   //Serial.print(" C");
  //Serial.println("Time  " + String((GPS.hour) + 1) + ':' + String(GPS.minute) + ':' + String(GPS.seconds));
//Serial.println("Datum  20" + String(GPS.year) + '-' + String(GPS.month) + '-' + String(GPS.day));//GPS.day, GPS.month and GPS.year
  }
  } 



 
