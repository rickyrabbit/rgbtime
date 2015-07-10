#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

#include "i2c_master.h"

#define WSDATA_PIN 0x08
#define WSDATA_DDR DDRD
#define WSDATA_PORT PORTD

/* Send a single data byte out to the WS2812 using somewhat correct
 * timing (it isn't as critical as the datasheet makes it seem).
 */
void sendByte(uint8_t data)
{
	for(uint8_t i = 0; i < 8; i++)
	{
		if((data & 0x80) == 0x80)
		{	
			WSDATA_PORT |= WSDATA_PIN;
			WSDATA_PORT |= WSDATA_PIN;
			WSDATA_PORT |= WSDATA_PIN;
			WSDATA_PORT |= WSDATA_PIN;
			WSDATA_PORT &= ~WSDATA_PIN;
			WSDATA_PORT &= ~WSDATA_PIN;
		} else {
			WSDATA_PORT |= WSDATA_PIN;
			WSDATA_PORT &= ~WSDATA_PIN;
			WSDATA_PORT &= ~WSDATA_PIN;
			WSDATA_PORT &= ~WSDATA_PIN;
			WSDATA_PORT &= ~WSDATA_PIN;
			WSDATA_PORT &= ~WSDATA_PIN;
		}
		data = (data<<1);
	}
}

/* Update a whole series of LEDs by sending out the RGB triplets as
 * fast as possible.
 * This function corrects for the byte order used by the LEDs.
 * The byte order for the input array is R-G-B 
 */
void updatePixels(uint8_t rgb[][3], uint16_t len)
{
	for(uint16_t i = 0; i < len; i++)
	{
		sendByte(rgb[i][1]);
		sendByte(rgb[i][0]);
		sendByte(rgb[i][2]);
	}
}	

void updateSingle(uint8_t rgb[])
{
	sendByte(rgb[1]);
	sendByte(rgb[0]);
	sendByte(rgb[2]);
}

int main(void)
{
	WSDATA_DDR |= WSDATA_PIN;
	
	uint8_t killer[3] = {0,0,0};
	
	updateSingle(killer);
	
	while(1)
	{		
		for(uint8_t i = 0; i < 255; i++)
		{
			killer[0] = i;
			killer[2] = 254-i;
			updateSingle(killer);
			_delay_ms(25);
		}
		
		for(uint8_t i = 0; i < 255; i++)
		{
			killer[0] = 254-i;
			killer[1] = i;
			updateSingle(killer);
			_delay_ms(25);
		}
		
		for(uint8_t i = 0; i < 255; i++)
		{
			killer[1] = 254-i;
			killer[2] = i;
			updateSingle(killer);
			_delay_ms(25);
		}
	}
	return 0;
}