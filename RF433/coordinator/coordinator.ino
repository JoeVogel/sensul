#include <EEPROM.h>
#include <RHReliableDatagram.h>
#include <RH_ASK.h>
#include <SPI.h>

#define COORDINATOR_ADDRESS 0
//AVAILABLE_ADDRESSES = 256 (0 - 255) - colector (0) - broadcast (255) - endereço de sincronismo (254) = 253
#define AVAILABLE_ADDRESSES 253
#define SYCHRONISM_ADDRESS 254

uint8_t colectorsAddresses[253];
uint8_t currentColector = 1;

int joinButton = 11; //switch 0

String dataToSend;

RH_ASK driver;
RHReliableDatagram manager(driver, COORDINATOR_ADDRESS);

void setup() 
{
  	Serial.begin(9600);

  	pinMode(joinButton, INPUT);

  	getNodesFromEEPROM();

  	if (!manager.init())
    	Serial.println("INIT FAILED!");
  	
}

//Important: Dont put this on the stack
uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];

void loop()
{
	checkButtons();
	checkSerial();
}

void checkButtons()
{
	if (digitalRead(joinButton))
	{
		uint8_t len = sizeof(buf);
  		uint8_t from;
	
  		if (manager.recvfromAck(buf, &len, &from))
  		{

    		Serial.print("RECEIVED A REQUEST FROM ");
    		Serial.println(from);
  			//Check if the message came from the synchronizer address
  			if (from == SYCHRONISM_ADDRESS)
  			{
  				uint8_t* newAddress = (uint8_t*)addNodeToNetwork();
    	      	// Send a reply back 
    	 		if (!manager.sendtoWait(newAddress, sizeof(newAddress), from))
		 	   		Serial.println("SEND RESPONSE FAILED");	
    	 		}
  			}
		}	
	}


void checkSerial()
{
	char readChar;
	String serialData;
	int colectorAddress;

	while(Serial.available())
	{
		readChar = Serial.read();
		serialData += readChar;
	}

	if (serialData.equals(""))
	{
		colectorAddress = serialData.substring(0, serialData.indexOf(';')).toInt();
		uint8_t buffer[40];
		serialData.getBytes(buffer, 40);
		requestToColector(buffer, colectorAddress);
	}
	
}

void requestToColector(uint8_t data[], uint8_t colectorAddress)
{
	if (colectorsAddresses[colectorAddress])
	{
		// Send a message to the colector
  		if (manager.sendtoWait(data, sizeof(data), colectorAddress))
  		{
    		// Now wait for a reply from the server
   			uint8_t len = sizeof(buf);
   			uint8_t from;   
    
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
	else
	{
		Serial.println("INVALID ADDRESS.");
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

		if (i == AVAILABLE_ADDRESSES && address == 0)
		{
			Serial.println("CAN'T ASSOCIATE MORE NODES");
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