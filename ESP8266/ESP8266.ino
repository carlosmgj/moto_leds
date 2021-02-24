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


#define LED     LED_BUILTIN // D1          
#define NEOPIXEL D2
#define NUMPIXELS 1

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, NEOPIXEL, NEO_GRB + NEO_KHZ800);

const char* ssid = "LED";
const char* password = "12345678";

static const char PROGMEM INDEX_HTML[] = R"(
<!DOCTYPE html>
<html>
  <head>                                                                             
      <meta charset=utf-8>                                                            
      <meta name='viewport' content='initial-scale=1, maximum-scale=1'>               
      <title>WebSocket ESP8266 - TOP CASE LEDS</title>
      <style>
        canvas {padding: 0; margin: auto; display: block;}
        body{text-align:center; margin:0 auto;}
      </style>
  </head>
  <body>
  <h1>CONTROL LUCES TOP CASE</h1>
  <h2>FRENO</h2>
  <label for="name">Número de LEDs:</label>
  <p id='ledsFreno'></p>
  <input type="text" id="textoLEDsFreno" required minlength="4" maxlength="8" size="10">
  <br> 
  <input type="button" value="Cambiar" onclick='boton1()'>
  <br><br>
  <label for="colorpicker">Seleccionar color -></label>
   <input type="color" id="colorpicker" value="#0000ff" onchange='PickerFrenoChanged()'>
   <br>
    <canvas id="myCanvas" width="250" height="250" style="border:0px solid #d3d3d3;">Tu navegador no soporta el elemento CANVAS de HTML5.</canvas>
    <b>ROJO   </b><input type='range' min='0' max='255' value='175' id='miValorR' oninput='SliderRedChanged()'><br>
    <b>VERDE   </b><input type='range' min='0' max='255' value='175' id='miValorG' oninput='SliderGreenChanged()'><br>
    <b>AZUL   </b><input type='range' min='0' max='255' value='175' id='miValorB' oninput='SliderBlueChanged()'><br>
    <h2>INTERMITENTES</h2>
    <input type='range' min='0' max='100' value='50' id='miValor' oninput='verValor()'>
    <p id='valor'></p>
    <script>
        var x;
        var r=10;
        var g=20;
        var b=20;
        var color;
        var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);

       
       ActualizarRuletaRojo();
       ActualizarRuletaVerde();
       ActualizarRuletaAzul();
       actualizarColorCentral();

       document.getElementById('ledsFreno').innerHTML = '15';
        
       connection.onopen = function () {
         connection.send('Conectado  -  ' + new Date()); 
         verValor();
       }
       connection.onmessage = function (event) {
         console.log('Servidor (recibe): ', event.data);
       }
       connection.onerror = function (error) {
         console.log('WebSocket Error!!!', error);
       }

       function boton1()
       {
        document.getElementById('ledsFreno').innerHTML = document.getElementById('textoLEDsFreno').value;
        connection.send("LEDSFRENO"+","+document.getElementById('textoLEDsFreno').value); 
       }
       
       function hexToRgb(hex) {
      var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
      return result ? {
      rr: parseInt(result[1], 16),
      gg: parseInt(result[2], 16),
      bb: parseInt(result[3], 16)
      } : null;
    }
        
      function rgbToHex(rr, gg, bb) {
      return "#" + ((1 << 24) + (r << 16) + (g << 8) + b).toString(16).slice(1);
    }

       
       function PickerFrenoChanged(){
       color= document.getElementById('colorpicker').value;
       r=hexToRgb(color).rr;
       g=hexToRgb(color).gg;
       b=hexToRgb(color).bb;
       ActualizarRuletaRojo();
       ActualizarRuletaVerde();
       ActualizarRuletaAzul();
       actualizarColorCentral();
       ActualizarCursores();   
       enviarValorFreno()    
       }
       
       
       function SliderRedChanged(){
        var valor = document.getElementById('miValorR').value;
        r=valor;
        ActualizarRuletaRojo();
        color='rgb('+r+','+g+','+b+' )';
        actualizarColorCentral();
        enviarValorFreno();
       }
      
       function SliderGreenChanged(){
        var valor = document.getElementById('miValorG').value;
        g=valor;
        ActualizarRuletaVerde();
        color='rgb('+r+','+g+','+b+' )';
        actualizarColorCentral();
        enviarValorFreno();
       }
       
       function SliderBlueChanged(){
        var valor = document.getElementById('miValorB').value;
        b=valor;
        ActualizarRuletaAzul();
        color='rgb('+r+','+g+','+b+' )';
        actualizarColorCentral();
        enviarValorFreno();
       }
       
       function ActualizarRuletaRojo() {
         var c = document.getElementById('myCanvas');
         var ctx = c.getContext('2d');
         ctx.clearRect(100, 200, 50, 25);
         ctx.beginPath();
         ctx.lineWidth = 20;
         ctx.strokeStyle = 'rgb(120,120,130)';
         ctx.arc(125,125,110,0.75*Math.PI,0.25*Math.PI);
         ctx.stroke();
         ctx.lineWidth = 20;
         ctx.beginPath();
         ctx.strokeStyle = 'rgb(255, 0,0 )'; 
         ctx.arc(125,125,110,0.75*Math.PI,(0.75+(1.5/255)*r)*Math.PI);
         ctx.stroke();
         ctx.font = 'bold 20px Arial'; 
         ctx.fillStyle = 'rgb(255, 0,0 )'; 
         ctx.textAlign = 'center';
         ctx.fillText(r,125, 215);
       }
       
       function ActualizarRuletaVerde() {
         var c = document.getElementById('myCanvas');
         var ctx = c.getContext('2d');
         ctx.clearRect(100, 175, 50, 25);
         ctx.beginPath();
         ctx.lineWidth = 20;
         ctx.strokeStyle = 'rgb(130,130,130)';
         ctx.arc(125,125,80,0.75*Math.PI,0.25*Math.PI);
         ctx.stroke();
         ctx.lineWidth = 20;
         ctx.beginPath();
         ctx.strokeStyle = 'rgb(0, 255,0 )'; 
         ctx.arc(125,125,80,0.75*Math.PI,(0.75+(1.5/255)*g)*Math.PI);
         ctx.stroke();
         ctx.font = 'bold 20px Arial'; 
         ctx.fillStyle = 'rgb(0, 255,0 )'; 
         ctx.textAlign = 'center';
         ctx.fillText(g,125, 195);
       }
              
       function ActualizarRuletaAzul() {
         var c = document.getElementById('myCanvas');
         var ctx = c.getContext('2d');
         ctx.clearRect(100, 160, 50, 15);
         ctx.beginPath();
         ctx.lineWidth = 20;
         ctx.strokeStyle = 'rgb(130,130,130)';
         ctx.arc(125,125,50,0.75*Math.PI,0.25*Math.PI);
         ctx.stroke();
         ctx.lineWidth = 20;
         ctx.beginPath();
         ctx.strokeStyle = 'rgb(0, 0,255 )'; 
         ctx.arc(125,125,50,0.75*Math.PI,(0.75+(1.5/255)*b)*Math.PI);
         ctx.stroke();
         ctx.font = 'bold 20px Arial'; 
         ctx.fillStyle = 'rgb(0, 0,255 )'; 
         ctx.textAlign = 'center';
         ctx.fillText(b,125, 175);
       }
       
       function ActualizarCursores()
       {
        document.getElementById('miValorR').value=r;
        document.getElementById('miValorG').value=g;
        document.getElementById('miValorB').value=b;
       }
       
       function actualizarPicker()
       {
         document.getElementById('colorpicker').value=rgbToHex(r,g,b);
       }
       
       function actualizarColorCentral(){
      var canvas = document.getElementById('myCanvas');
      var context = canvas.getContext('2d');
      var centerX = canvas.width / 2;
      var centerY = canvas.height / 2;
      var radius = 30;

      context.beginPath();
      context.arc(centerX, centerY, radius, 0, 2 * Math.PI, false);
      context.fillStyle = color;
      context.fill();
      context.lineWidth = 5;
      context.strokeStyle = color;
      context.stroke();
       }

      function verValor() {
         x = document.getElementById('miValor').value;
         document.getElementById('valor').innerHTML = 'Velocidad (0-100): ' + x; 
         connection.send("VELOCIDAD"+","+x);
       }
      
      function enviarValorFreno(){
         console.log('Cliente (envía): ' + r);
         connection.send("FRENO"+","+r+","+g+","+b);
       }
    </script>
  </body>
</html>
)";

ESP8266WebServer server (80);
WebSocketsServer webSocket = WebSocketsServer(81);

int breakR,breakG,breakB;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("[%u] Conectado a la URL: %d.%d.%d.%d - %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
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
      
      if(firstValue=="FRENO"){
        cambioColorFreno(secondValue.toInt(),thirdValue.toInt(),fourthValue.toInt());  
      }else if(firstValue=="VELOCIDAD"){
        Serial.print("VEL: ");
        Serial.println(secondValue.toInt());
        analogWrite(LED_BUILTIN,map(secondValue.toInt(),0,100,0,255));
      }else if(firstValue=="LEDSFRENO"){
        Serial.print("LEDS FRENO: ");
        Serial.println(secondValue.toInt());
      }
      break;
  }
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

  analogWriteRange(255);
}

void cambioColorFreno(int red, int green, int blue){
      breakR=red;
      breakG=green;
      breakB=blue;
      Serial.println(breakR);
      Serial.println(breakG);
      Serial.println(breakB);
      Serial.println("");
      pixels.setPixelColor(0, pixels.Color(breakR,breakG,breakB));
      pixels.show();
}

void loop() {
    webSocket.loop();
    server.handleClient();
}
