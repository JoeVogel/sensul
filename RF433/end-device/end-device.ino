#include "RHReliableDatagram.h"
#include "RH_ASK.h"
#include <SPI.h>

#define COORDINATOR_ADDRESS 0
//Endereço utilizado para pareamento com o coordenador
#define SYCHRONISM_ADDRESS 254

int joinButton = 3; //Switch 0

uint8_t from;
uint8_t len;

bool joined;

String dataToSend;

RH_ASK driver;
RHReliableDatagram manager(driver, SYCHRONISM_ADDRESS); //We can use manager.setThisAddress(uint8_t thisAddress) to edit address

void setup() 
{
  	Serial.begin(9600);

  	pinMode(joinButton, INPUT);

  	if (!manager.init())
    	Serial.println("init failed");

}

// Dont put this on the stack:
uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];

void loop()
{
  	if (manager.available())
  	{
  		if (joined)
  			manageRequest();
  	}

  	checkButtons();
}

void checkButtons()
{
	if (joinButton)
	{
		if (manager.available())
  			networkJoin();
  		else
  			Serial.println("RADIO ISN'T AVAILABLE");

	}
}

void networkJoin()
{
	uint8_t data[] = "J;SDTH22H;SDTH22T;SDTH22H;SDTH22T;SDTH22H;SDTH22T;"; //Validar com João

	if (manager.sendtoWait(data, sizeof(data), COORDINATOR_ADDRESS))
  {
   	// Now wait for a reply from the server
   	uint8_t len = sizeof(buf);
   	uint8_t from;   
    
   	if (manager.recvfromAckTimeout(buf, &len, 2000, &from))
    {
      manager.setThisAddress(buf);//Converter uint8_t* para uint8_t
    }
    else
      Serial.println("NO REPLY. IS THE DEVICE RUNNING?"); 
  }
  else
   	Serial.println("SEND FAILED");
}

void manageRequest()
{
	uint8_t len = sizeof(buf);
  	uint8_t from;
	
  	if (manager.recvfromAck(buf, &len, &from))
  	{
  		//Verifica se a mensagem veio do coordenador
  		if (from == 0)
  		{
  	   		//Requisição de leitura?
  			if ((char*)buf == "RR")
  	   		{
            mountPacket();
  	   			uint8_t packet[sizeof(dataToSend)];
            dataToSend.getBytes(packet, sizeof(dataToSend));
            // Send a reply back 
  	   			if (!manager.sendtoWait(packet, sizeof(dataToSend), from))
	  	   			Serial.println("SEND RESPONSE FAILED");	
  	   		}
  		}
  	}
}

void mountPacket()
{
	dataToSend = "";
}