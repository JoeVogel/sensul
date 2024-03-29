#include <Wire.h>
#include "Adafruit_BMP085.h" // https://github.com/4refr0nt/iot-manager-demo/tree/master/ArduinoIDE/ESP8266/users-demo/bmp180/Adafruit-BMP085
#include "PubSubClient.h"
#include "DHT.h"
#include <ESP8266WiFi.h>

//const char *ssid      = "jaraguatec";
//const char *passwd    = "89vcj13jkd39dsmg3";

const char *ssid      = "SENSUL_AGRICULTURE";
const char *passwd    = "";

#define mqtt_server "138.68.21.186"
#define mqtt_user ""
#define mqtt_password ""

DHT dht1(0, DHT22);
Adafruit_BMP085 bmp;

WiFiClient espClient;
PubSubClient client(espClient);

String payload; 
String mac;

void setup()
{
    Serial.begin(115200);
  
    dht1.begin();

    Wire.pins(4, 5); //SDA, SCL. Needed because ESP8266 pins aren't the same of Arduino. In ESP-01 is Wire.pins(0, 2)
    
    if (!bmp.begin())
    {

      Serial.println("COULD NOT FIND A VALID BMP SENSOR, CHECK WIRING!");

    } 

    //Busca e tratamento do MAC para uso na identificação do payload
    mac = WiFi.macAddress();

    client.setServer(mqtt_server, 1883);

    wifiConnect(); //Testar como fica conexão caso a rede caia

    delay(2000);

}

void loop()
{   

  if (mqttConnect())
  {

    publish();

  }

  delay(1000);

}

/*
  void wifiConnect()

  Tenta efetuar a conexão na rede WiFi até conseguir. 
  Quando estiver autenticado, exibe o IP recebido e finaliza a função.
*/
void wifiConnect()
{

  Serial.print("CONNECTING TO ");
  Serial.println(ssid);

  //Configura ESP no modo client
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, passwd);

  //Aguarda retorno do AP
  while (WiFi.status() != WL_CONNECTED) {
    
    delay(100);
    Serial.print(".");

  }

  Serial.println("");
  Serial.println("WIFI CONNECTED");
  Serial.println("IP ADDRESS: ");
  Serial.println(WiFi.localIP());

}  

/*
  void mqttConnect() 

  Realiza a conexão com o broker mqtt, se em 10 tentativas não funcionar, aborta tentativa. (acender um LED???)
*/
boolean mqttConnect() 
{

  boolean connected = false;

  for (int i = 0; i < 10; i++)
  {
  
    if (!client.connected())
    {
  
      Serial.print("ATTEMPTING MQTT CONNECTION...");

      //if (client.connect(mac.c_str(), mqtt_user, mqtt_password)) {
      if (client.connect(mac.c_str()))
      {
  
        Serial.println("CONNECTED.");
        connected = true;
        break;
  
      }
      else 
      {
  
        Serial.print("FAILED, RC= ");
        Serial.print(client.state());
        Serial.println(" TRY AGAIN IN 5 SECONDS.");
        delay(5000);
  
      }
  
    }
  
  }

  return connected;

}

/*
  String fillPayload()

  Função responsável por criar a String JSON por meio de concatenação de strings e valores das leituras
  para envio posterior
*/
String fillPayload()
{

  payload = "{\"mac\":\"";
  payload += mac;
  payload += "\",";

  //início sensores
  payload += "\"t\":";
  payload += dht1.readTemperature(); 
  payload += ",";
  payload += "\"h\":";
  payload += dht1.readHumidity();
  payload += ","; 
  payload += "\"p\":";
  payload += bmp.readPressure()/100;
  //payload += ","; 
  //payload += "\"u\":";
  //payload += calculateUVIndex();
  //final sensores

  payload += "}";

  Serial.println(payload);

  return payload;

}

void publish()
{

  if (client.publish("/sensor", fillPayload().c_str(), true))
  {

    Serial.println("SENDED.");

  }
  else
  {

    Serial.println("NOT SENDED.");

  }

}
/*
void freeHEAP() {

  if ( ESP.getFreeHeap() < freeheap ) {

    if ( ( freeheap != 100000) ) {

       Serial.print("MEMORY LEAK DETECTED! OLD FREE HEAP = ");
       Serial.print(freeheap);
       Serial.print(", NEW VALUE = ");
       Serial.println(ESP.getFreeHeap());

    }

    freeheap = ESP.getFreeHeap();

  }

}*/
/*
int calculateUVIndex()
{

  int valor_sensor = analogRead(ANALOG_PIN);
  int UV_index;
  
  //Calcula tensao em milivolts
  int tensao = (valor_sensor * (5.0 / 1023.0)) * 1000;

  //Gera índice de UV
  if (tensao > 0 && tensao < 227)
  {

    UV_index = 0;
  
  }
  
  else if (tensao > 227 && tensao <= 318)
  {
  
    UV_index = 1;
  
  }
  
  else if (tensao > 318 && tensao <= 408)
  {
  
    UV_index = 2;
  
  }
  
  else if (tensao > 408 && tensao <= 503)
  {
  
    UV_index = 3;
  
  }
  
  else if (tensao > 503 && tensao <= 606)
  {
  
    UV_index = 4;
  
  }
  
  else if (tensao > 606 && tensao <= 696)
  {
  
    UV_index = 5;
  
  }
  
  else if (tensao > 696 && tensao <= 795)
  {
  
    UV_index = 6;
  
  }
  
  else if (tensao > 795 && tensao <= 881)
  {
  
    UV_index = 7;
  
  }
  
  else if (tensao > 881 && tensao <= 976)
  {
  
    UV_index = 8;
  }
  
  else if (tensao > 976 && tensao <= 1079)
  {
  
    UV_index = 9;
  
  }
  
  else if (tensao > 1079 && tensao <= 1170)
  {
  
    UV_index = 10;
  
  }
  
  else if (tensao > 1170)
  {
  
    UV_index = 11;
  
  }

}*/
