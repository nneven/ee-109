/********************************************
 *
 *  Name:
 *  Email:
 *  Section:
 *  Assignment: Lab 7 - Rotary Encoder
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "lcd.h"

void variable_delay_us(int);
void play_note(unsigned short);

// Frequencies for natural notes from middle C (C4)
// up one octave to C5.
unsigned short frequency[8] =
    { 262, 294, 330, 349, 392, 440, 494, 523 };

volatile unsigned char new_state, old_state;
volatile unsigned char changed = 0;  // Flag for state change
volatile int count = 0;		// Count to display

int main(void) {

    // Initialize DDR and PORT registers and LCD
    lcd_init();
    DDRC &= ~((1 << PC1) | (1 << PC5));
    PORTC |= ((1 << PC1) | (1 << PC5));
    DDRB |= (1 << PB4);
    PORTB &= ~(1 << PB4);

    PCICR |= (1 << PCIE1);
    PCMSK1 |= ((1 << PCINT9) | (1 << PCINT13));
    sei();

    // Write a spash screen to the LCD
    lcd_writecommand(1);
    lcd_stringout("Lab 7");
    lcd_moveto(1, 0);
    lcd_stringout("Nicolas Neven");
    _delay_ms(1000);
    lcd_writecommand(1);

    unsigned char status = PINC;

    unsigned char a = status & (1 << PC5);
    unsigned char b = status & (1 << PC1);

    // Read the A and B inputs to determine the intial state
    // Warning: Do NOT read A and B separately.  You should read BOTH inputs
    // at the same time, then determine the A and B values from that value.    

    if (!b && !a)
	old_state = 0;
    else if (!b && a)
	old_state = 1;
    else if (b && !a)
	old_state = 2;
    else
	old_state = 3;

    new_state = old_state;

    lcd_moveto(0, 0);
	lcd_writecommand(1);
    char string[17];
    snprintf(string, 17, "Counter: %d", count);
    lcd_stringout(string);

    while (1) {                 // Loop forever
	    if (changed) { // Did state change?
	    	changed = 0;        // Reset changed flag

		    // Output count to LCD
		    lcd_moveto(0, 0);
		    lcd_writecommand(1);
	    	snprintf(string, 17, "Counter: %d", count);
	    	lcd_stringout(string);

		    // Do we play a note?
		    if ((count % 8) == 0) {

		    	lcd_moveto(0, 0);
			    lcd_writecommand(1);
		    	char string[17];
		    	snprintf(string, 17, "Counter: %d", count);
		    	lcd_stringout(string);

				// Determine which note (0-7) to play
			    unsigned char note = abs((count % 64) / 8);

				// Find the frequency of the note
			    unsigned short freq = frequency[note];

			    lcd_moveto(1, 0);
		    	snprintf(string, 17, "Freq: %d", freq);
		    	lcd_stringout(string);

				// Call play_note and pass it the frequency
				play_note(freq);

			}
	    }
    }
}

/*
  Play a tone at the frequency specified for one second
*/
void play_note(unsigned short freq)
{
    unsigned long period;

    period = 1000000 / freq;      // Period of note in microseconds

    while (freq--) {
		// Make PB4 high
	    PORTB |= (1 << PB4);

		// Use variable_delay_us to delay for half the period
		variable_delay_us(period / 2);

		// Make PB4 low
		PORTB &= ~(1 << PB4);

		// Delay for half the period again
		variable_delay_us(period / 2);

    }
}

/*
    variable_delay_us - Delay a variable number of microseconds
*/
void variable_delay_us(int delay)
{
    int i = (delay + 5) / 10;

    while (i--)
        _delay_us(10);
}

ISR(PCINT1_vect) {
	// Read the input bits and determine A and B
    unsigned char status = PINC;

    unsigned char a = status & (1 << PC5);
    unsigned char b = status & (1 << PC1);

	// For each state, examine the two input bits to see if state
	// has changed, and if so set "new_state" to the new state,
	// and adjust the count value.
	if (old_state == 0) {

	    // Handle A and B inputs for state 0
	    if (a) {
	    	new_state = 1;
	    	count++;
	    } else if (b) {
	    	new_state = 2;
	    	count--;
	    }

	}
	else if (old_state == 1) {

	    // Handle A and B inputs for state 1
	    if (!a) {
	    	new_state = 0;
	    	count--;
	    } else if (b) {
	    	new_state = 3;
	    	count++;
	    }

	}
	else if (old_state == 2) {

	    // Handle A and B inputs for state 2
	    if (!b) {
	    	new_state = 0;
	    	count++;
	    } else if (a) {
	    	new_state = 3;
	    	count--;
	    }

	}
	else {   // old_state = 3

	    // Handle A and B inputs for state 3
	    if (!b) {
	    	new_state = 1;
	    	count--;
	    } else if (!a) {
	    	new_state = 2;
	    	count++;
	    }

	}

	// If state changed, update the value of old_state,
	// and set a flag that the state has changed.
	if (new_state != old_state) {
	    changed = 1;
	    old_state = new_state;
	}
}
