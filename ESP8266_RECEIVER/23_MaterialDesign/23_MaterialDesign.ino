#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>
#include <espnow.h>

#include "config.h"  // Sustituir con datos de vuestra red
#include "Server.hpp"
#include "ESP8266_Utils.hpp"
#include "ESP8266_Utils_WS.hpp"
#include "WebSockets.hpp"

void setup(void)
{
	Serial.begin(115200);
	SPIFFS.begin();

  ConnectWiFi_AP(true);
  

  InitWebSockets();	
	InitServer();
}

void loop(void)
{
}
