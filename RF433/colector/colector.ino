#include <EEPROM.h>
#include <RHReliableDatagram.h>
#include <RH_ASK.h>
#include <SPI.h>

#define COORDINATOR_ADDRESS 0

//AVAILABLE_ADDRESSES = 256 (0 - 255) - colector (0) - broadcast (255) = 254 (0 - 253)
#define AVAILABLE_ADDRESSES 253 

uint8_t colectorsAvaliable[254];
uint8_t currentColector;

uint8_t from;
uint8_t len;

RH_ASK driver;
RHReliableDatagram manager(driver, COORDINATOR_ADDRESS);

void setup() 
{
  	Serial.begin(9600);

  	getNodesFromEEPROM();

  	if (!manager.init()){
    	Serial.println("INIT FAILED!");
  	}
}

//Important: Dont put this on the stack
uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
uint8_t data[] = "1";

void loop()
{
	for (int i = 0; i < sizeof(colectorsAvaliable); ++i)
	{
		currentColector = colectorsAvaliable[i];

  		// Send a message to the colector
  		if (manager.sendtoWait(data, sizeof(data), currentColector))
  		{
    		//from receives 0 to clear old value. 0 is the same address of server. 
    		//It can be used do find errors
    		from	= 0;   
    		len		= sizeof(buf);   
    
    		if (manager.recvfromAckTimeout(buf, &len, 5000, &from))
    		{
      			Serial.println("");
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
  	
	delay(500);
}

void addNodeToNetwork()
{
	for (int i = 1; i < AVAILABLE_ADDRESSES; ++i)
	{
		uint8_t value = EEPROM.read(i);

		if (value == 0)
		{
			EEPROM.write(i, (uint8_t)1);	
			break;
		}
	}
}

void removeNodeFromNetwork(int address)
{
	EEPROM.write(address, 0);

	getNodesFromEEPROM();
}

void getNodesFromEEPROM()
{
	//clear array
	memset(colectorsAvaliable, 0, sizeof colectorsAvaliable);

	for (int i = 1; i < AVAILABLE_ADDRESSES; ++i)
	{
		uint8_t value = EEPROM.read(i);

		if (value != 0)
		{
			colectorsAvaliable[i] = value;	
		}		 
	}
}