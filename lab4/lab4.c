/********************************************
*
*  Name: Nicolas Neven
*  Email: nneven@usc.edu
*  Section: 12:30 Friday
*  Assignment: Lab 4 - Up/Down counter on LCD display
*
********************************************/

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
        
        // Use a state machine approach to organize your code
        //   - Declare and initialize a variable to 
        //     track what state you are in

        char num = 0;
        char state = 1;
        char count = 0;
        
    while (1) {     // Loop forever

            if((PINC & (1 << PC2)) == 0){
                state = 0;
            }

            if((PINC & (1 << PC4)) == 0){
                state = 1;
            }
            // Use an outer if statement to select what state you are in
            // Then inside each 'if' or 'else' block, perform the desired
            // output operations and then sample the inputs to update
            // the state for the next iteration
            count++;
            if (count >= 10) {
                if (state == 1) {
                    num++;
                    if (num > 9) num = 0;
                    lcd_writecommand(1);
                    lcd_writedata(num + '0');
                }

                if (state == 0) {
                    num--;
                    if (num < 0) num = 9;
                    lcd_writecommand(1);
                    lcd_writedata(num + '0');
                    
                }
                count = 0;
            }  
            // Delay before we go to the next iteration of the loop
            
            _delay_ms(50);
                
    }

    return 0;   /* never reached */
}

