/********************************************
 *
 *  Name: Nicolas Neven
 *  Email: nneven@usc.edu
 *  Section: 12:30 Friday
 *  Assignment: Lab 9 - Serial Communications
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "lcd.h"
#include "adc.h"

// Serial communications functions and variables
void serial_init(unsigned short);
void serial_stringout(char *);
void serial_txchar(char);

#define FOSC 16000000           // Clock frequency
#define BAUD 9600               // Baud rate used
#define MYUBRR (FOSC/16/BAUD-1) // Value for UBRR0 register
#define ADC_CHAN 0              // Buttons use ADC channel 0

// ADC functions and variables
int size = 10;
int msgCnt = 0;
int received = 0;
int character = 0;
char message[17] = {' '};
char *messages[] = {
    "Hello           ",
    "How are you?    ",
    "I am doing well ",
    "I am sad        ",
    "I love EE109    ",
    "Fight On!       ",
    "Goodbye         ",
    "See you soon!   ",
    "What to say...  ",
    "Hahahahaha      "
};

bool inRange(int low, int high) {
    unsigned char v = adc_sample(ADC_CHAN);
    return (low <= v) && (v <= high);
}

void debounce(int low, int high) {
    _delay_ms(10);
    while (inRange(low, high)) {} ;
    _delay_ms(10);
}

int main(void) {

    // Initialize the LCD, ADC and serial modules
	DDRC &= ~(1 << PC0);
    PORTC |= (1 << PC0);
    adc_init();
    lcd_init();
    serial_init(MYUBRR);

    // Enable interrupts
	UCSR0B |= (1 << RXCIE0);
	sei();

    // Show the splash screen
	lcd_writecommand(1);
    lcd_stringout("Lab 9");
    lcd_moveto(1, 0);
    lcd_stringout("Nicolas Neven");
    _delay_ms(1000);
    lcd_writecommand(1);

    char clr[17] = {' '};
    lcd_moveto(0, 0);
    lcd_stringout(clr);
    lcd_moveto(0, 0);
    lcd_stringout(messages[msgCnt]);
    while (1) {                 // Loop forever

	// Get an ADC sample to read buttons

        // Up button pressed?
    	if (inRange(40, 60)) {
            debounce(40, 60);
            msgCnt--;
            if (msgCnt < 0) {
                msgCnt = size - 1;
            }
            char clr[17] = {' '};
		    lcd_moveto(0, 0);
		    lcd_stringout(clr);
		    lcd_moveto(0, 0);
		    lcd_stringout(messages[msgCnt]);
        }

        // Down button pressed?
        if (inRange(90, 110)) {
            debounce(90, 110);
            msgCnt++;
            if (msgCnt > size - 1) {
                msgCnt = 0;
            }
            char clr[17] = {' '};
		    lcd_moveto(0, 0);
		    lcd_stringout(clr);
		    lcd_moveto(0, 0);
		    lcd_stringout(messages[msgCnt]);
        }

        // Select button pressed?
        if (inRange(190, 210)) {
            debounce(190, 210);
            serial_stringout(messages[msgCnt]);
        }

        // Message received from remote device?
        if (received) {
            lcd_moveto(1, 0);
    		lcd_stringout(message);
            received = 0;
        }
    }
}

/* ----------------------------------------------------------------------- */

void serial_init(unsigned short ubrr_value)
{
	UBRR0 = ubrr_value;
    // Set up USART0 registers
	UCSR0B |= (1 << TXEN0 | 1 << RXEN0);
	UCSR0C = (3 << UCSZ00);
    // Enable tri-state buffer
	DDRD |= (1 << PD3);
	PORTD &= ~(1 << PD3);
}

void serial_txchar(char ch)
{
    while ((UCSR0A & (1 << UDRE0)) == 0);
    UDR0 = ch;
}

void serial_stringout(char *s)
{
    // Call serial_txchar in loop to send a string
    // (Very similar to lcd_stringout in lcd.c)
	int i = 0;
	while (s[i] != '\0') {
		serial_txchar(s[i]);
		i++;
	}
}

ISR(USART_RX_vect)
{
    // Handle received character
    message[character] = UDR0;
    character++;

    if (character == 16) {
        received = 1;
        character = 0;
    }
}
