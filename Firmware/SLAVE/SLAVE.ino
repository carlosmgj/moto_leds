/* Ejemplo de comunicación WebSocket Servidor <---> Cliente. Escrito por Dani No www.esploradores.com

  Este sofware está escrito bajo la licencia CREATIVE COMMONS con Reconocimiento - CompartirIgual
  (CC BY-SA) https://creativecommons.org/

  -Redistributions of source code must retain the above creative commons and this list of conditions
  and the following disclaimer.
  -Redistributions in binary form must reproduce the above creative commons notice, this list of
  conditions and the following disclaimer in the documentation and/or other materials provided
  with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>
#include <espnow.h>

#include "Server.hpp"
#include "config.h"
#include "Css.hpp"

#define LED     LED_BUILTIN // D1
#define NEOPIXEL D2
#define NUMPIXELS 1

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, NEOPIXEL, NEO_GRB + NEO_KHZ800);

typedef struct message {
  String key;
  int value;
} message;
message myMessage;


ESP8266WebServer server (80);
WebSocketsServer webSocket = WebSocketsServer(81);

int breakR=155,breakG=0,breakB=0;
int NumLedsFreno = 10;
unsigned long lastFrenoActivado;
bool frenoactivado = false;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("[%u] Conectado a la URL: %d.%d.%d.%d - %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      inicializacionValoresWS();
      }
      break;

    case WStype_DISCONNECTED:
      Serial.printf("[%u] Desconectado!\n", num);
      break;

    case WStype_TEXT:
      String str = (char*)payload;
      String str_sub= str.substring(0,5);
      int commaIndex = str.indexOf(',');
      int secondCommaIndex = str.indexOf(',', commaIndex + 1);
      int thirdCommaIndex = str.indexOf(',', secondCommaIndex + 1);
      String firstValue = str.substring(0, commaIndex);
      String secondValue = str.substring(commaIndex + 1, secondCommaIndex);
      String thirdValue = str.substring(secondCommaIndex + 1,thirdCommaIndex);
      String fourthValue = str.substring(thirdCommaIndex + 1);
      Serial.println(str);
      if(firstValue=="FRENO"){
        cambioColorFreno(secondValue.toInt(),thirdValue.toInt(),fourthValue.toInt());
        webSocket.sendTXT(num, payload);
      }else if(firstValue=="VELOCIDAD"){
        Serial.print("VEL: ");
        Serial.println(secondValue.toInt());
        analogWrite(LED_BUILTIN,map(secondValue.toInt(),0,100,0,255));
        webSocket.sendTXT(num, payload);
      }else if(firstValue=="LEDSFRENO"){
        Serial.print("LEDS FRENO: ");
        Serial.println(secondValue.toInt());
        webSocket.sendTXT(num, payload);
      }else if(firstValue=="LOG"){
        Serial.print("MENSAJE RECIBIDO: ");
        Serial.println(str);
      }else if(firstValue=="ACTIVARFRENO"){
        String mostrar = String("boton")+","+String("1");
        webSocket.sendTXT(0, mostrar);
        pixels.setPixelColor(0, pixels.Color(breakR,breakG,breakB));
        pixels.show();
        frenoactivado=true;
        lastFrenoActivado=millis();
      }
      break;
  }
}

void onDataReceiver(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
   //Serial.println("Message received.");
   String mostrar = String(myMessage.key)+","+String(myMessage.value);
   webSocket.sendTXT(0, mostrar);
   memcpy(&myMessage, incomingData, sizeof(myMessage));
   Serial.print("KEY:");
   Serial.println(myMessage.key);
   Serial.print("VALUE:");
   Serial.println(myMessage.value);
   mostrarFreno(myMessage.value);
   
   
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  pixels.begin();

  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("IP del access point: ");
  Serial.println(myIP);

   // start webSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  // handle index
  server.on("/", []() {
      server.send_P(200, "text/html", INDEX_HTML);
  });

  server.begin();
  Serial.println("WebServer iniciado...");

  pinMode(LED, OUTPUT);
  digitalWrite(LED, 0);

  if (esp_now_init() != 0) {
    Serial.println("Problem during ESP-NOW init");
    return;
  }

  esp_now_register_recv_cb(onDataReceiver);
  analogWriteRange(255);
  delay(100);
  
}

void cambioColorFreno(int red, int green, int blue){
      breakR=red;
      breakG=green;
      breakB=blue;
      /*Serial.println(breakR);
      Serial.println(breakG);
      Serial.println(breakB);
      Serial.println(""); */     
}

void mostrarFreno(int mostrar)
{
  if (!mostrar){
   pixels.setPixelColor(0, pixels.Color(breakR,breakG,breakB));
   pixels.show();
  }else{
   pixels.setPixelColor(0, pixels.Color(0,0,0));
   pixels.show();
  }
}
void inicializacionValoresWS(){
   Serial.println("inicialización valores");
   String mostrar = "INIT"+String(",")+String(breakR)+","+String(breakG)+","+String(breakB)+","+String(NumLedsFreno);
   webSocket.sendTXT(0,mostrar);
}

void comprobarFreno(){
  if(frenoactivado){
    if(millis()-lastFrenoActivado>=2000)
      {
      frenoactivado=false;
      pixels.setPixelColor(0, pixels.Color(0,0,0));
      pixels.show();
      String mostrar = String("boton")+","+String("0");
      webSocket.sendTXT(0, mostrar);
      }
  }
}
void loop() {
    webSocket.loop();
    server.handleClient();
    comprobarFreno();
}
