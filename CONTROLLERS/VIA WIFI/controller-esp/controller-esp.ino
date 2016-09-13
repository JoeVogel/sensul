#include "PubSubClient.h"
#include <ESP8266WiFi.h>

//const char *ssid      = "jaraguatec";
//const char *passwd    = "89vcj13jkd39dsmg3";

const char *ssid      = "SENSUL_AGRICULTURE";
const char *passwd    = "";

#define MQTT_SERVER "138.68.21.186"
#define MQTT_USER ""
#define MQTT_PASSWORD ""
#define RELE_PORT 4

WiFiClient espClient;
PubSubClient client(espClient);

String payload; 
String mac;
int lastMQTTReconnectAttempt;
int lastReconnectAttempt;
int releStatus;

void callback(char* topic, byte* payload, unsigned int length) 
{
	String data = "";
  
	Serial.print("MESSAGE ARRIVED [");
  	Serial.print(topic);
  	Serial.print("] ");
  	
  	for (int i=0;i<length;i++) 
  	{
    	data += payload[i];
  	}

  	Serial.print(data);

	Serial.println();

	if (data.equals("1") || data.equals("0"))
	{

		releStatus = data.toInt();
		digitalWrite(RELE_PORT, releStatus);


	} else {

		Serial.print("RECEIVED AN INVALID MESSAGE: ");
		Serial.println(data);

	}

}


/*
  boolean wifiReconnect()

  Tenta efetuar a conexão na rede WiFi. 
  Quando estiver autenticado, exibe o IP recebido e finaliza a função.
*/
boolean wifiReconnect()
{

  Serial.print("CONNECTING TO ");
  Serial.println(ssid);

  //Configura ESP no modo client
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, passwd);

  //Aguarda retorno do AP
  for (int i = 0; i < 10; i++)
  {
  	if (WiFi.status() != WL_CONNECTED) {
    
    	delay(100);
    	Serial.print(".");

  	} else {
  		
  		Serial.println("");
  		Serial.println("WIFI CONNECTED");
  		Serial.println("IP ADDRESS: ");
  		Serial.println(WiFi.localIP());
  		break;

  	}

  }

  if (WiFi.status() != WL_CONNECTED)
  {

  	Serial.println("ERROR IN WIRELESS CONNECTION");

  }

}  

/*
  boolean mqttReconnect() 

  Realiza a conexão com o broker mqtt, se em 10 tentativas não funcionar, aborta tentativa. (acender um LED???)
*/
boolean mqttReconnect() 
{
	if (client.connect(mac.c_str())) {
    	// Once connected, publish an announcement...
    	client.publish("controlsStatus", fillPayload().c_str());
    	// ... and resubscribe
    	client.subscribe("controls");
  	}
  	else 
    {
  
    	Serial.print("FAILED, RC= ");
    	Serial.println(client.state());
    }
	
	return client.connected();

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

  payload += "\"r\":";
  payload += releStatus;

  payload += "}";

  Serial.println(payload);

  return payload;

}

void setup()
{
    Serial.begin(115200);

    pinMode(RELE_PORT, OUTPUT);
    digitalWrite(RELE_PORT, LOW);
  
    //Busca MAC para uso na identificação do payload
    mac = WiFi.macAddress();

    client.setServer(MQTT_SERVER, 1883);
    client.setCallback(callback);

    lastMQTTReconnectAttempt = 0;
    lastReconnectAttempt = 0;

    wifiReconnect();

    delay(2000);

}

void loop()
{   
	if (WiFi.status() != WL_CONNECTED)
	{

		long now = millis();
    	if (now - lastReconnectAttempt > 5000)
    	{
      		lastReconnectAttempt = now;
      		// Attempt to reconnect
      		if (wifiReconnect()) 
      		{
        		lastReconnectAttempt = 0;
      		}
    	}
	
	} else {

		if (!client.connected()) 
		{
    		long now = millis();
    		if (now - lastMQTTReconnectAttempt > 5000)
    		{
      			lastMQTTReconnectAttempt = now;
      			// Attempt to reconnect
      			if (mqttReconnect()) 
      			{
        			lastMQTTReconnectAttempt = 0;
      			}
    		}
  		} else {
    		// Client connected

    		client.loop();
		}

	}

}
