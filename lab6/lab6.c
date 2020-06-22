/********************************************
 *
 *  Name:
 *  Email:
 *  Section:
 *  Assignment: Lab 6 - Analog-to-digital conversion
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include "lcd.h"
#include "adc.h"

void rand_init(void);

int main(void)
{

    // Initialize the LCD
    lcd_init();

    // Initialize the ADC
    adc_init();

    // Initialize the random number function
    rand_init();

    // Write splash screen
    lcd_writecommand(1);
    lcd_stringout("Lab 6");
    lcd_moveto(1, 0);
    lcd_stringout("Nicolas Neven");
    _delay_ms(1000);
    lcd_writecommand(1);

    // Find a random initial position of the 'X'
    unsigned char r = (15.0 / 32767) * rand();

    // Display the 'X' on the screen
    lcd_moveto(0, r);
    lcd_stringout("X");

    unsigned char next = 0;
    unsigned char prev = 0;
    unsigned char count = 0;

    while (1) {

    	// Check for button pressed
        prev = next;
        next = (15.0 / 32767) * adc_sample(0x03);


        if (prev != next) {

            // If pressed, move 'X' to new position
            lcd_moveto(1, prev);
            lcd_stringout(" ");
            lcd_moveto(1, next);
            lcd_stringout("X");

            // Do a conversion of potentiometer input
            prev = next;
            next = (15.0 / 32767) * adc_sample(0x03);

        }

    	// Move '^' to new position
        lcd_moveto(1, 0);
        lcd_stringout(" ");
        lcd_moveto(1, next);
        lcd_stringout("^");
        prev = next;

        // Delay
        _delay_ms(10);

    	// Check if '^' is aligned with 'X'
        if (next == r) {
            count++;
            if (count == 200) {
                lcd_moveto(1, 0);
                lcd_stringout("You Won!");
                break;
            } else {
                count = 0;
            }
        }

    }

    return 0;   /* never reached */
}

void rand_init()
{
    int seed = 0;
    unsigned char i, j, x;

    // Build a 15-bit number from the LSBs of an ADC
    // conversion of the channels 1-5, 3 times each
    for (i = 0; i < 3; i++) {
	for (j = 1; j < 6; j++) {
	    x = adc_sample(j);
	    x &= 1;	// Get the LSB of the result
	    seed = (seed << 1) + x; // Build up the 15-bit result
	}
    }
    srand(seed);	// Seed the rand function
}
