#include <EEPROM.h>

void setup() {

  Serial.begin(9600);

  Serial.println("PROCESS STARTED");
  
  for(int i = 1;i < 255;i++)
  {
    EEPROM.write(i, 0);
  }

  Serial.println("FINISHED.");
}

void loop() {
  // put your main code here, to run repeatedly:

}
