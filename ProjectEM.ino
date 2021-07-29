#include <VS1053.h>
#include "EEPROM.h"
#define EEPROM_SIZE 32
unsigned int counter,old_counter,new_counter;

#include <WiFi.h>
#include <HTTPClient.h>

#include "SSD1306.h"
SSD1306  display(0x3c, 21,22);

char ssid[] = "Holidays";    //  your network SSID (name) 
char pass[] = "ihvdduh1";   // your network password

char *host[7] = {"radio12.plathong.net","fmone.plathong.net","radio11.plathong.net", "stream.pjcdn.com","62.210.10.4","icecast.omroep.nl","rstream.mcot.net"};
char *path[7] = {"/;stream.mp3","/;","/1261_192","/SuksonRadio","/stream2","/radio1-bb-mp3","/fm1005"};
int   port[7] = {8660,7010,8596,9000,8644,80,8000};

char *sname[7] = {"Cool Fahrenheit","FM ONE 103.5","Radiosbpac","MK Radio FM 105.50 MHz","bangkok.fm","icecast-NZ","FM 100.5 MHz"};

#define change 13
bool x=true;
int status = WL_IDLE_STATUS;
WiFiClient  client;
uint8_t mp3buff[32];   // vs1053 likes 32 bytes at a time

// Wiring of VS1053 board (SPI connected in a standard way)
#define VS1053_CS    32 //32
#define VS1053_DCS   33  //33
#define VS1053_DREQ  35 //15
#define VOLUME  100
VS1053 player(VS1053_CS, VS1053_DCS, VS1053_DREQ);
#define pot 34
int vol = 0;
int tim = 0;
int address = 0;
void setup () {
//Begin display
if(!EEPROM.begin(EEPROM_SIZE))
{
  Serial.println("FAIL");
  delay(1000);
  ESP.restart();
}
  display.init();
  display.setFont(ArialMT_Plain_16);
  display.normalDisplay();
  display.setColor(WHITE);
   pinMode(change,INPUT_PULLUP);
   Serial.begin(115200);
   delay(500);
   
   SPI.begin();
   
   player.begin();
   player.setVolume(100);
   display.display();
   WiFi.begin(ssid, pass);
   while (WiFi.status() != WL_CONNECTED) {
    display.drawString(0,15,"   Connecting..");
    delay(500);
    Serial.print(".");
   }
  display.clear();
  display.drawRect(-1,0,140,62);
  display.drawString(0,0,"  Internet Radio\n");
  display.setFont(ArialMT_Plain_10);
  Serial.println("WiFi connected");  
  Serial.print("IP address:");  
  Serial.println(WiFi.localIP());
  display.setColor(WHITE);
  String x1="Connected to:"+String(ssid);
  display.drawString(0,15,x1);
  x1="IP:"+WiFi.localIP().toString();
  display.drawString(0,27,x1);
  counter = EEPROM.read(address);
  old_counter=counter;
  
  String x3="Station:"+String(counter+1);
  String x4="Playing:"+String(sname[counter]);  //+ String(path[counter])+":"+String(port[counter]);  //.toString();
  display.drawString(0,37,x3);
  display.drawString(0,47,x4);
  display.display();
  x4="";
  Serial.printf("Current counter value: %u\n", counter);
  delay(100);
  station_connect(counter); 
}

void loop() {
      vol = analogRead(pot);
        //Serial.println(vol/41);
        player.setVolume(vol/41);
      if (client.available() > 0) 
      {
        uint8_t bytesread = client.read(mp3buff, 32);
        player.playChunk(mp3buff, bytesread);
      }
      
    if(digitalRead(change)== 0 and x==true)
    {
      x=false;
      old_counter = counter;
      counter = counter+1;
      if(counter>6) counter=0;
      EEPROM.writeUInt(address,counter);
      EEPROM.commit();
      new_counter=counter;
      
      Serial.printf("Now Streaming chanel : %u\n", counter);
      delay(500); 
  
      if(old_counter != new_counter) 
      {
        player.softReset(); 
        x=true;
        station_connect(new_counter);
        String x3="Station:"+String(counter+1);
        String x4="Playing:"+String(sname[counter]);  //+ String(path[counter])+":"+String(port[counter]);  //.toString();
        display.clear();
        display.setFont(ArialMT_Plain_16);
        display.setColor(WHITE);
        display.drawRect(-1,0,140,62); 
        display.drawString(0,0,"  Internet Radio\n");
        display.setFont(ArialMT_Plain_10);
        String x1="Connected to:"+String(ssid);
        display.drawString(0,15,x1);
        x1="IP:"+WiFi.localIP().toString();
        display.drawString(0,27,x1);
        display.drawString(0,37,x3);
        display.drawString(0,47,x4);
        
        display.display();
        delay(200);
      }
  } 
}

void station_connect (int station_no ) {
    if (client.connect(host[station_no],port[station_no]) ) {
    Serial.println("Connected now");
     }
    Serial.print(host[station_no]);
    Serial.println(path[station_no]);
    // This will send the request to the server
    client.print(String("GET ") + path[station_no] + " HTTP/1.1\r\n" +
               "Host: " + host[station_no] + "\r\n" + 
               "Connection: close\r\n\r\n");     
  }
