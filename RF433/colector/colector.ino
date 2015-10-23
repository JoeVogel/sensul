#include <RHReliableDatagram.h>
#include <RH_ASK.h>
#include <SPI.h>

#define COLECTOR_ADDRESS 0

int devices[254]; //256 - colector - broadcast = 254
uint8_t from;
uint8_t len;

RH_ASK driver;
RHReliableDatagram manager(driver, COLECTOR_ADDRESS);

void setup() 
{
  	Serial.begin(9600);
  	if (!manager.init()){
    	Serial.println("INIT FAILED!");
  	}
}

//Important: Dont put this on the stack
uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
uint8_t data[] = "1";

void loop()
{
    Serial.print("SENDING REQUEST TO ");
    Serial.println(); //Colocar o endereço do end device

  	// Send a message to manager_server
  	if (manager.sendtoWait(data, sizeof(data), COLECTOR_ADDRESS))
  	{
    	//from receives 0 to clear old value. 0 is the same address of server. 
    	//It can be used do find errors
    	from	= 0;   
    	len		= sizeof(buf);   
    
    	if (manager.recvfromAckTimeout(buf, len, 5000, &from))
    	{
      		Serial.println(from);
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
  	
  delay(500);
}