#include <Wire.h>
#include <WiFi.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define RELAY_PIN 4
#define POTE_PIN 32
#define BUTTON_PIN 19
#define LEDV_PIN 26
//#define LEDA_PIN 33
#define DSCL_PIN 22
#define DSDA_PIN 21
#define DHTTYPE DHT22 
#define DHTPIN 33

WiFiServer server(80);

const char* ssid = "Wokwi-GUEST";//Poner wifi a usar
const char* password = "";

String ledIntegrado="Apagado";
String estadoRelay="Apagado";
String header;
String temp;
String hum;
String message="";
String intensidadLed ="0";
float porcentaje;
float t;
float h;
String pagina = "<!DOCTYPE html>"
"<html lang='en'>"
"<head>"
    "<meta charset='UTF-8'>"
    "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
    "<link href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css' rel='stylesheet' integrity='sha384-QWTKZyjpPEjISv5WaRU9OFeRpok6YctnYmDr5pNlyT2bRjXh0JMhjY6hW+ALEwIH' crossorigin='anonymous'>"
    "<title>Document</title>"
    "<style>"
        ".display,button[type=button]{border:1px solid #000;box-shadow:0 2px 5px rgba(2,2,2,.699)}.container{background-color:#0099bc}.titulo{text-shadow:0 2px 5px rgba(2,2,2,.699)}.caja1-a{display:flex;align-content:center}.display_interno p{padding:0;margin:0}.display_fuera{flex-grow:1}button[type=button]{color:#000;background-color:transparent}.display{background-color:#00b7c3;border-radius:20px}.display_interno{color:#2e2e2e;background-color:#a7e1e5;border-radius:10px;border:1px solid #000}"
    "</style>"
    "<script>"
        "function mostrarValor(e){document.getElementById('valorSeleccionado').textContent='intencidad al: '+e+' %'}function mostrarMensaje(){var e=document.getElementById('message').value;document.getElementById('displayMessage').textContent=e}"
    "</script>"
"</head>"
"<body>"
    "<div class='titulo display-4 text-center'>CONTROL ESP32</div>"
    "<div class='container d-flex justify-content-center  pt-4'>"
        "<div  class='caja'>"
            "<div class='caja1-a'>"
                "<div class='display mx-2 p-2 text-center'>"
                    "<h5>Led Integrado</h5>"
                    "<a href='/ledOn'><button type='button' class='btn btn-primary w-50 my-2'>ON</button></a>"
                    "<a href='/ledOff'><button type='button' class='btn btn-primary w-50 my-2'>OFF</button></a>"
                    "<p class='text-white'>"+ ledIntegrado +"</p>"
                "</div>"
                "<div class='display mx-2 p-2 text-center'>"
                    "<h5 class='pb-2'>Temperatura</h5>"
                    "<input class='w-75 text-end pe-2' type='text' name='temperature' id='tempValue' value='"+ temp + "C' disabled>"
                    "<div class='py-2'></div>"
                    "<h5 class='pb-2'>Humedad</h5>"
                    "<input class='w-75 text-end pe-2' type='text' name='humedad' id='humedadValue' placeholder=' " + hum + "%' disabled>"
                "</div>"
            "</div>"
            "<div class='display m-2 p-2 text-center'>"
                "<h5>Led Verde</h5>"
                "<a href='/ledVa'><button class='display btn btn-outline-primary' type='button' >0%</button></a>"
                "<a href='/ledVb'><button class='display btn btn-outline-primary' type='button' >25%</button></a>"
                "<a href='/ledVc'><button class='display btn btn-outline-primary' type='button' >50%</button></a>"
                "<a href='/ledVd'><button class='display btn btn-outline-primary' type='button' >75%</button></a>"
                "<a href='/ledVe'><button class='display btn btn-outline-primary' type='button' >100%</button></a>"
                "<h6 class='py-1 my-1'>intensidad led Verde: "+ intensidadLed +" %</h6>"
            "</div>"
            "<div class='display d-flex flex-column m-2 p-2 align-items-center'>"
                "<h5>Potenciometro</h5>"
                "<div class='progress w-75'>"
                    "<div class='progress-bar ' role='progressbar' style='width: 50%' aria-valuenow='"+ porcentaje +"' aria-valuemin='0' aria-valuemax='100'><p class=' d-flex justify-content-end align-items-center m-0 text-dark fw-bold'>"+ String (porcentaje) +"%</p></div>"
                "</div>"
            "</div>"
            "<div class='display d-flex m-2 p-2 justify-content-around'>"
              "<form action='/' method='GET'>"
                "<input type='text' id='message' name='texto' placeholder='Ingrese texto'><br><br>"
                "<input type='submit'  value='Enviar'>"
                "</form>"    
            "</div>"
        "</div>"
        "<div  class='caja d-flex flex-column'>"
            "<div class='display mx-2 p-2 text-center'>"
                "<h5>Relé</h5>"
                    "<a href='/releActivar'><button type='button' class='btn btn-primary  w-50 my-2 text-uppercase'>activar</button></a>"
                    "<a href='/releDesactivar'><button type='button' class='btn btn-primary  min-vw-50 my-2 text-uppercase'>desactivar</button></a>"
                    "<p class='text-white'>"+ estadoRelay +"</p>"
            "</div>"
            "<div class='display display_fuera m-2 p-1 text-center'>"
                "<h5>Display</h5>"
                "<div class='display_interno mx-2 ps-2 text-start'>"
                    "<p>Potenciometro:  <em>"+ String (porcentaje) +" %</em></p>"
                    "<p>Temperatura: <em>"+ temp +" °C</em></p>"
                    "<p>Humedad: <em>"+ hum +" %</em></p>"
                    "<p>Led Int: <em>A"+ ledIntegrado+"</em></p>"
                    "<p>Relay: <em>"+ estadoRelay +"</em></p>"
                    "<p>Intensidad led: <em>"+intensidadLed+"</em></p>"
                    "<p>"+ message +"</p>"
                "</div>"
            "</div>"
        "</div>"
    "</div>"
    
    "<script src='https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/js/bootstrap.bundle.min.js' integrity='sha384-YvpcrYf0tY3lHB60NNkmXc5s9fDVZLESaAA55NDzOxhy9GkcIdslK1eN7N6jIeHz' crossorigin='anonymous'></script>"
