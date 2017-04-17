#include 	"defines.h"
#include <util/delay.h>

const uint16_t pwmtable[43] PROGMEM =
{
0,1,2,3,4,5,6,7,8,9,10,12,13,15,18,20,23,26,30,35,40,46,52,60,69,79,90,103,118,
136,156,178,204,234,269,308,353,404,464,531,609,698,800
};

ISR(INT0_vect)	//switch modulated signal on/off
{	
	TCCR1B	^=	(1<<CS10);
	TCNT1 = 112;			//set 
}


int main(void)
{

	port_init();
	timer1_init();
	usart_init(MYUBRR);
	uint8_t testcounter = 0;
	uint16_t timervalue=0;
	uint8_t state=1;

	uint8_t array[10];
	sei();

	while(1)
	{
		_delay_ms(250);

		
		timervalue	=	pgm_read_word (& pwmtable[testcounter]);


		array[0]=ID;
		array[1]=10;
		array[2]=timervalue>>8;
		array[3]=timervalue;
		uint8_t length=4;


		send_serial_data(array, length);
		checktxbuffer();

		if(state)
		{
			testcounter++;
			if(testcounter>=42)
			{
				state=0;
			}
		}
		else
		{
			testcounter--;
			if(testcounter==0)
			{
				state=1;
			}
		}

		

	}


}



