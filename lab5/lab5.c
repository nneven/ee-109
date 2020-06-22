/********************************************
*
*  Name: Nicolas Neven
*  Email: nneven@usc.edu
*  Section: 12:30 Friday
*  Assignment: Lab 5 - Number Lock
*
********************************************/

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lcd.h"

int main(void) {

        // Setup DDR and PORT bits for the 2 input buttons
        // as necessary
        DDRC &= ~(1<<PC2 | 1<<PC4);
        PORTC |= (1 << PC2);
        PORTC |= (1 << PC4);

        // Initialize the LCD
        lcd_init();
        lcd_writecommand(1);

        /* Call lcd_stringout to print out your name */
        lcd_stringout("Nicolas N");

        /* Use snprintf to create a string with your birthdate */
        char buf[17];
        char month, day;
        int year;
        month = 11;
        day = 23;
        year = 1998;
        snprintf(buf, 17, "%d/%d/%d", month, day, year);

        /* Use lcd_moveto to start at an appropriate column in the
           bottom row to appear centered */
        lcd_moveto(1, 0);

        /* Use lcd_stringout to print the birthdate string */
        lcd_stringout(buf);

        /* Delay 1 second */
        _delay_ms(1000);
        lcd_writecommand(1);
        
        // Use a state machine approach to organize your code
        //   - Declare and initialize a variable to 
        //     track what state you are in

        char button;
        char state = 0;
        char * status = "Locked";
        
    while (1) {     // Loop forever

            if((PINC & (1 << PC2)) == 0) {
                button = 'A';
                _delay_ms(5);
            	while ((PINC & (1 << PC2)) == 0) {}
            	_delay_ms(5);
            } else if((PINC & (1 << PC4)) == 0) {
                button = 'B';
                _delay_ms(5);
            	while ((PINC & (1 << PC4)) == 0) {}
            	_delay_ms(5);
            } else button = 0;

            // Use an outer if statement to select what state you are in
            // Then inside each 'if' or 'else' block, perform the desired
            // output operations and then sample the inputs to update
            // the state for the next iteration
            if (button != 0) {
                if (state == 0) {
                    if (button == 'A') state = 0;
                    else state = 1;
                } else if (state == 1) {
                    if (button == 'A') state = 2;
                    else state = 1;
                } else if (state == 2) {
                    if (button == 'A') state = 3;
                    else state = 1;
                } else if (state == 3) {
                    if (button == 'A') state = 0;
                    else {
                    	state = 4; status = "Unlock";
                    }
                } else if (state == 4) {
                    if (button == 'A' || button == 'B') {
                        state = 0;
                        status = "Locked";
                    } 
                }

                char m1[17];
                snprintf(m1, 17, "State = S%d", state);

                char m2[17];
                snprintf(m2, 17, "Status = %s", status);

                lcd_moveto(0, 0);
                lcd_stringout(m1);
                lcd_moveto(1, 0);
                lcd_stringout(m2);

                _delay_ms(100);
            }
            
            // OUTPUT

            // Delay before we go to the next iteration of the loop
                
    }

    return 0;   /* never reached */
}

