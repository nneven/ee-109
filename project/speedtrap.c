/********************************************
*
*  Name: Nicolas Neven
*  Lab section: Friday 12:30
*  Assignment: Speed Trap Project
*
********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "speedtrap.h"
#include "lcd.h"
#include "encoder.h"
#include "serial.h"
#define DIST 50

int main(void) { 
	// TRI-STATE
	DDRD |= (1 << PD2);
	PORTD &= ~(1 << PD2);
	// LCD
	lcd_init();
	// ENCODER TIMER
	init_timer();
	// SERIAL
	init_serial();
	// BUZZER 
	DDRD |= (1 << PD3);
	// INTERRUPTS
	PCICR |= (1 << PCIE0);
	PCMSK0 |= (1 << PCINT3);
	sei();
	DDRB |= (1 << PB5);
	state = 0;
	sMeasured = 0;
	// SPEED TIMER
	TCCR1B |= (1 << WGM12);
	TIMSK1 |= (1 << OCIE1A);
	OCR1A = 16000;

	lcd_moveto(0,0);
	lcd_stringout("Nicolas Neven");
	lcd_moveto(1,0);
	lcd_stringout("Speed Trap");
	_delay_ms(1000);
	lcd_writecommand(1);
	lcd_moveto(0,0);
	lcd_stringout("Max:               ");
	lcd_moveto(1,0);
	lcd_stringout("S:                 ");

	//READ EEPROM SPEED
	if ( eeprom_read_byte(&sAddress) < 1 || eeprom_read_byte(&sAddress) > 99 ) {
		maxspeed = 50;
	} else {
		maxspeed = eeprom_read_byte(&sAddress);
	}
	char str[17];
	lcd_moveto(0,5);
	snprintf(str, 3, "%2d", maxspeed);
	lcd_stringout(str);

	while (1) {

		// SPEED MEASURED
		if (sMeasured) {
			int speed = DIST*1000/time; 
			if (speed > 9999)
				speed = 9999;
			else if (speed < 0)
				speed = 0;
			send(speed);

			lcd_moveto(0,10);
			snprintf(str, 7, "%4dms", time);
			lcd_stringout(str);

			lcd_moveto(1,7);
			snprintf(str, 10, "%3d.%1dcm/s", speed/10, speed%10);
			lcd_stringout(str);


			//SPEEDING
			if ((speed+9)/10 > maxspeed) {
				for (int i = 0; i < 100; i++ ){
					PORTD |= (1 << PD3);
			        _delay_us(2000);
			        PORTD &= ~(1 << PD3);
			        _delay_us(2000);
				}
				_delay_ms(100);
			}
			sMeasured = 0;
		}

		if (time > 4000) {
			state = 0;
			PCMSK0 |= (1 << PCINT3);
			PCMSK0 &= ~(1 << PCINT4);
			// STOP TIMER
			PORTB &= ~(1 << PB5);
			TCCR1B &= ~(1 << CS10);
		}

		// DATA RECEIVED
		if (sReceived) {
			int speed = convert_received_data();
			sReceived = 0;

			lcd_moveto(1,2);
			snprintf(str, 10, "%3d.%1d", speed/10, speed%10);
			lcd_stringout(str);

			// SPEEDING
			if ((speed+9)/10 > maxspeed) { 
				for (int i = 0; i < 100; i++ ){
					PORTD |= (1 << PD3);
			        _delay_us(2000);
			        PORTD &= ~(1 << PD3);
			        _delay_us(2000);
				}
			}
		}

		// MAX CHANGED
		if (sChanged) { 
			snprintf(str, 3, "%2d", maxspeed);
			eeprom_update_byte(&sAddress, maxspeed);
			lcd_moveto(0, 5);
			lcd_stringout(str);
			sChanged = 0;
		}
	}
}

short convert_received_data() {
	short data;
	sscanf(buffer, "%hd", &data);
	return data;
}

ISR(PCINT0_vect) {
	
	// FIRST SENSOR
	if (state == 0) {
		state = 1;
		PCMSK0 |= (1 << PCINT4);
		PCMSK0 &= ~(1 << PCINT3);

		// START TIMER
		time = 0;
		sMeasured = 0;
		PORTB |= (1 << PB5);
		TCCR1B |= (1 << CS10);
	}

	// SECOND SENSOR
	else {
		state = 0;
		PCMSK0 |= (1 << PCINT3);
		PCMSK0 &= ~(1 << PCINT4);
		// STOP TIMER
		sMeasured = 1;
		PORTB &= ~(1 << PB5);
		TCCR1B &= ~(1 << CS10);
	}
}

ISR(TIMER1_COMPA_vect) {
	time++;
}
