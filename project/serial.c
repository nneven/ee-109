#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "serial.h"
#include "lcd.h"
#define FOSC 16000000
#define BAUD 9600
#define MYUBRR (FOSC/16/BAUD-1)

void init_serial() {
	UCSR0B |= (1 << RXCIE0);
	UBRR0 = 103;
	UCSR0B |= (1 << TXEN0 | 1 << RXEN0);
	UCSR0C = (3 << UCSZ00);
	UCSR0A &= ~(1 << U2X0);
	sReceived = 0;
	rStart = 0;
}

void send( short speed ) {
	int i = 0;
	char buf[5];
	snprintf(buf, 5, "%d", speed);

	UDR0 = '<';
	while ( (UCSR0A & (1 << UDRE0)) == 0 );
	while ( buf[i] != '\0' ) {
		UDR0 = buf[i];
		i++;
		while ( (UCSR0A & (1 << UDRE0)) == 0 );
	}
	UDR0 = '>';
	while ( (UCSR0A & (1 << UDRE0)) == 0 );
}

ISR(USART_RX_vect) {
	char current_char = UDR0;

	// START OVER
	if (current_char == '<') {
		rStart = 1;
		bCount = 0;
		sReceived = 0;
	}
	// CHARS RECIEVED
	else if ((current_char == '>') && bCount ) {
		rStart = 0;
		buffer[bCount] = '\0';
		sReceived = 1;
	}
	else if (rStart) {
		// CORRUPT DATA
		if ( (current_char < '0') || (current_char > '9') ) {
			bCount = 0;
			rStart = 0;
		}
		// TOO MANY CHARS
		else if ( bCount > 3 ) {
			bCount = 0;
			rStart = 0;
		}
		// VALID
		else {
			buffer[bCount] = current_char;
			bCount++;
		}
	}
}