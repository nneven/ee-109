#include <avr/io.h>

#include "adc.h"


void adc_init(void)
{
    ADMUX |= (1 << REFS0);
    ADMUX &= ~(1 << REFS1);
    ADCSRA |= ((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0));
    ADMUX |= (1 << ADLAR);
    ADCSRA |= (1 << ADEN);

}

unsigned char adc_sample(unsigned char channel)
{
    ADMUX &= ~((1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0));
    ADMUX |= channel;
    ADCSRA |= (1 << ADSC);
    while ((ADCSRA & (1 << ADSC)) != 0) {}
    return ADCH;
}
