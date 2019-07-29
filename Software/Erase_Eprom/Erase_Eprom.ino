#include <Wire.h> 					// Built-in
#include <EEPROM.h>					// Built-in

int 	epromSize = 1024;				// On the Mega
int		epromAddress;					// EEPROM Address
char	data = 0;						// What to put there

void setup()
{
	Serial.begin ( 9600 );				// Initialize serial monitor
	delay ( 1000 );

	for ( epromAddress = 0; epromAddress < epromSize; epromAddress++ )
		EEPROM.put ( epromAddress, data );

	Serial.println ( "EEPROM erased" );

}

void loop() {}								// No loop!
 
