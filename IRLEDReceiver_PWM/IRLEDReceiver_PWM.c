#include 	"defines.h"



int main(void)
{

	port_init();
	timer1_init();
	usart_init(MYUBRR);
	receive_message new_message;
	uint16_t ocr1a_temp=50;
	sei();
	
	while(1)
	{
		_delay_ms(100);


		new_message=check_serial_data();

		if(new_message.status==6)
		{
			if((new_message.data[0]==ID) && (new_message.data[1]==10))
			{
				ocr1a_temp = (new_message.data[2]<<8);
				ocr1a_temp = new_message.data[3];
			}
			else
			{
				//ocr1a_temp=50;
			}
		}
		else
		{
			ocr1a_temp=new_message.status;
		}
ocr1a_temp=1;
		if(ocr1a_temp<=ICR1)
		{
			OCR1A=ocr1a_temp;
		}
		else
		{
			OCR1A=800;
		}
	}
}