"</body>"
"</html>";



DHT dht(DHTPIN, DHTTYPE);

Adafruit_SH1106G display = Adafruit_SH1106G (SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void displayInit(){
  display.begin(0x3C, true);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
 
  display.display();
}


void setup() {
  Serial.begin(11500);
  dht.begin();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LEDV_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(POTE_PIN, INPUT);
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_BUILTIN, LOW);
  analogWrite(LEDV_PIN, 0);

  // Conexión WIFI
  WiFi.begin(ssid, password, 6);
  //Cuenta 500 milisegundos para preguntar nuevamente si se pudo conectar
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println(WiFi.localIP());
  server.begin();
  
displayInit();
}

void loop() {

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    String temp = String(t);
    String hum = String(h);
    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
     
  //tomar el valor del potenciometro para mostrar, mapeado para representar 0%-100%
  int POTE_VALOR = analogRead(POTE_PIN);
  
  
  float valorPotePlaca= map(POTE_VALOR, 0, 4095, 0, 255);
  float porcentaje = float(valorPotePlaca) * 100/255;
  
  
  delay(10); 
  //lo que va a mostrar el display
  display.clearDisplay();
  display.setCursor(0,0);
  display.printf("Potenc: %.2f %%\n",porcentaje);
  display.printf("Temperatura: %.2f %C\n", t);
  display.printf("Humedad: %.2f %%\n", h);
  display.printf("Led Integr: %s\n", ledIntegrado);
  display.printf("Relay: %s \n", estadoRelay);
  display.printf("Led Verde: %s %%\n", intensidadLed);
  display.printf("Mensaje: %s \n", message);
  display.display();
 
  

WiFiClient client = server.available();
      if (client) {  
        String currentLine = "";
          while (client.connected()) {
            if (client.available()) {
              char c = client.read(); 
              header += c;
              if (c == '\n') {

                if (currentLine.length() == 0) {
                  client.println("HTTP/1.1 200 OK");   
                  client.println("Content-type:text/html");             	
 	                client.println("Connection: close");                  	
	                client.println();
                  if (header.indexOf("GET /ledOn") >= 0) {
                    digitalWrite(LED_BUILTIN, HIGH);
                    ledIntegrado ="Encendido";
                    }
                     else if (header.indexOf("GET /ledOff") >= 0) {
                      digitalWrite(LED_BUILTIN, LOW);
                      ledIntegrado = "Apagado";
                    }
                      else if (header.indexOf("GET /releActivar") >= 0) {
                      digitalWrite(RELAY_PIN, HIGH);
                      estadoRelay = "Activado";
                    }
                      else if (header.indexOf("GET /ledVa") >= 0) {
                      int brightness = map(0, 0, 100, 0, 255);
                      analogWrite(LEDV_PIN, brightness);
                      intensidadLed="0";
                    }
                    else if (header.indexOf("GET /ledVb") >= 0) {
                      int brightness = map(25, 0, 100, 0, 255);
                      analogWrite(LEDV_PIN, brightness);
                      intensidadLed = "25";
                    }
                    else if (header.indexOf("GET /ledVc") >= 0) {
                      int brightness = map(50, 0, 100, 0, 255);
                      analogWrite(LEDV_PIN, brightness);
                      intensidadLed = "50";
                    }
                    else if (header.indexOf("GET /ledVd") >= 0) {
                      int brightness = map(75, 0, 100, 0, 255);
                      analogWrite(LEDV_PIN, brightness);
                      intensidadLed = "75";
                    }
                    else if (header.indexOf("GET /ledVe") >= 0) {
                      int brightness = map(100, 0, 100, 0, 255);
                      analogWrite(LEDV_PIN, brightness);
                      intensidadLed = "100";
                    }
                      else if (header.indexOf("GET /releDesactivar") >= 0) {
                      digitalWrite(RELAY_PIN, LOW);
                      estadoRelay = "Desactivado";
                    }
                      else if (header.indexOf("/?texto=") != -1) {
                          int separatorIndex = header.indexOf('=');
                          if (separatorIndex != -1) {
                            message = header.substring(separatorIndex + 1);
                            message = message.replace("+", " ");
                          }
                    }
                      client.println(pagina);                             
                        
                  client.println();
                  break;
                }else {
                   currentLine = "";
                }
              } else if (c != '\r') {
                currentLine += c; 
              }
            }
          }

          // Limpiamos la variable header
          Serial.println(header);

          header = "";
             // Cerramos la conexión
          client.stop();
      }

}