ESP8266WebServer server (80);
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
   <br>
   <canvas id="myCanvas2" width="250" height="20" style="border:1px solid #d3d3d3;">Tu navegador no soporta el elemento CANVAS de HTML5.</canvas>
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
        var r=200;
        var g=0;
        var b=0;
        var color = 'rgb('+r+','+g+','+b+' )';
        var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);

        ActualizarRuletaRojo();
        ActualizarRuletaVerde();
        ActualizarRuletaAzul();
        ActualizarColorCentral();
        ActualizarCursores();
        ApagarFreno();

        document.getElementById('ledsFreno').innerHTML = '15';

        connection.onopen = function () {
            connection.send('Conectado  -  ' + new Date());
            verValor();
        }

        connection.onmessage = function (event) {
            console.log('Servidor (recibe): ', event.data);
            connection.send("LOG"+","+event.data);
            if(event.data == 0){
                ApagarFreno();
            }else if(event.data == 1){
                EncenderFreno();
            }
        }

        connection.onerror = function (error) {
            console.log('WebSocket Error!!!', error);
        }

        function boton1() {
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
           ActualizarColorCentral();
           ActualizarCursores();
           enviarValorFreno()
        }

        function SliderRedChanged(){
           var valor = document.getElementById('miValorR').value;
           r=valor;
           ActualizarRuletaRojo();
           color='rgb('+r+','+g+','+b+' )';
           ActualizarColorCentral();
           enviarValorFreno();
        }

        function SliderGreenChanged(){
           var valor = document.getElementById('miValorG').value;
           g=valor;
           ActualizarRuletaVerde();
           color='rgb('+r+','+g+','+b+' )';
           ActualizarColorCentral();
           enviarValorFreno();
       }

       function SliderBlueChanged(){
          var valor = document.getElementById('miValorB').value;
          b=valor;
          ActualizarRuletaAzul();
          color='rgb('+r+','+g+','+b+' )';
          ActualizarColorCentral();
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

       function ActualizarCursores() {
          document.getElementById('miValorR').value=r;
          document.getElementById('miValorG').value=g;
          document.getElementById('miValorB').value=b;
       }

       function actualizarPicker() {
          document.getElementById('colorpicker').value=rgbToHex(r,g,b);
       }

       function ActualizarColorCentral(){
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

       function EncenderFreno(){
          var colorfreno='rgb('+r+','+g+','+b+' )';
          var canvas = document.getElementById('myCanvas2');
          var ctx = canvas.getContext('2d');
          ctx.beginPath();
          ctx.lineWidth = 40;
          ctx.strokeStyle = colorfreno;
          ctx.moveTo(100,0);
          ctx.lineTo(150,0);
          ctx.stroke();
       }

       function ApagarFreno(){
          var colorfreno='rgb(200,200,200)';
          var canvas = document.getElementById('myCanvas2');
          var ctx = canvas.getContext('2d');
          ctx.beginPath();
          ctx.lineWidth = 40;
          ctx.strokeStyle = colorfreno;
          ctx.moveTo(100,0);
          ctx.lineTo(150,0);
          ctx.stroke();
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

void InitServer()
{

  server.on("/", []() {
    server.send_P(200, "text/html", INDEX_HTML);
  });
	server.begin();
  Serial.println("WebServer iniciado...");
}
