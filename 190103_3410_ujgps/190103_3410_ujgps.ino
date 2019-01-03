
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <U8g2lib.h>
//#define OLED_RESET 4

U8G2_PCF8812_96X65_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 3, /* data=*/ 4, /* cs=*/ 6, /* dc=*/ 5, /* reset=*/ 7);

static const int RXPin = 4, TXPin = 3;
static const int GPSBaud = 9600;
const int timezonehr = +5; //Timezone hour offset
const int timezonemn = 30; //Timezone minute offset
int hr;
int mn;

// The TinyGPS++ object
TinyGPSPlus gps;
// TinyGPSPlus Custom Elements
TinyGPSCustom pdop(gps, "GPGSA", 15); // $GPGSA sentence, 15th element
TinyGPSCustom hdop(gps, "GPGSA", 16); // $GPGSA sentence, 16th element
TinyGPSCustom vdop(gps, "GPGSA", 17); // $GPGSA sentence, 17th element
TinyGPSCustom GPSMode(gps, "GPGSA", 2); // GPSMode over Ground
TinyGPSCustom GPSHorPosError(gps, "GPRME", 15); // GPS Data Check
TinyGPSCustom Altitude(gps, "GPGGA", 9); // Antenna Altitude above mean Sea level

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

// Display This section will contain elements for the OLED
void draw(void) {
// graphic commands to redraw the complete screen should be placed here


  //Seperator (Vertical Line)
  u8g2.drawLine(69,27,69,64);


  //GPS:: Checks if GPS has valid fix, then display selective elements
  if (gps.location.isValid()){
  u8g2.drawCircle(122,5,5, u8g2_DRAW_ALL); // GPS fix circle Top Right
  u8g2.drawDisc(122,5,3, u8g2_DRAW_ALL); 
  }
  else{
  u8g2.drawCircle(122,5,5, u8g2_DRAW_ALL);
  }
  u8g2.setFont(u8g2_font_profont12);
  u8g2.setPrintPos(78, 9);
  u8g2.print(" SAT:"); u8g2.print(gps.satellites.value());
  

  //TIME:: Time Elements including conversion of UTC to Local (IST +5.30)
  u8g2.setFont(u8g2_font_profont22r);
  u8g2.setPrintPos(0, 14);
  hr = gps.time.hour();
  mn = gps.time.minute();
    mn = mn + timezonemn; //Add The Minute Time Zone Offset
    if (mn > 59) { //If The Minute Is Over 59 (From The Time Zone Conversion)
      mn = mn - 60; //Subtract 60 From It
      hr = hr + 1; //Then Add 1 To The Hour
    }
    else {
      if (mn < 0) { //If Minute Is Less Than 0, Do The Inverse Of Above
        mn = mn + 60;
        hr = hr - 1;
      }
    }
    hr = hr + timezonehr; //Add The Hour Time Zone Offset
    if (hr > 23) { //Do The Same Thing We Did Above
      hr = hr - 24;
    }
    else {
      if (hr < 0) {
        hr = hr + 24;
      }
    }
  if (hr < 10) u8g2.print('0'); u8g2.print(hr); u8g2.print(":"); if (mn < 10) u8g2.print('0'); u8g2.print(mn); u8g2.setFont (u8g2_font_courR08r); u8g2.print(""); if (gps.time.second() < 10) u8g2.print('0');u8g2.print(gps.time.second()); // u8g2.print(" "); if (gps.date.day() < 10) u8g2.print('0'); u8g2.print(gps.date.day());
  
  u8g2.setPrintPos(44, 11);


  //SPEED:: Displays Speed and other related graphics
  //u8g2.drawBox(0,20,(gps.speed.kmph()+0),8);
  u8g2.drawFrame(2,59,48,4);
  u8g2.drawBox(2,59,((gps.speed.kmph()+0)/2.5),4);
  
  if ((gps.speed.kmph()) > 100)
  {
  u8g2.drawBox(2,22,63,35);
  u8g2.setColorIndex(0);
  u8g2.setFont(u8g2_font_fur30n);
  u8g2.setPrintPos(4, 55);
  u8g2.print(gps.speed.kmph(),0);
  }
  else
  {
  u8g2.setFont(u8g2_font_fur30n);
  u8g2.setPrintPos(0, 56);
  u8g2.print(gps.speed.kmph(),0);
  }
  u8g2.setColorIndex(1);
  
  u8g2.setFont(u8g2_font_orgv01r);
  u8g2.setPrintPos(0, 22);
  //u8g2.print("SPEED >>");
  u8g2.setFont(u8g2_font_fur30n);
  u8g2.setPrintPos(0, 56);
  u8g2.print(gps.speed.kmph(),0);
  u8g2.setFont(u8g2_font_orgv01r);
  u8g2.setPrintPos(53, 64);
  u8g2.print("KM");
  

  //GPS DATA:: Other GPS related data and statistics
  u8g2.setFont(u8g2_font_profont10r);
  u8g2.setPrintPos(75, 34); u8g2.print("ACCU:");u8g2.print(hdop.value());
  u8g2.setPrintPos(75, 44); u8g2.print("ALT:"); u8g2.print(gps.altitude.meters()); u8g2.print("m");
  u8g2.setPrintPos(75, 54); u8g2.print("GPS MODE:"); u8g2.print(GPSMode.value());
  u8g2.setPrintPos(75, 64); u8g2.print("GPS ERR:"); u8g2.print(GPSHorPosError.value());

}


