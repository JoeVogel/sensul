#include "RHReliableDatagram.h"
#include "RH_ASK.h"
#include <SPI.h>
#include <EEPROM.h>

#define COORDINATOR_ADDRESS 0
//Endereço utilizado para pareamento com o coordenador
#define SYCHRONISM_ADDRESS 254
#define EEPROM_ADDRESS 1

int joinButton = 3; //Switch 0

bool joined; //FALSE by default

String dataToSend;

RH_ASK driver;
RHReliableDatagram manager(driver, SYCHRONISM_ADDRESS); //We can use manager.setThisAddress(uint8_t thisAddress) to edit address

void setup() 
{
  	Serial.begin(9600);

  	pinMode(joinButton, INPUT);

    checkEEPROM();

  	if (!manager.init())
    	Serial.println("init failed");

}

// Dont put this on the stack:
uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];

void loop()
{
  	if (manager.available())
  	{
  		manageRequest();
  	}

  	checkButtons();
}

void checkEEPROM()
{
  uint8_t value = EEPROM.read(EEPROM_ADDRESS);

  if (value != SYCHRONISM_ADDRESS)
  {
    manager.setThisAddress(value);
  }
}

void checkButtons()
{
	if (joinButton)
	{
    if (!joined)
    {
      if (manager.available())
      {
        networkJoin();
      }
      else
      {
        Serial.println("RADIO ISN'T AVAILABLE");
      }
    }
    else
    {
      Serial.println("THIS DEVICE IS CURRENTLY IN A NETWORK.")
    }
	}
}

void networkJoin()
{
	uint8_t data[] = "J;SDTH22H;SDTH22T;SDTH22H;SDTH22T;SDTH22H;SDTH22T;"; //Validar com João

  Serial.println("SENDING JOIN PACKET");
	if (manager.sendtoWait(data, sizeof(data), COORDINATOR_ADDRESS))
  {
   	// Now wait for a reply from the server
   	uint8_t len = sizeof(buf);
   	uint8_t from;   
    
   	if (manager.recvfromAckTimeout(buf, &len, 2000, &from))
    {
      manager.setThisAddress(buf);//Converter uint8_t* para uint8_t
      EEPROM.write(EEPROM_ADDRESS, buf); //Verificar comportamento
      joined = true;
      Serial.print("JOINED. CURENTLY ADDRESS IS ");
      Serial.println(buf);
    }
    else
      Serial.println("NO REPLY. IS THE DEVICE RUNNING?"); 
  }
  else
   	Serial.println("SEND FAILED");
}

void reset()
{
  manager.setThisAddress(SYCHRONISM_ADDRESS);
  EEPROM.write(EEPROM_ADDRESS, SYCHRONISM_ADDRESS);
  joined = false;
}

void manageRequest()
{
	uint8_t len = sizeof(buf);
  uint8_t from;
	
  if (manager.recvfromAck(buf, &len, &from))
  {

    Serial.print("RECEIVED A REQUEST FROM ");
    Serial.println(from);
  	//Verifica se a mensagem veio do coordenador
  	if (from == 0)
  	{
     	//Coleta
  		if ((char*)buf == "RR")
     	{

        mountPacket();

        //Converte para array de uint8_t
     		uint8_t packet[sizeof(dataToSend)];
        dataToSend.getBytes(packet, sizeof(dataToSend));

        // Send a reply back 
     		if (manager.sendtoWait(packet, sizeof(dataToSend), from))
          Serial.println("RESPONSE SENDED"); 
        else
          Serial.println("SEND RESPONSE FAILED"); 

     	}
  	}
    else
      Serial.println("REQUEST DON'T CAME FROM THE COORDINATOR.");
  }
}

void mountPacket()
{
	dataToSend = "";
}