#include <avr/io.h>

#include "adc.h"


void adc_init(void)
{
    // Initialize the ADC
    ADMUX |= (1 << REFS0);
    ADMUX &= ~(1 << REFS1);
    ADCSRA |= ((1 << ADPS2));
    ADCSRA |= ((1 << ADPS1));
    ADCSRA |= ((1 << ADPS0));
    ADMUX &= ~((1 << MUX3));
    ADMUX &= ~((1 << MUX2));
    ADMUX &= ~((1 << MUX1));
    ADMUX &= ~((1 << MUX0));
    ADMUX |= (1 << ADLAR);
    ADCSRA |= (1 << ADEN);

}

unsigned char adc_sample(unsigned char channel)
{
    ADMUX |= (1 << channel);

    // Convert an analog input and return the 8-bit result

    ADCSRA |= (1 << ADSC);
    while ((ADCSRA & (1 << ADSC)) != 0) {};
    return ADCH;

}
