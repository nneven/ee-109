volatile unsigned char sReceived, rStart, bCount;
volatile char buffer[5];

void init_serial();
void send(short data);
