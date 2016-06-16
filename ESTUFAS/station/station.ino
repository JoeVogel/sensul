#include "PubSubClient.h"
#include "DHT.h"
#include <ESP8266WiFi.h>

#define DHTPIN1 5
#define DHTTYPE DHT22

#define ssid "VOGEL_NETWORK"
#define passwd "labradora"

#define mqtt_server "192.81.213.239"
#define mqtt_user ""
#define mqtt_password ""

WiFiClient espClient;
PubSubClient client(espClient);

DHT dht1(DHTPIN1, DHTTYPE);

String payload; 
String ESPMac = "";
unsigned char mac[6];

void setup()
{
  Serial.begin(115200);
  
  //Descoberta do MAC para uso na identificação do payload
  WiFi.macAddress(mac);
  ESPMac += macToStr(mac);

  wifiConnect();
  client.setServer(mqtt_server, 1883);

  dht1.begin();
}

void loop()
{
  //ESP.deepSleep(60000, WAKE_RF_DEFAULT);

//Delay para testes sem SLEEP	
  delay(300000);

  if (!client.connected()) {
    reconnect();
  }
  
  client.publish("/sensors", fillPayload().c_str(), true);
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
  
  while (!client.connected()) {
    Serial.print("ATTEMPTING MQTT CONNECTION...");

    //if (client.connect("Client01", mqtt_user, mqtt_password)) {
    if (client.connect("ESPClient - " + mac)) {
      Serial.println("CONNECTED.");
    } else {
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
  payload = "{\"sensors\":[";

  payload += "{\"temperature\":";
  payload += dht1.readTemperature(); 
  payload += ", ";
  payload += "\"humidity\":";
  payload += dht1.readHumidity();
  payload += "}";

  payload += "]}";

  return payload;

}

/*
	String macToStr(const uint8_t* mac)

	Função responsável por extrair o MAC do array e transformar em uma String

	@const uint8_t* mac : este parâmetro recebe o array contendo o MAC que se deseja transformar em String
*/
String macToStr(const uint8_t* mac)
{
	String result;

	for (int i = 0; i < 6; ++i) {

		result += String(mac[i], 16);

		if (i < 5)
		{
			result += ':';
		}

	}

	return result;

}