void setup() 
{
  Serial.begin(115200);
  ss.begin(GPSBaud);

  Serial.println(F("UsingCustomFields.ino"));
  Serial.println(F("Demonstrating how to extract any NMEA field using TinyGPSCustom"));
  Serial.print(F("Testing TinyGPS++ library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println(F("by Mikal Hart"));
  Serial.println();
  
}


void loop() 
{
 
  // Every time anything is updated, print everything.
  if (gps.altitude.isUpdated() || gps.satellites.isUpdated() ||
      pdop.isUpdated() || hdop.isUpdated() || vdop.isUpdated() || gps.speed.isUpdated() ||
      GPSMode.isUpdated() || GPSHorPosError.isUpdated() || Altitude.isUpdated())
  {
    Serial.println(F("")); 
    Serial.print(F("ALT="));   Serial.print(gps.altitude.meters()); 
    Serial.print(F(" PDOP=")); Serial.print(pdop.value()); 
    Serial.print(F(" HDOP=")); Serial.print(hdop.value()); 
    Serial.print(F(" VDOP=")); Serial.print(vdop.value());
    Serial.print(F(" SATS=")); Serial.print(gps.satellites.value());
    Serial.print(F(" Altitude=")); Serial.print(gps.altitude.meters());
    Serial.print(F(" GPS Mode=")); Serial.print(GPSMode.value());
    Serial.print(F(" GPS Data=")); Serial.print(GPSHorPosError.value());
    Serial.print(F(" TIME=")); Serial.print(hr); Serial.print(mn); Serial.print(gps.time.second());
  }

  while (ss.available() > 0)
    gps.encode(ss.read());

  
  // This code was repeated because the GPS data would otherwise take about a minute to update on OLED even without Mode 3 Fix.
  // I believe this forces the page to be recreated whenever a component is updated.
  if (gps.altitude.isUpdated() || gps.satellites.isUpdated() ||
      pdop.isUpdated() || hdop.isUpdated() || vdop.isUpdated() || gps.speed.isUpdated() ||
      GPSMode.isUpdated() || GPSHorPosError.isUpdated() || Altitude.isUpdated() || gps.time.isUpdated())
  {
  
    {
    u8g2.firstPage();
    do{
    draw();
    } 
    while(u8g2.nextPage());
   }
  }
}
