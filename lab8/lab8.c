/********************************************
 *
 *  Name: Nicolas Neven
 *  Email: nneven@usc.edu
 *  Section: 12:30 Friday
 *  Assignment: Lab 8 - Digital Stopwatch
 *  56 || 106
 ********************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lcd.h"
#include "adc.h"

unsigned char state = 0;
volatile unsigned char tens = 0;
volatile unsigned char ones = 0;
volatile unsigned char tenths = 0;
volatile unsigned char tensFlag = 0;
volatile unsigned char onesFlag = 0; 

void init_timer(unsigned short m)
{
    TCCR1B |= (1 << WGM12);
    TIMSK1 |= (1 << OCIE1A);
    OCR1A = m;
    TCCR1B |= (1 << CS12);
}

bool inRange(int low, int high) {
    unsigned char v = adc_sample(PC0);
    return (low <= v) && (v <= high);
}

void debounce(int low, int high) {
    _delay_ms(10);
    while (inRange(low, high)) {} ;
    _delay_ms(10);
}

void displayTime() {
    if (tensFlag) {
        lcd_moveto(0, 0);
        lcd_writedata(tens + '0');
        tensFlag = 0;
    }

    if (onesFlag) {
        lcd_moveto(0, 1);
        lcd_writedata(ones + '0');
        onesFlag = 0;
    }

    lcd_moveto(0, 3);
    lcd_writedata(tenths + '0');
}

int main(void)
{
    init_timer(15625);
    sei();

    DDRC &= ~(1 << PC0);
    PORTC |= (1 << PC0);

    adc_init();
    lcd_init();
    lcd_writecommand(1);
    lcd_stringout("Lab 8");
    lcd_moveto(1, 0);
    lcd_stringout("Nicolas Neven");
    _delay_ms(1000);
    lcd_writecommand(1);

    while(1) {
        // do something w/ qsecs

        if (state == 0) {
            if (inRange(100, 110)) {
                debounce(100, 110);
                // RESET
                tens = 0;
                ones = 0;
                tenths = 0;
                lcd_moveto(0, 0);
                lcd_writedata(tens + '0');
                lcd_writedata(ones + '0');
                lcd_moveto(0, 2);
                lcd_stringout(".");
                lcd_moveto(0, 3);
                lcd_writedata(tenths + '0');
                lcd_moveto(0, 5);
                lcd_stringout("seconds");

            } else if (inRange(50, 60)) {
                debounce(50, 60);
                state = 1;
                // TURN ON
                TCCR1B |= (1 << CS10) | (1 << CS11);
            }

        } else if (state == 1) {
            // DISPLAY TIME
            displayTime();

            if (inRange(50, 60)) {
                debounce(50, 60);
                state = 0;
                // TURN OFF
                TCCR1B &= ~((1 << CS10) | (1 << CS11) | (1 << CS12));

            } else if (inRange(100, 110)) {
                debounce(100, 110);
                state = 2;
            }

        } else {
            if (inRange(50, 60)) {
                debounce(50, 60);
                state = 1;
            } else if (inRange(100, 110)) {
                debounce(100, 110);
                state = 1;
            }
        }
    }

    return 0;   /* never reached */
}

ISR(TIMER1_COMPA_vect)
{
    if (state > 0) {

        tenths++;

        if (tenths > 9) {
            tenths = 0;
            ones++;
            onesFlag = 1;
        }

        if (ones > 9) {
            ones = 0;
            tens++;
            tensFlag = 1;
        }

        if (tens > 5) {
            tens = 0;
            ones = 0;
            tenths = 0;
        }
    }
}
