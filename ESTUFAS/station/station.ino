#include "PubSubClient.h"
#include "DHT.h"
#include <ESP8266WiFi.h>

#define DHTPIN1 5
#define DHTPIN2 5
#define DHTPIN3 5
#define DHTTYPE DHT22

#define ssid "VOGEL_NETWORK"
#define passwd "labradora"

#define mqtt_server "192.81.213.239"
#define mqtt_user ""
#define mqtt_password ""

WiFiClient espClient;
PubSubClient client(espClient);

DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN1, DHTTYPE);
DHT dht3(DHTPIN1, DHTTYPE);

String payload; 
String mac;

void setup()
{
  Serial.begin(115200);
  
  //Busca e tratamento do MAC para uso na identificação do payload
  mac = WiFi.macAddress();
  
  client.setServer(mqtt_server, 1883);

  dht1.begin();
  dht2.begin();
  dht3.begin();
}

void loop()
{
  wifiConnect();

  mqttConnect();
  
  client.publish("/sensors", fillPayload().c_str(), true);

  ESP.deepSleep(60000, WAKE_RF_DEFAULT);
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
      if (client.connect(mac.c_str())) {
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
  payload = "{\"mac\":";
  payload += mac;
  payload += " :[";

  //início sensor 1
  payload += "{\"temperature\":";
  payload += dht1.readTemperature(); 
  payload += ", ";
  payload += "\"humidity\":";
  payload += dht1.readHumidity();
  payload += "},";
  //final sensor 1

  //início sensor 2
  payload += "{\"temperature\":";
  payload += dht2.readTemperature(); 
  payload += ", ";
  payload += "\"humidity\":";
  payload += dht2.readHumidity();
  payload += "},";
  //final sensor 2

  //início sensor 3
  payload += "{\"temperature\":";
  payload += dht3.readTemperature(); 
  payload += ", ";
  payload += "\"humidity\":";
  payload += dht3.readHumidity();
  payload += "}"; //Sem "," no final, pois é o último elemento
  //final sensor 3

  payload += "]}";

  return payload;

}