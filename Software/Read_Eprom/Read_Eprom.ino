#include <Wire.h> 					// Built-in
#include <String.h>					// Built-in
#include <EEPROM.h>					// Built-in

int 			epromSize = 96;					// Max address used for analyzer
int				epromAddress;					// EEPROM Address
unsigned int	data = 0;						// What to put there

void setup()
{
	Serial.begin ( 9600 );				// Initialize serial monitor
	delay ( 1000 );

	for ( epromAddress = 0; epromAddress < epromSize; epromAddress +=2 )
	{
		EEPROM.get ( epromAddress, data );

		Serial.print ( epromAddress );
		Serial.print ( "    " );

		Serial.println ( data );

	}

	Serial.println ( "\n\nEEPROM finished" );

}

void loop() {}								// No loop!
 
