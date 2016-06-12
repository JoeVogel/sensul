#include "PubSubClient.h"
#include "DHT.h"
#include <ESP8266WiFi.h>

#define DHTPIN 2
#define DHTTYPE DHT22

#define ssid "VOGEL_NETWORK"
#define passwd "labradora"

/*#define mqtt_server "10.0.109.177"
#define mqtt_user ""
#define mqtt_password ""*/

/*#define mqtt_server "m12.cloudmqtt.com"
#define mqtt_user "vwdwpedt"
#define mqtt_password "8E-bfl64oLIj"*/

#define mqtt_server "104.31.79.107"
#define mqtt_user ""
#define mqtt_password ""

WiFiClient espClient;
PubSubClient client(espClient);

DHT dht(DHTPIN, DHTTYPE);

String payload; 

void setup()
{
  Serial.begin(115200);
  
  wifiConnect();
  client.setServer(mqtt_server, 1883);
  //client.setServer(mqtt_server, 13679);

  dht.begin();
}

void loop()
{
  //ESP.deepSleep(60000, WAKE_RF_DEFAULT);

  delay(1000);

  if (!client.connected()) {
    reconnect();
  }
  
  //“{ ‘temperature’: 22.5, ‘humidity’: 31 }"
  payload = "'{ 'temperature': ";
  payload += dht.readTemperature(); 
  payload += ", 'humidity': ";
  payload += dht.readHumidity();
  payload += " }'";
  
  client.publish("/sensul-sensors", payload.c_str(), true);

  
}

void wifiConnect()
{
  Serial.print("CONNECTING TO ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, passwd);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WIFI CONNECTED");
  Serial.println("IP ADDRESS: ");
  Serial.println(WiFi.localIP());
} 

void reconnect() {
  
  while (!client.connected()) {
    Serial.print("ATTEMPTING MQTT CONNECTION...");

    //if (client.connect("Client01", mqtt_user, mqtt_password)) {
    if (client.connect("Client01")) {
      Serial.println("CONNECTED.");
    } else {
      Serial.print("FAILED, RC= ");
      Serial.print(client.state());
      Serial.println(" TRY AGAIN IN 5 SECONDS.");
      delay(5000);
    }
  }
}


