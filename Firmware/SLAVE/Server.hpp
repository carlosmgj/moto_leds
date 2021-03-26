static const char PROGMEM INDEX_HTML[] = R"(
<!DOCTYPE html>
<html>
  <head>
      <meta charset=utf-8>
      <meta name='viewport' content='initial-scale=1, maximum-scale=1'>
      <title>WebSocket ESP8266 - TOP CASE LEDS</title>
      <style>
        canvas {padding: 0; margin: auto; display: block;}
        body{
          text-align:center;
          margin:0 auto;
          background-color: AntiqueWhite;
          }
         h1 {
          color: #c96f00;
          margin-bottom: 10px;
          font-family: helvetica;
          font-size: 40px;
          font-style: italic;
          letter-spacing: 2px;
          text-shadow: -1px 1px 0 #000,
                        1px 1px 0 #000,
                        1px -1px 0 #000,
                       -1px -1px 0 #000;
         }
         h2 {
          color: #aaa;
          margin-bottom: 5px;
          font-family: helvetica;
          font-size: 33px;
          font-style: italic;
          letter-spacing: 2px;
          text-shadow: -1px 1px 0 #000,
                        1px 1px 0 #000,
                        1px -1px 0 #000,
                       -1px -1px 0 #000;
         }         
        .button{
          background-color: #990033;
          border: none;
          color: white;
          padding: 7px 15px;
          text-align: center;
          cursor: pointer;
        }
        .button2{
          background-color: #000000;
          border: none;
          color: white;
          padding: 7px 15px;
          text-align: center;
          cursor: pointer;
        }
        .entradaTexto {
          border:  1px solid #39c;
          background-color: #eee;
          outline: none;
          width: 30px;
         }
         .slider {
            border-radius: 5px;
            cursor: pointer;
          }
      </style>
  </head>
  <body>
  <h1>CONTROL LUCES TOP CASE</h1>
  <br>
  <canvas id="myCanvas2" width="250" height="20" style="border:1px solid #d3d3d3;">Tu navegador no soporta el elemento CANVAS de HTML5.</canvas>
  <input class="button2" type="button" value="Probar Izquierdo" >
  <input class="button2" type="button" value="Probar Freno 2s" onmousedown='activarFreno()'>
  <input class="button2" type="button" value="Probar Derecho">
  <br>
  <label for="name">LEDs totales:</label>
  <a id='ledsTotales'></a>
  <br>
  <input type="text" class="entradaTexto" id="textoLEDsTotales" required minlength="4" maxlength="2" size="10"></input>
  <input class="button" type="button" value="Cambiar">
  <h2>FRENO</h2>
  <label for="name">LEDs en freno:</label>
  <a id='ledsFreno'></a>
  <br>
  <input type="text" class="entradaTexto" id="textoLEDsFreno" required minlength="4" maxlength="2" size="10"></input>
  <input class="button" type="button" value="Cambiar" onclick='boton1()'>
  <br><br>
  <label for="colorpicker">Seleccionar color: </label>
   <input type="color" id="colorpicker" value="#0000ff" onchange='PickerFrenoChanged()'>
   <br>
    <canvas id="myCanvas" width="250" height="250" style="border:0px solid #d3d3d3;">Tu navegador no soporta el elemento CANVAS de HTML5.</canvas>
    <b>ROJO   </b><input type='range'class="slider" min='0' max='255' value='175' id='miValorR' oninput='SliderRedChanged()'><br>
    <b>VERDE   </b><input type='range' min='0' max='255' value='175' id='miValorG' oninput='SliderGreenChanged()'><br>
    <b>AZUL   </b><input type='range' min='0' max='255' value='175' id='miValorB' oninput='SliderBlueChanged()'><br>
    <h2>INTERMITENTES</h2>
    <input type='range' min='0' max='100' value='50' id='miValor' oninput='verValor()'>
    <p id='valor'></p>
    <script>
        var x;
        var FrenoR=200;
        var FrenoG=0;
        var FrenoB=0;
        var color = 'rgb('+FrenoR+','+FrenoG+','+FrenoB+' )';
        var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);
        
        ApagarFreno();
        ApagarLeft();
        ApagarRight();
        
        connection.onopen = function () {
            connection.send('Conectado  -  ' + new Date());
        }

        connection.onmessage = function (event) {
            console.log('Servidor (recibe): ', event.data);
            connection.send("LOG"+","+event.data);
            var arrayParseado = event.data.split(',');
            if(arrayParseado[0].toString() == "boton"){
                if(arrayParseado[1] == 0){
                    ApagarFreno();
                }else if(arrayParseado[1] == 1){
                    EncenderFreno();
                }
            }else if(arrayParseado[0].toString() == "INIT"){
              FrenoR=arrayParseado[1];
              FrenoG=arrayParseado[2];
              FrenoB=arrayParseado[3];
              color='rgb('+FrenoR+','+FrenoG+','+FrenoB+' )';
              ActualizarRuletaRojo();
              ActualizarRuletaVerde();
              ActualizarRuletaAzul();
              ActualizarCursores();  
              ActualizarColorCentral();
              actualizarPicker();
              document.getElementById('ledsFreno').innerHTML = arrayParseado[4];            
            }
        }

        connection.onerror = function (error) {
            console.log('WebSocket Error!!!', error);
        }

        function boton1() {
           document.getElementById('ledsFreno').innerHTML = document.getElementById('textoLEDsFreno').value;
           connection.send("LEDSFRENO"+","+document.getElementById('textoLEDsFreno').value);
        }

        function activarFreno(){
          connection.send("ACTIVARFRENO");
        }
        
        function hexToRgb(hex) {
           var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
           return result ? {
             rr: parseInt(result[1], 16),
             gg: parseInt(result[2], 16),
             bb: parseInt(result[3], 16)
           } : null;
        }
        
        function rgbToHex  (rgb) { 
           var hex = Number(rgb).toString(16);
           if (hex.length < 2) {
           hex = "0" + hex;
        }
        return hex;
        };

        function fullColorHex (r,g,b) {   
          var red = rgbToHex(r);
          var green = rgbToHex(g);
          var blue = rgbToHex(b);
          return red+green+blue;
        };


        function PickerFrenoChanged(){
           color= document.getElementById('colorpicker').value;
           FrenoR=hexToRgb(color).rr;
           FrenoG=hexToRgb(color).gg;
           FrenoB=hexToRgb(color).bb;
           color='rgb('+FrenoR+','+FrenoG+','+FrenoB+' )';
           ActualizarRuletaRojo();
           ActualizarRuletaVerde();
           ActualizarRuletaAzul();
           ActualizarColorCentral();
           ActualizarCursores();
           enviarValorFreno()
        }

        function SliderRedChanged(){
           var valor = document.getElementById('miValorR').value;
           FrenoR=valor;
           ActualizarRuletaRojo();
           color='rgb('+FrenoR+','+FrenoG+','+FrenoB+' )';
           ActualizarColorCentral();
           actualizarPicker();
           enviarValorFreno();
        }

        function SliderGreenChanged(){
           var valor = document.getElementById('miValorG').value;
           FrenoG=valor;
           ActualizarRuletaVerde();
           color='rgb('+FrenoR+','+FrenoG+','+FrenoB+' )';
           ActualizarColorCentral();
           actualizarPicker();
           enviarValorFreno();
       }

       function SliderBlueChanged(){
          var valor = document.getElementById('miValorB').value;
          FrenoB=valor;
          ActualizarRuletaAzul();
          color='rgb('+FrenoR+','+FrenoG+','+FrenoB+' )';
          ActualizarColorCentral();
          actualizarPicker();
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
          ctx.arc(125,125,110,0.75*Math.PI,(0.75+(1.5/255)*FrenoR)*Math.PI);
          ctx.stroke();
          ctx.font = 'bold 20px Arial';
          ctx.fillStyle = 'rgb(255, 0,0 )';
          ctx.textAlign = 'center';
          ctx.fillText(FrenoR,125, 215);
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
          ctx.arc(125,125,80,0.75*Math.PI,(0.75+(1.5/255)*FrenoG)*Math.PI);
          ctx.stroke();
          ctx.font = 'bold 20px Arial';
          ctx.fillStyle = 'rgb(0, 255,0 )';
          ctx.textAlign = 'center';
          ctx.fillText(FrenoG,125, 195);
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
          ctx.arc(125,125,50,0.75*Math.PI,(0.75+(1.5/255)*FrenoB)*Math.PI);
          ctx.stroke();
          ctx.font = 'bold 20px Arial';
          ctx.fillStyle = 'rgb(0, 0,255 )';
          ctx.textAlign = 'center';
          ctx.fillText(FrenoB,125, 175);
       }

       function ActualizarCursores() {
          document.getElementById('miValorR').value=FrenoR;
          document.getElementById('miValorG').value=FrenoG;
          document.getElementById('miValorB').value=FrenoB;
       }

       function actualizarPicker() {
          document.getElementById('colorpicker').value="#"+fullColorHex(FrenoR,FrenoG,FrenoB);
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
          var colorfreno='rgb('+FrenoR+','+FrenoG+','+FrenoB+' )';
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

       function ApagarLeft(){
          var colorintermitente='rgb(200,200,200)';
          var canvas = document.getElementById('myCanvas2');
          var ctx = canvas.getContext('2d');
          ctx.beginPath();
          ctx.lineWidth = 40;
          ctx.strokeStyle = colorintermitente;
          ctx.moveTo(0,0);
          ctx.lineTo(90,0);
          ctx.stroke();
       }
       
        function ApagarRight(){
          var colorintermitente='rgb(200,200,200)';
          var canvas = document.getElementById('myCanvas2');
          var ctx = canvas.getContext('2d');
          ctx.beginPath();
          ctx.lineWidth = 40;
          ctx.strokeStyle = colorintermitente;
          ctx.moveTo(160,0);
          ctx.lineTo(250,0);
          ctx.stroke();
       }
       
       function verValor() {
          x = document.getElementById('miValor').value;
          document.getElementById('valor').innerHTML = 'Velocidad (0-100): ' + x;
          connection.send("VELOCIDAD"+","+x);
       }

       function enviarValorFreno(){
         console.log('Cliente (envía): ' + FrenoR);
         connection.send("FRENO"+","+FrenoR+","+FrenoG+","+FrenoB);
       }
    </script>
  </body>
</html>
)";
