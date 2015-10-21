#include <RHReliableDatagram.h>
#include <RH_ASK.h>
#include <SPI.h>

#define SERVER_ADDRESS 1

int clients[64];

// Singleton instance of the radio driver
RH_ASK driver;

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, SERVER_ADDRESS);

void setup() 
{
  	Serial.begin(9600);
  	if (!manager.init()){
    	Serial.println("INIT FAILED!");
  	}
}

uint8_t data[];
uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];

void loop()
{
  if (manager.available())
  {
    // Wait for a message addressed to us from the client
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from))
    {
      Serial.print("got request from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);
      // Send a reply back to the originator client
      if (!manager.sendtoWait(data, sizeof(data), from))
        Serial.println("sendtoWait failed");
    }
  }
}