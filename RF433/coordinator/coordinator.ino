#include <EEPROM.h>
#include "RHReliableDatagram.h"
#include "RH_ASK.h"
#include <SPI.h>

#define COORDINATOR_ADDRESS 0
//AVAILABLE_ADDRESSES = 256 (0 - 255) - colector (0) - broadcast (255) - endereço de sincronismo (254) = 253
#define AVAILABLE_ADDRESSES 253

unsigned long delay_time = 300000;

uint8_t colectorsAddresses[253];
uint8_t currentColector = 1;

int commandButton = 11; //switch 0

uint8_t from;
uint8_t len;

RH_ASK driver;
RHReliableDatagram manager(driver, COORDINATOR_ADDRESS);

void setup() 
{
  	Serial.begin(9600);

  	pinMode(commandButton, INPUT);

  	getNodesFromEEPROM();

  	if (!manager.init())
    	Serial.println("INIT FAILED!");
  	
}

//Important: Dont put this on the stack
uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
uint8_t data[] = "RR"; //RR = Read Request

void loop()
{
	checkButtons();
	checkSerial();

	if (colectorsAddresses[currentColector])
		requestToColector();
  	
	if (currentColector == AVAILABLE_ADDRESSES)
		currentColector = 1;
	else
		currentColector++;

	delay(delay_time);
}

void checkButtons()
{
	if (digitalRead(commandButton))
	{
		//Ver com João como faremos. Propor tela de diálogo no rasp solicitando o que o técnico pretende fazer
		
		/*
		Opções tela:
		
		-> Adicionar coletor;
		-> Remover coletor.

		*/
	}
}

void checkSerial()
{
	//Ver com João se ele deseja mandar comandos via raspberry (Serial)
}

void requestToColector()
{
	uint8_t colector = colectorsAddresses[currentColector];

  	// Send a message to the colector
  	if (manager.sendtoWait(data, sizeof(data), colector))
  	{
    	//from receives 0 to clear old value. 0 is the same address of server. 
    	//It can be used do find errors
    	from	= 0;   
    	len		= sizeof(buf);   
    
    	if (manager.recvfromAckTimeout(buf, &len, 2000, &from))
    	{
      		Serial.println((char*)buf);
    	}
    	else
    	{
    		Serial.println("NO REPLY. IS THE DEVICE RUNNING?");
    	}
  	}
  	else
  	{
    	Serial.println("SEND FAILED");
  	}
}

int addNodeToNetwork()
{
	int address;

	for (int i = 1; i <= AVAILABLE_ADDRESSES; ++i)
	{
		uint8_t value = EEPROM.read(i);

		if (value == 0)
		{
			EEPROM.write(i, 1);	
			address = i;
			break;
		}
	}

	getNodesFromEEPROM();

	return address;
}

void removeNodeFromNetwork(int address)
{
	EEPROM.write(address, 0);

	getNodesFromEEPROM();
}

void getNodesFromEEPROM()
{
	//clear array
	memset(colectorsAddresses, 0, sizeof colectorsAddresses);

	for (int i = 1; i <= AVAILABLE_ADDRESSES; ++i)
	{
		colectorsAddresses[i] = EEPROM.read(i);		 
	}
}