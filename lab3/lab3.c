/********************************************
*
*  Name: Nicolas Neven
*  Email: nneven@usc.edu
*  Lab section: 12:30pm Friday
*  Assignment: Lab3
*
********************************************/

#include <avr/io.h>
#include <util/delay.h>

#define DOT_LENGTH  250         /* Define the length of a "dot" time in msec */

void dot(void);
void dash(void);
void makeOutput(char);
char checkInput(char);

int main(void)
{

    // Initialize appropriate DDR registers
    DDRB &= ~(1 << 3);
    DDRB &= ~(1 << 4);
    DDRB &= ~(1 << 5);
	
    // Initialize the LED output to 
    DDRD |= (1 << 2);
	
    // Enable the pull-up resistors for the 
    // 3 button inputs
    PORTB |= (1 << 3);
    PORTB |= (1 << 4);
    PORTB |= (1 << 5);
	

    // Loop forever
    while (1) {      
    
  	//  Use if statements and the checkInput()
  	//  function to determine if a button
  	//  is being pressed and then output
  	//  the correct dot/dash sequence by
  	//  calling the dot(), dash(), and 
  	//  using appropriate delay functions

      if (checkInput(5)) {
        dot();
        dot();
        dash();
      }

      if (checkInput(4)) {
        dot();
        dot();
        dot();
      }

      if (checkInput(3)) {
        dash();
        dot();
        dash();
        dot();
      }

    }
    return 0;   /* never reached */
}

/*
  makeOutput() - Changes the output bit to either a zero 
  or one, based on the input argument "value".
  
  If the argument is zero, turn the output OFF,
  otherwise turn the output ON. 
  
  Do not use any delays here.  Just use bit-wise operations
  to make the appropriate PORT bit turn on or off.
*/
void makeOutput(char value)
{
  if (value == 0) {
    PORTD &= ~(1 << 2);
  } else {
    PORTD |= (1 << 2);
  }
}

/*
  dot() - Makes the output LED blink a "dot".

  Write code to generate a dot by using the makeOutput function
  to turn the output ON and OFF with appropriate delays.
  Be sure you don't forget to also delay an
  appropriate time after you turn the output off
*/
void dot()
{
  makeOutput(1);
  _delay_ms(DOT_LENGTH);
  makeOutput(0);
  _delay_ms(DOT_LENGTH);
}

/*
  dash() - Makes the output LED blink a "dash".

  Write code to generate a dash by using the makeOutput function
  to turn the output ON and OFF with appropriate delays.
  Be sure you don't forget to also delay
  appropriate time after you turn the output off
*/
void dash()
{
  makeOutput(1);
  _delay_ms(3 * DOT_LENGTH);
  makeOutput(0);
  _delay_ms(DOT_LENGTH);
}

/*
  checkInput(bit) - Checks the state of the input bit specified by the
  "bit" argument (0-7), and returns either 0 or 1 depending on its state.
  
  Write code to use the appropriate group's PIN register and determine if  
  the specified bit (which is passed as the argument) of that group is
  pressed or not.  Think carefully about what bit value means "pressed"
  when connected to a pushbutton.
 */
char checkInput(char bit)
{
  if (PINB & (1 << bit)) return 0;
  else return 1;
}
