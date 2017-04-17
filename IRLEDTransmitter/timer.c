#include "timer.h"


void timer1_init()
{
	//TIMER1
	TCCR1A |=	(1<<COM1A0)|(1<<COM1A1)|(1<<WGM11);
	TCCR1B	|=	(1<<WGM12)|(1<<WGM13);
	//TIMSK	|=	(1<<TOIE1);
	ICR1	=	TIMER1_TOP;
	OCR1A 	=	ICR1/2;
	TCCR1B	|=	(1<<CS10);

}
