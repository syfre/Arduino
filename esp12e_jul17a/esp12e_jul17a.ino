#include "ESP8266WiFi.h"
#include "Esp.h"

void setup() {
    Serial.begin(115200);

       
    WiFi.softAPdisconnect(true);
    WiFi.disconnect(true);
}
void loop() {}
