//  PICK ONE of these serial ports:

// BEST: AltSoftSerial on pins 8 & 9 (on an UNO)
//#include <AltSoftSerial.h>
//static AltSoftSerial gpsPort;

static const int RXPin = 8, TXPin = 9;

// 2nd BEST: NeoSWSerial on any two pins
#include <NeoSWSerial.h>
static NeoSWSerial gpsPort(RXPin, TXPin);
static const int GPSBaud = 9600;

// WORST: SoftwareSerial NOT RECOMMENDED

static const bool useOLED = true;

#include <U8g2lib.h>


U8G2_PCF8812_96X65_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 3, /* data=*/ 4, /* cs=*/ 6, /* dc=*/ 5, /* reset=*/ 7);

static const int timezonehr = +1; //Timezone hour offset
static const int timezonemn = 00; //Timezone minute offset

#include <NMEAGPS.h>
static NMEAGPS         gps;
static gps_fix         fix;
static NeoGPS::time_t  localTime;

// Display: This section will contain elements for the OLED
static void draw(void) {
  // graphic commands to redraw the complete screen should be placed here


  //Separator (Vertical Line)
 // u8g2.drawLine(69,27,69,64);


  //GPS:: Checks if GPS has valid fix, then display selective elements
  if (fix.valid.location) {
//    u8g2.drawCircle(55,5,5, U8G2_DRAW_ALL); // GPS fix circle Top Right
//    u8g2.drawDisc(55,5,3, U8G2_DRAW_ALL); 
//  } else {
  u8g2.drawCircle(35,25,5, U8G2_DRAW_ALL); //csak kört rajzol
  }
  //u8g2.setFont(u8g_font_profont12);
  u8g2.setCursor(25, 30);
  u8g2.print( F(" SAT:") );
  if (fix.valid.satellites)
    u8g2.print( fix.satellites );
  

  if (fix.valid.time) {
    // TIME:: Time Elements including conversion of UTC to Local (IST +5.30)
    u8g2.setFont(u8g2_font_profont17_tr); // instead of profont22r?
    u8g2.setCursor(0, 14);

    if (localTime.hours < 10)
      u8g2.print( '0' );
    u8g2.print( localTime.hours );
    u8g2.print( ':' );
    if (localTime.minutes < 10)
      u8g2.print( '0' );
    u8g2.print( localTime.minutes );
    u8g2.print( ':' );
    if (localTime.seconds < 10)
      u8g2.print( '0' );
    u8g2.print( localTime.seconds );
  }
  
//  if (fix.valid.date) {
//    u8g2.print(' ');
//    if (localTime.date < 10)
//      u8g2.print('0');
//    u8g2.print(localTime.date);
//  }
  
  u8g2.setCursor(25, 11);

  if (fix.valid.speed) {
    //SPEED:: Displays Speed and other related graphics
    //u8g2.drawBox(0,20,(gps.speed.kmph()+0),8);
    float speed = fix.speed_kph();
    u8g2.drawFrame(2,59,48,4);
    u8g2.drawBox(2,59,(speed/2.5),4);
    
    if (speed > 100)
    {
      u8g2.drawBox(2,22,63,35);
      u8g2.setColorIndex(0);
      //u8g2.setFont(u8g_font_fur30n);
      u8g2.setCursor(4, 55);
      u8g2.print( (int)speed );
    }
    else
    {
      //u8g2.setFont(u8g_font_fur30n);
      u8g2.setCursor(0, 56);
      u8g2.print( (int)speed );
    }
    u8g2.setColorIndex(1);
    
    //u8g2.setFont(u8g_font_orgv01r);
    u8g2.setCursor(0, 22);
    //u8g2.print("SPEED >>");
    //u8g2.setFont(u8g_font_fur30n);
    u8g2.setCursor(0, 56);
    u8g2.print( (int)speed );
  
    //u8g2.setFont(u8g_font_orgv01r);
    u8g2.setCursor(53, 64);
    u8g2.print(F("KM"));
  }  

  //GPS DATA:: Other GPS related data and statistics
  //u8g2.setFont(u8g_font_profont10r);
 }



void setup() 
{
  Serial.begin(115200);
  gpsPort.begin(GPSBaud);
  u8g2.begin();

  Serial.println( F("NeoGPS display\n") );
}


void loop() 
{
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

    Serial.println(); 
  Serial.print( F(" GPS speed=") );
    if (fix.valid.speed) Serial.print(fix.speed()); 
//    Serial.print( F(" PDOP=")     );
//    if (fix.valid.pdop) Serial.print(fix.pdop * 0.001); 
    
    Serial.print( F(" GPS Mode=") );
    if (fix.valid.status) Serial.print(fix.status);
   
    Serial.print( F(" TIME=")     );
    if (fix.valid.time) Serial << localTime;
// Serial.print( F(" kmh=") );
//  Serial.print( (int)speed ); 
    
    if (useOLED) {
      // Update the display, all at once, during the GPS quiet time
      u8g2.firstPage();
      do{
        draw();
      } 
      while(u8g2.nextPage());
    }
  }
}
