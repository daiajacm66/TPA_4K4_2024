#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <DHT.h>

#define DHTPIN 14 // Pin del sensor de temperatura y humedad
#define DHTTYPE DHT22 // Tipo de sensor DHT

#define LED_PIN 2 // Pin del LED Azul integrado
#define RELAY_PIN 5 // Pin del relé para el dispositivo de alta potencia
#define POT_PIN 34 // Pin del potenciómetro
#define GREEN_LED_PIN 4 // Pin del LED Verde
#define MESSAGE_LENGTH 100 // Longitud máxima del mensaje

Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, &Wire);
DHT dht(DHTPIN, DHTTYPE);

WiFiServer server(80);

const char* ssid = "TuSSID";
const char* password = "TuContraseña";

int relayState = LOW; // Estado inicial del relé
int ledState = LOW; // Estado inicial del LED Azul
int greenLedIntensity = 0; // Intensidad inicial del LED Verde
char message[MESSAGE_LENGTH] = ""; // Mensaje a mostrar

void displayInit(){
  display.begin(0x3C, true);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.println("ESP32 Web Server");
  display.display();
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  dht.begin();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  server.begin();
  displayInit();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        currentLine += c;
        if (c == '\n') {
          if (currentLine.length() == 1) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if (currentLine.indexOf("GET /") != -1) {
              client.println("<html><head><title>ESP32 Web Server</title></head><body>");
              client.println("<h1>Estado Actual</h1>");
              
              // Lectura del potenciómetro
              int potValue = analogRead(POT_PIN);
              client.print("Valor del Potenciómetro: ");
              client.println(potValue);

              // Lectura del sensor de temperatura y humedad
              float humidity = dht.readHumidity();
              float temperature = dht.readTemperature();
              client.print("Temperatura: ");
              client.print(temperature);
              client.print("°C, Humedad: ");
              client.print(humidity);
              client.println("%");

              // Estado del LED Azul
              client.print("Estado del LED Azul: ");
              client.println(ledState == HIGH ? "Encendido" : "Apagado");

              // Estado del relé
              client.print("Estado del Relé: ");
              client.println(relayState == HIGH ? "Encendido" : "Apagado");

              // Intensidad del LED Verde
              client.print("Intensidad del LED Verde: ");
              client.println(greenLedIntensity);

              // Formulario para ingresar un mensaje
              client.println("<h2>Ingrese un Mensaje</h2>");
              client.println("<form action='/' method='POST'>");
              client.println("<input type='text' name='message' maxlength='100'><br><br>");
              client.println("<input type='submit' value='Enviar'>");
              client.println("</form>");

              // Mostrar mensaje ingresado
              client.print("<p>Mensaje: ");
              client.print(message);
              client.println("</p>");

              client.println("</body></html>");
            } else if (currentLine.indexOf("POST /") != -1) {
              if (currentLine.indexOf("message=") != -1) {
                int startPos = currentLine.indexOf("message=") + 8;
                int endPos = currentLine.indexOf("HTTP") - 1;
                String tempMessage = currentLine.substring(startPos, endPos);
                tempMessage.toCharArray(message, MESSAGE_LENGTH);
              }
              client.println("HTTP/1.1 301 Moved Permanently");
              client.println("Location: /");
              client.println("Connection: close");
              client.println();

              delay(100);
              client.stop();
              return;
            }

            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
  }
}
