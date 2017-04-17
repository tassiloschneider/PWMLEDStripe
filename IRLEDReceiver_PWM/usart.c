#include "usart.h"



ISR(USART_UDRE_vect)
{
	if(txbuffer_wpos!=txbuffer_rpos)
	{
		UDR=buffer(TXREADANDINCREMENT,0,0);
	}
	else
	{
		UCSRB &= ~(1<<UDRIE);
	}
}


ISR(USART_RXC_vect)
{

	buffer(RXWRITEANDINCREMENT,UDR,0);

}
void usart_init( unsigned int ubrr)
{
/* Set baud rate */
UBRRH = (unsigned char)(ubrr>>8);
UBRRL = (unsigned char)ubrr;
/* Enable receiver and transmitter */
UCSRB = (1<<RXEN)|(1<<RXCIE);
/* Set frame format: 8data, 2stop bit */
UCSRC = (1<<URSEL)|(3<<UCSZ0);

}

void USART_Transmit( unsigned char data )
{
	cli();
/* Wait for empty transmit buffer */
while ( !( UCSRA & (1<<UDRE)) )
;
/* Put data into buffer, sends the data */
UDR = data;
sei();
}

unsigned char USART_Receive( void )
{
/* Wait for data to be received */
while ( !(UCSRA & (1<<RXC)) )
;
/* Get and return received data from buffer */
return UDR;
}




