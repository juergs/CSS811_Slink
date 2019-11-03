/***************************************************************************
  This is a library for the CCS811 air

  This sketch reads the sensor

  Designed specifically to work with the Adafruit CCS811 breakout
  ----> http://www.adafruit.com/products/3566

  These sensors use I2C to communicate. The device's I2C address is 0x5A

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Dean Miller for Adafruit Industries.
  BSD license, all text above must be included in any redistribution

  SimpleKalmanFilter(e_mea, e_est, q);
  e_mea: Measurement Uncertainty 
  e_est: Estimation Uncertainty 
  q: Process Noise

 ***************************************************************************/
 #include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
//---needed for library
//#include <DNSServer.h>
//#include <ESP8266WebServer.h>
#include <WiFiManager.h>
//#include  <WiFiUdp.h>
#include "Adafruit_CCS811.h"
#include "SimpleKalmanFilter.h"

//--- protypes
void sendUdpMessage(char* msg) ;
void buildUDPMessage(float measured_value, float estimated_value, float temperature ); 

#define use_slink true

Adafruit_CCS811 ccs;
SimpleKalmanFilter simpleKalmanFilter(1, 1, 0.01);

// UDP - Multicast declarations
WiFiUDP udp; 
IPAddress ipMulti(239, 255, 255, 250);    // site-local
unsigned int portMulti = 2085;            // port
char packetIn[255];                       // UDP in-buffer
char packetOut[512];                      // UPD out-buffer

//-------------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);

  delay(1000);

  Serial.println("CCS811 test");

  // fetches ssid and pass from eeprom and tries to connect
  // opens an AP (ESP+ChipID), enable WiFi setup at 192.168.4.1
  // reboots if credentials are set
  WiFiManager wifiManager;
  
  //--- uncomment and run it once, if you want to erase all the stored information
  // wifiManager.resetSettings();
  wifiManager.setTimeout(1); // TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  wifiManager.autoConnect();

  // restart watchdog as a safety measure. 
  // its unknown how much time we spend in connect - but the wdt triggers after 3.5 sec
  ESP.wdtFeed();
  ESP.wdtEnable(0);

  //Udp.begin(portMulti); 
  udp.begin(portMulti); 
  //DEBUG_PRINT("connected. device ip: " + WiFi.localIP());

  if(!ccs.begin()){
    Serial.println("Failed to start sensor! Please check your wiring.");
    while(1);
  }

  // Wait for the sensor to be ready
  while(!ccs.available());
}
//-------------------------------------------------------------------------------
void loop() 
{
  if(ccs.available())
  {
       float css811_tvoc = 0; 
       float temp = ccs.calculateTemperature();
       float measured_value = 0.0;
       float estimated_value = 0.0; 
    
      if(!ccs.readData())
      {  
        //--- add a noise to the reference value and use as the measured value    
        measured_value = ccs.geteCO2();
    
        //--- calculate the estimated value with Kalman Filter
        estimated_value = simpleKalmanFilter.updateEstimate(measured_value);

        css811_tvoc = ccs.getTVOC(); 

        //--- output configuration is fitting to Arduino-Plotter
        
        //Serial.print("CO2: ");
        Serial.print(measured_value);
        Serial.print(" "); 

        //Serial.print("CO2, filtered: ");
        Serial.print(estimated_value);
        Serial.print(" "); 
        
        //Serial.print("ppm, TVOC: ");
        Serial.print(css811_tvoc);
  
        //Serial.print(" ppb   Temp:");      
        Serial.print(" "); 
        
        Serial.println(temp);
        //Serial.println("");

        //--- send to FHEM 
        buildUDPMessage(measured_value, estimated_value, css811_tvoc, temp ); 
      }
      else
      {
        Serial.println("ERROR! Please find reason!");
        while(1);
      }
  }
  
  delay(2000);
  ESP.wdtFeed();
  delay(2000);
  ESP.wdtFeed();
  delay(1000);
  ESP.wdtFeed();
  
}
//-----------------------------------------------------------------------------
void sendUdpMessage(char* msg) 
{
  if (WiFi.status() == WL_CONNECTED && strlen(msg) != 0) 
  {
    snprintf(packetOut, sizeof(packetOut), "T:IAQC;FW:1.0;ID:%06X;IP:%s;R:%d;%s", ESP.getChipId(), WiFi.localIP().toString().c_str(), WiFi.RSSI(), msg);
    //DEBUG_PRINT(packetOut);
    udp.beginPacketMulticast( ipMulti, portMulti, WiFi.localIP() );
    udp.println(packetOut);
    udp.endPacket();
    strcpy(msg, "");
  };
};
//-----------------------------------------------------------------------------
void buildUDPMessage(float measured_value, float estimated_value, float tvoc, float temperature )
{
    //--- send UDP-Message to fhem 
    char cs811Msg[128];   
    char str_temp[6];
    char str_hum[6];
    char str_absHum[6];
    char str_dewPoint[6];
    char str_pressure[16];
    //char str_altitude[8];
    char str_tVoc[8];
    char str_gas[8];
    char str_r[6];
    char str_filtered[6]; 
    char str_ratio[6]; 
    char str_base[6];
                
    //--- using udp transmission (once every minute)
    // @fhem use these additional perl modules: https://github.com/herrmannj/AirQuality/tree/master/FHEM
    if (use_slink) 
    {
      dtostrf(temperature, 4, 2, str_temp);
      dtostrf(0, 4, 2, str_hum);
      dtostrf(0, 4, 2, str_absHum);
      dtostrf(0, 4, 2, str_dewPoint);
      dtostrf(0, 3, 1, str_pressure);
      dtostrf(measured_value, 3, 1, str_gas);
      dtostrf(estimated_value, 1, 0, str_tVoc);   
      dtostrf(measured_value, 1, 0, str_r);      //--- unused
      dtostrf(tvoc, 1, 0, str_filtered);  //--- unused   
      dtostrf(1.0, 1, 0, str_ratio);   //--- unused
      dtostrf(1.0, 1, 0, str_base);     //--- unused              
      
      //---  prepare to send UDP-message to fhem
      snprintf(cs811Msg
       , sizeof(cs811Msg)
       , "F:THPV;T:%s;H:%s;AH:%s;D:%s;P:%s;V:%s;R:%lu;DB:%lu;DF:%s;DR:%s;"
       , str_temp
       , str_hum
       , str_absHum
       , str_dewPoint
       , str_pressure
       , str_tVoc
       , str_r
       , str_base
       , str_filtered
       , str_ratio);

       sendUdpMessage(cs811Msg); 
       
    }
}
