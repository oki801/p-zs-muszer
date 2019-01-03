
#include <Arduino.h>
#include <U8g2lib.h>
#include <Adafruit_GPS.h> //Using AdaFruit GPS lib
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h> //Serial com

#define ONE_WIRE_BUS 10 // ds külső szenzor port 
#define RXPin  9
#define TXPin  8
#define GPSBaud  9600
// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
U8G2_PCF8812_96X65_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 3, /* data=*/ 4, /* cs=*/ 6, /* dc=*/ 5, /* reset=*/ 7);  // Could be also PCF8814(53, 51, 47, 49, 45)

#include "grafik.h"
#include "graf2.h"
#include "vizlogo.h"
//#include "7segmentfont.h"
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);



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
u8g2.drawFrame(0, 0, 76, 38); // bekeretzzük a km mutatót+feliratát
} 


void irjkiH_CH(){ 

u8g2.setDrawColor(1); /* color 1 for the box 0 az fehér kkirunk H és C betűt*/
//u8g2.drawBox(1, 1, 50, 64);
u8g2.setFont(u8g2_font_profont17_tr);

    u8g2.drawStr(81,12,"H");
    u8g2.drawStr(81,65,"C");
   // u8g2.setFont(u8g2_font_helvB10_te);
   // u8g2.drawStr(28,38,"kmh");
u8g2.drawStr(62,14,"k");
u8g2.drawStr(62,23,"m"); 
u8g2.drawStr(62,35,"h"); 

}

void kmora(){ 

u8g2.setDrawColor(1); /* color 1 for the box 0 az fehér kkirunk H és C betűt*/
//u8g2.drawBox(1, 1, 50, 64);

//u8g2.setFont(u8g2_font_px437wyse700b_mn); 
//u8g2.setFont(u8g2_font_freedoomr25_tn); 
    //u8g2.setFont(u8g2_font_helvB08_tf);  
u8g2.setCursor(2, 32);
u8g2.print( tempC); // teszt miatt a +100

  
}

void doboz(void) {
u8g2.setDrawColor(0); /* color 1 for the box 0 az fehér*/
u8g2.drawBox(76, 13, 20, ( rajzoltC) );


}

void takarjki(){ // kitakarunk hogy szaggatott legyen a hőmérő oszlop
  u8g2.setDrawColor(0); /* color 1 for the box 0 az fehér*/
 u8g2.drawBox(76,20,20,2);
 u8g2.drawBox(76,30,20,2);
 u8g2.drawBox(76,40,20,2);
 u8g2.setDrawColor(0);
 
  
   

}

void ora(){ //ora és külső hő
  u8g2.enableUTF8Print();
  u8g2.setDrawColor(1); /* color 1 for the box 0 az fehér*/
  //u8g2.setFont(u8g2_font_profont17_tr);
  u8g2.setCursor(0, 50);
   //u8g2.print(String((GPS.hour) + 1) + ':' + String(GPS.minute));//ora kiirása  + ':' + String(GPS.seconds)
u8g2.print(tempC);//ora kiirása
  //u8g2.setFont(u8g2_font_profont22_tr);
   u8g2.setCursor(0, 64);
   u8g2.print(tempC, 1); //1 tizedesig irjuk ki
//   u8g2.setCursor(55, 35);
//    //u8g2.print("C");

//u8g2.setFont(u8g2_font_helvB08_tf);  
u8g2.setCursor(38, 64);
u8g2.print("°C");

  //u8g2.setFont(u8g2_font_profont17_tr);
   u8g2.setCursor(76, 25);
   u8g2.print(tempC); // mért h)őfokot teszt idejére kiirom bele a barba
   

  
   

}
void setup(void) {
  //Serial.begin(115200); // start pc serial port
  ss.begin(9600);   // start HW GPS serial port
  sensors.begin();
  //delay(500); 
  Wire.begin();
 

  
  
  {
     sensors.requestTemperatures();
      tempC = sensors.getTempCByIndex(0);
   
  

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
