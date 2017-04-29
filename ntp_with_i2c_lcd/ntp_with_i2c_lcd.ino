#include <Time.h> 

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#include <SPI.h>         
#include <Ethernet.h>
#include <EthernetUdp.h>

EthernetUDP Udp;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress timeServer(132, 163, 4, 101);  // time-a.timefreq.bldrdoc.gov NTP server
unsigned int localPort = 8888;           // local port to listen for UDP packets

time_t prevDisplay = 0;                  // when the digital clock was displayed
const  long timeZoneOffset = 9L;         // set this to the offset in seconds to your local time;

const int NTP_PACKET_SIZE = 48;          // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE];      // buffer to hold incoming and outgoing packets 

LiquidCrystal_I2C lcd(0x20, 16, 2);      // set the LCD address to 0x20 for a 16 chars and 2 line display


void digitalClockDisplay() {
  char strDateTime[17];

  memset(strDateTime, 0x00, sizeof(strDateTime));

  sprintf(strDateTime, "%04d-%02d-%02d %02d:%02d", year(), month(), day(), hour(), minute());
  Serial.println(strDateTime);
  lcd.print(strDateTime);
}

unsigned long getNtpTime() {
  sendNTPpacket(timeServer); // send an NTP packet to a time server

  delay(1000);  // wait to see if a reply is available

  if(Udp.parsePacket()) {  
    // We've received a packet, read the data from it
    Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord  = word(packetBuffer[42], packetBuffer[43]);  
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;  
    Serial.print("Seconds since Jan 1 1900 = " );
    Serial.println(secsSince1900);               

    // now convert NTP time into everyday time:
    Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;     
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;  
    // print Unix time:
    Serial.println(epoch);                               

    // print the hour, minute and second:
    Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
    Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)

    Serial.print(':');  
    if(((epoch % 3600) / 60) < 10) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      Serial.print('0');
    }

    Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
    Serial.print(':'); 
    if((epoch % 60) < 10) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.println(epoch %60); // print the second

    return epoch + (timeZoneOffset * 60 * 60);
  }
  
  return 0;
}

unsigned long sendNTPpacket(IPAddress& address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE); 
  // Initialize values needed to form NTP request
  packetBuffer[0] = 0b11100011;  // LI, Version, Mode
  packetBuffer[1] = 0;           // Stratum, or type of clock
  packetBuffer[2] = 6;           // Polling Interval
  packetBuffer[3] = 0xEC;        // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49; 
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp: 		   
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer,NTP_PACKET_SIZE);
  Udp.endPacket(); 
}

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("*Arduino dClock*");
  lcd.setCursor(0, 1);
  lcd.print("2013-01-01 12:00");

  // start Ethernet and UDP
  if(Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }

  Udp.begin(localPort);
  Serial.println("Time sync.....");
  setSyncProvider(getNtpTime);
  while(timeStatus() == timeNotSet)   
     ; // wait until the time is set by the sync provider
}

void loop() {
  if(now() != prevDisplay) {
    prevDisplay = now();
    lcd.setCursor(0, 1);
    digitalClockDisplay();
    delay(500);
    lcd.setCursor(13, 1);
    lcd.print(" ");
  }

  delay(500);
}

