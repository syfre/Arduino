/*



 */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <time.h>

/*
extern "C" {
#include <sntp.h>
extern char* sntp_asctime(const struct tm *t);
extern struct tm* sntp_localtime(const time_t *clock);
}
*/

unsigned long tick;
char ssid[] = "SYFRENETGEAR";  //  your network SSID (name)
char pass[] = "syfre92600";    // your network password


const char* ntpServerName = "time.nist.gov";


void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // from time.c
  //void configTime(int timezone_sec, int daylightOffset_sec, const char* server1, const char* server2, const char* server3)
  configTime(0, 0, "0.pool.ntp.org" , "1.pool.ntp.org", "2.pool.ntp.org");

  tick = 0;
}


void loop()
{
  // from time.c
  //time_t seconds = sntp_get_current_timestamp();
  //struct tm* p_tm = sntp_localtime(&seconds);
  //char* asc = sntp_asctime(p_tm);
  //Serial.println(asc);
  
  tick++;
  time_t now = time(nullptr);
  Serial.println(ctime(&now));

  // yes still ticking after disconnect
  //if (tick==20) {
  //  WiFi.disconnect();
  //  Serial.println("disconnected");
  //}
    
  delay(500);
}

