#include "PubSubClient.h"
#include <ESP8266WiFi.h>

#define ssid "VOGEL_NETWORK"
#define passwd "labradora"

#define mqtt_server "192.81.213.239"
#define mqtt_user ""
#define mqtt_password ""

WiFiClient espClient;
PubSubClient client(espClient);

String mac;

char message_buff[100];

void setup()
{
  Serial.begin(115200);
  
  //Busca e tratamento do MAC para uso na identificação do payload
  mac = WiFi.macAddress();

  wifiConnect();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop()
{
  delay(300000);

  if (!client.connected()) {
    reconnect();
  }

  client.loop();
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
	void reconnect() 

	Verifica se o a conexão MQTT ainda existe, caso tenha perdido a conexão, tenta conectar novamente
*/
void reconnect() 
{
  
  while (!client.connected()) 
  {
    Serial.print("ATTEMPTING MQTT CONNECTION...");

    //if (client.connect(mac.c_str(), mqtt_user, mqtt_password))
    if (client.connect(mac.c_str())) 
    {
      Serial.println("CONNECTED.");
      client.subscribe("/control", 1);
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

/*
	String fillPayload()

	Função responsável por criar a String JSON por meio de concatenação de strings e valores das leituras
	para envio posterior
*/
String fillPayload()
{
  String payload;

  payload = "{\"mac\":";
  payload += mac;
  payload += " :[";

  //início rele 1
  payload += "{\"re01\":";
  //LEITURA DO RELE 01 
  payload += "},";
  //final rele 1

  //início rele 2
  payload += "{\"re02\":";
  //LEITURA DO RELE 02 
  payload += "},";
  //final rele 2

  //início rele 3
  payload += "{\"re03\":";
  //LEITURA DO RELE 03 
  payload += "},";
  //final rele 3

  //início rele 4
  payload += "{\"re04\":";
  //LEITURA DO RELE 04 
  payload += "}";//Sem "," no final, pois é o último elemento
  //final rele 4

  payload += "]}";

  return payload;

}

//Capturar mensagens recebidas do topico
void callback(char* topic, byte* payload, unsigned int length) {

  int i = 0;

  //Serial.println("Message arrived:  topic: " + String(topic));
  //Serial.println("Length: " + String(length,DEC));
  
  // create character buffer with ending null terminator (string)
  for(i=0; i<length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';
  
  String msgString = String(message_buff);

  //Serial.println("Payload: " + msgString);
  
  //ADICONAR AQUI CONTROLE DOS RELES

  //verificar topico
  client.publish("/sensors", fillPayload().c_str(), true);
}