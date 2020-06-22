void init_timer();
ISR(PCINT1_vect);

volatile unsigned char nState, oState, sChanged, speed;
