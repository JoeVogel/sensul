#include "Wire.h"
#include "Adafruit_BMP085.h" // https://github.com/4refr0nt/iot-manager-demo/tree/master/ArduinoIDE/ESP8266/users-demo/bmp180/Adafruit-BMP085
#include "PubSubClient.h"
#include "DHT.h"
#include <ESP8266WiFi.h>

#define DHT_PIN 0
#define DHT22_TYPE DHT22

const char *ssid      = "ESPap";
const char *passwd    = "1234567890";

#define mqtt_server ""
#define mqtt_user ""
#define mqtt_password ""

DHT dht(DHT_PIN, DHT22_TYPE);
Adafruit_BMP085 bmp;

WiFiClient espClient;
PubSubClient client(espClient);

String payload; 
String mac;

void setup()
{
    Serial.begin(115200);
  
  	dht.begin();

    Wire.pins(4, 5); //SDA, SCL. Needed because ESP8266 pins aren't the same of Arduino. In ESP-01 is Wire.pins(0, 2)
  	
    if (!bmp.begin())
    {

      Serial.println("COULD NOT FIND A VALID BMP SENSOR, CHECK WIRING!");

    } 
  
  	//Busca e tratamento do MAC para uso na identificação do payload
  	mac = WiFi.macAddress();

  	client.setServer(mqtt_server, 1883);

  	wifiConnect();

}

void loop()
{
  	if (mqttConnect())
  	{

  		publish();

  	}

    //FreeHEAP();

  	delay(60000);//1 min

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
  payload += "\"temperature\":";
  payload += dht1.readTemperature(); 
  payload += ",";
  payload += "\"humidity\":";
  payload += dht1.readHumidity();
  payload += ","; 
  payload += "\"pressure\":";
  payload += bmp.readPressure();
  //payload += ","; 
  //final sensores

  bmp.readPressure()/133

  payload += "}";

  Serial.println(payload);

  return payload;

}

void publish()
{

	if (client.publish("/wheater", fillPayload().c_str(), true))
  {

    Serial.println("SENDED.");

  }
  else
  {

   	Serial.println("NOT SENDED.");

  }

}

void FreeHEAP() {

  if ( ESP.getFreeHeap() < freeheap ) {

    if ( ( freeheap != 100000) ) {

       Serial.print("MEMORY LEAK DETECTED! OLD FREE HEAP = ");
       Serial.print(freeheap);
       Serial.print(", NEW VALUE = ");
       Serial.println(ESP.getFreeHeap());

    }

    freeheap = ESP.getFreeHeap();

  }

}