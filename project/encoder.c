 #include <avr/interrupt.h>
#include "encoder.h"

void init_timer() {
	PCICR |= (1 << PCIE1);
	PCMSK1 |= (1 << PCINT12 | 1 << PCINT13);
	sei();

	PORTC |= (1 << PC5 | 1 << PC4);

	// INITIAL STATE
	unsigned char bits, a, b;
	bits = PINC;
	a = bits & (1 << PC4);
	b = bits & (1 << PC5);

	if (!b && !a) {
		oState = 0;
	} else if (!b && a) {
		oState = 1;
	} else if (b && !a) {
		oState = 2;
	} else {
		oState = 3;
	}

	nState = oState;
}

ISR(PCINT1_vect) {
	unsigned char bits, a, b;
	bits = PINC;
	a = bits & (1 << PC4);
	b = bits & (1 << PC5);

	if (oState == 0) {
		if (a) {
			nState = 1;
			speed++;
		} else if (b) {
			nState = 2;
			speed--;
		}
	} else if (oState == 1) {
		if (!a) {
			nState = 0;
			speed--;
		} else if (b) {
			nState = 3;
			speed++;
		}
	}
	else if (oState == 2) {
		if (a) {
			nState = 3;
			speed--;
		}
		else if (!b) {
			nState = 0;
			speed++;
		}
	} else if (oState == 3) {
		if (!a) {
			nState = 2;
			speed++;
		} else if (!b) {
			nState = 1;
			speed--;
		}
	}

	if (speed > 99) {
		speed = 99;
	} else if (speed < 1) {
		speed = 1;
	}

	oState = nState;
	sChanged = 1;
}
