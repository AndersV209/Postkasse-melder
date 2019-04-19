#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <SPI.h>

ADC_MODE(ADC_VCC);

 //MOSI: pin 11 don't use for anything
 //MISO: pin 12 to Q7 of last register
 //SCK:  pin 13 to CP
 // PL:  pin 2
 byte shift_1;

const char* SSID = "WIFI-SSID";
const char* PASS = "WIFI_password";

const String host_address  = "http://192.168.1.21:8080";
const String change_state_address = "/rest/items/";
const String mailbox_item_name = "Mailbox";
const String mailbox_voltage_item_name = "Mailbox_voltage";


double voltage = 0.0;

int httpCode_send = 0;

#define HOSTNAME "postkassemelder"     // Friedly hostname
//#define AP_NAME "ConfigAP-postmelder"   // Friedly Autoconfig ap name 

/*const char * const RST_REASONS[] =
{
    "REASON_DEFAULT_RST",       // normal startup by power on 
    "REASON_WDT_RST",           // hardware watch dog reset 
    "REASON_EXCEPTION_RST",     // exception reset, GPIO status won’t change 
    "REASON_SOFT_WDT_RST",      // software watch dog reset, GPIO status won’t change 
    "REASON_SOFT_RESTART",      // software restart ,system_restart , GPIO status won’t change 
    "REASON_DEEP_SLEEP_AWAKE",  // wake up from deep-sleep 
    "REASON_EXT_SYS_RST"        // external system reset 
};

*/


 
void setup(){
  Serial.begin(115200);


  //print reset reason
  Serial.println();
  Serial.println();
  //const rst_info * resetInfo = system_get_rst_info();
  //Serial.print(F("reset reason: "));
  //Serial.println(RST_REASONS[resetInfo->reason]);
  //Serial.println();
  //Serial.println();
  
  //
  
  delay(10);

  pinMode(4, OUTPUT);


  Serial.print("Connecting To ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASS);
    //WiFi.begin(SSID);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  
  // ***************************************************************************
  // Setup: MDNS responder
  // ***************************************************************************
  //String hostname(HOSTNAME);
  MDNS.begin(HOSTNAME);
  //Serial.print("Open http://");
  //Serial.print(HOSTNAME);
  //Serial.println(".local/edit to see the file browser");
  
  
  SPI.setFrequency(1000000);
  SPI.setClockDivider(SPI_CLOCK_DIV128);
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.begin();

  digitalWrite(4, HIGH); 
  shift_1 = SPI.transfer(0x00);
  Serial.println("");  
  Serial.print("Register 1 = ");
  Serial.println(shift_1,BIN);

  Serial.println("");  
  delay(10);

}


void loop(){
voltage = ESP.getVcc();
voltage = voltage/1000;

Serial.print("voltage: ");
Serial.println(voltage);
  
if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;  //Object of class HTTPClient
      http.begin(host_address + change_state_address + mailbox_item_name);
      

  if(shift_1== 0b00010001 || shift_1== 0b00010000){
    httpCode_send = http.POST("OFF");
  }

  else{
    Serial.println("mail switch on");
    httpCode_send = http.POST("ON");
  
  }

  //Check the returning code                                                                  
  if (httpCode_send > 0) {
    // Get the request response payload
    //String payload = http.getString();
    // TODO: Parsing
    //Serial.println("POST message succes");
          
          
    }
    http.end();   //Close connection

//send voltage
   http.begin(host_address + change_state_address + mailbox_voltage_item_name);
char temp[15] = "";
  dtostrf(voltage, 7, 3 , temp);
  sprintf(temp, temp, " V");
 // temp = temp + ' V';
    httpCode_send = http.POST(temp);
  

  //Check the returning code                                                                  
  if (httpCode_send > 0) {
    // Get the request response payload
    //String payload = http.getString();
    // TODO: Parsing
    //Serial.println("POST message succes");
          
          
    }
    http.end();   //Close connection
  }

  digitalWrite(4, LOW); 
  Serial.println("message send");





 
        
  Serial.println("sleep");
  ESP.deepSleep(0, WAKE_RF_DEFAULT);
      
  delay(1);



}
