#include "defines.h"

void timer1_init()
{
	TCCR1A |=	(1<<COM1A1)|(1<<COM1A1)|(1<<WGM11);
	TCCR1B	|=	(1<<WGM12)|(1<<WGM13)|(1<<CS10);
	ICR1=800;
	OCR1A = 400;
	usart_init(MYUBRR);
}
