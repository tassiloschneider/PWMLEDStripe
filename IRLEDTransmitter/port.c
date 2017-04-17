#include "port.h"


void port_init()
{
	MCUCR |=	(1<<ISC00);
	GICR	|=	(1<<INT0);
	DDRB |=	(1<<PB1);
	DDRD |=	(1<<PD1);
}
