#include "PubSubClient.h"
#include <ESP8266WiFi.h>

#define ssid "jaraguatec"
#define passwd "89vcj13jkd39dsmg3"

/*#define mqtt_server "10.0.109.177"
#define mqtt_user ""
#define mqtt_password ""*/

#define mqtt_server "m12.cloudmqtt.com"
#define mqtt_user "vwdwpedt"
#define mqtt_password "8E-bfl64oLIj"

WiFiClient espClient;
PubSubClient client(espClient);

int msg = 1;

void setup()
{
  Serial.begin(115200);
  
  wifiConnect();
  //client.setServer(mqtt_server, 1883);
  client.setServer(mqtt_server, 13679);
}

void loop()
{
  //ESP.deepSleep(60000, WAKE_RF_DEFAULT);

  delay(1000);

  if (!client.connected()) {
    reconnect();
  }

  
  client.publish("/sensor", String(msg).c_str(), true);
  msg = msg + 1;
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

    if (client.connect("Client01", mqtt_user, mqtt_password)) {
    //if (client.connect("Client01")) {
      Serial.println("CONNECTED.");
    } else {
      Serial.print("FAILED, RC= ");
      Serial.print(client.state());
      Serial.println(" TRY AGAIN IN 5 SECONDS.");
      delay(5000);
    }
  }
}
