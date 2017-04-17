#include "serial_streaming.h"

volatile uint8_t txbuffer[TXBUFFERSIZE];
volatile uint8_t txbuffer_rpos=0;
volatile uint8_t txbuffer_wpos=0;

volatile uint8_t rxbuffer[RXBUFFERSIZE];
volatile uint8_t rxbuffer_rpos=0;
volatile uint8_t rxbuffer_wpos=0;

uint8_t buffer(int8_t action, uint8_t byte, uint8_t pos)
{
	uint8_t crement(uint8_t pos, int8_t steps, uint8_t size)
	{
		for(;steps!=0;)
		{
			if(steps<0)
			{
				if(pos==0)
				{
					pos=size;
					steps++;
				}
				else
				{
					pos--;
					steps++;
				}
			}
			else 
			{
				if(pos==size)
				{
					pos=0;
					steps--;
				}
				else
				{
					pos++;
					steps--;
				}
			}
		}
		return pos;
	}
	
	switch (action)
	{
		case RXREADWITHOUTINCREMENT:
		{
			return rxbuffer[crement(rxbuffer_rpos,pos, RXBUFFERSIZE)];
		}
		
		case RXREADANDINCREMENT:
		{
			rxbuffer_rpos=crement(rxbuffer_rpos,pos+1, RXBUFFERSIZE);
			return rxbuffer[crement(rxbuffer_rpos,-1, RXBUFFERSIZE)];
		}
		
		case RXWRITEWITHOUTINCREMENT:
		{
			rxbuffer[crement(rxbuffer_wpos,pos, RXBUFFERSIZE)]=byte;
			break;
		}
		
		case RXWRITEANDINCREMENT:
		{
			rxbuffer[crement(rxbuffer_wpos,pos, RXBUFFERSIZE)]=byte;
			rxbuffer_wpos=crement(rxbuffer_wpos,1, RXBUFFERSIZE);
			break;
		}
		case TXREADWITHOUTINCREMENT:
		{
			return txbuffer[crement(txbuffer_rpos,pos, TXBUFFERSIZE)];
			break;
		}
		
		case TXREADANDINCREMENT:
		{
			txbuffer_rpos=crement(txbuffer_rpos,pos+1, TXBUFFERSIZE);
			return txbuffer[crement(txbuffer_rpos,-1, TXBUFFERSIZE)];
			break;
		}
		
		case TXWRITEWITHOUTINCREMENT:
		{
			txbuffer[crement(txbuffer_wpos,pos, TXBUFFERSIZE)]=byte;
			break;
		}
		
		case TXWRITEANDINCREMENT:
		{
			txbuffer[crement(txbuffer_wpos,pos, TXBUFFERSIZE)]=byte;
			txbuffer_wpos=crement(txbuffer_wpos,1, TXBUFFERSIZE);
			break;
		}

	}
	return 0;
}

void send_serial_data(uint8_t data[], uint8_t length)
{ 
		uint8_t dataarray[TXBUFFERSIZE];

		for(uint8_t i=0;i<length;i++)
		{
			dataarray[i]=*(data+i);
		}
		
		convert_to_frame(dataarray, &length);

		//fill buffer
		for(uint8_t i=0; i<length;i++)
		{
			buffer(TXWRITEANDINCREMENT,dataarray[i],0);
		}
}

void checktxbuffer()
{
	if(txbuffer_wpos!=txbuffer_rpos) //txdata not empty
	{
		//start send
		UCSRB |= (1<<UDRIE);
		cli();
		ISR(USART_UDRE_vect);
		sei();
	}
}

struct receive_message check_serial_data(void)
{
	receive_message valid_message=fetch_valid_message();
	if(valid_message.status == 5)
	{
		uint16_t crc;
		convert_to_data(valid_message.data, &valid_message.length);
		crc= crc16(valid_message.data, valid_message.length-2);

		if(((crc>>8) == valid_message.data[valid_message.length-2]) && (((uint8_t)crc) == valid_message.data[valid_message.length-1]))
		{
			valid_message.status=6;
			valid_message.length-=2;
		}
	}

	return valid_message;
}

struct receive_message fetch_valid_message(void)
{

	static receive_message message={.status=0, .length=0, .data[RXBUFFERSIZE]=0};
	if(message.status==5) message.status=0;

	while((rxbuffer_rpos != rxbuffer_wpos) || (message.status==4))
	{
		switch (message.status)
		{	
			case 0: //init
			{
				message.status=1;
				message.length=0;
				break;
			}
			case 1:	//find start byte
			{
				message.data[message.length]=buffer(RXREADANDINCREMENT,0,0); //write new byte to message
				if(message.data[message.length]==0x7E) //start byte found
				{
					message.status=2;
					message.length++;	
				} 
				break;
			}
			case 2: //check if startbyte is valid
			{
				message.data[message.length]=buffer(RXREADANDINCREMENT,0,0); //write new byte to message
				if(message.data[message.length]!=0x7E && message.data[message.length]!=0x7D) //check if byte is data (valid start condition)
				{
					message.status=3;
					message.length++;	
				} 
				else	//no valid start  condition
				{
					message.status=0;
					message.length=0;
				}
				break;
			}
			case 3:	//add byte to message until stop byte is found
			{
				message.data[message.length]=buffer(RXREADANDINCREMENT,0,0); 
				if(message.data[message.length]==0x7E)
				{
					message.length++;
					message.status=4;
				}
				else message.length++;
				break;
			}
			case 4:	//check if stop byte is a stop condition or data
			{
				static uint8_t i=0;
				for(;i<(10000/BAUD+1);i++) //wait max 10bits long
				{	
					if((rxbuffer_rpos != rxbuffer_wpos))
					{
						if(buffer(RXREADWITHOUTINCREMENT,0,0)==0x7D)
						{
							message.status=3;
							i=0;
							break;
						}
						else if(buffer(RXREADWITHOUTINCREMENT,0,0)==0x7E)
						{
							message.status=5;
							i=0;
							break;
						}
					}
					else
					{
						_delay_ms(1);
					}
				}
				message.status=5;
				i=0;
				return message;
				break;
			}
		}
	}
	return message;
}


uint16_t crc16(const uint8_t *data_p, uint8_t length)
{
    uint8_t x;
    uint16_t crc = 0xFFFF;

    while (length--)
	{
		x = crc >> 8 ^ *data_p++;
        x ^= x>>4;
        crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x <<5)) ^ ((uint16_t)x);
    }
    return crc;
}

void convert_to_frame(uint8_t *data, uint8_t *length)
{
	uint8_t pos=0;
	uint8_t buffer[255];

	//copy data to new buffer
	for(uint8_t i=0;i<*length;i++)
	{
		buffer[i]=*(data+i);
	}
	
	//function to instert data into the data and increment position of insertion
	void addbytetobuffer(uint8_t byte, uint8_t check_flag_esc)
	{
		*(data+pos)=byte; //add byte
		pos++;
	
		if(check_flag_esc)
		{
			if((byte==0x7E) || byte==0x7D)//start/stop/esc byte found
			{
				//insert exeption byte
				*(data+pos)=0x7D;
				pos++;
			}
		}	
	}
	
		//add startbyte
		addbytetobuffer(0x7E, 0);
		//add data-bytes from buffer-array back to *data
		for(uint8_t i=0;i<*length;i++)
		{
			addbytetobuffer( *(buffer+i), 1);
		}
		//add crc16
		addbytetobuffer((crc16(buffer, *length))>>8, 1);
		addbytetobuffer(crc16(buffer, *length), 1);
		//add stop byte
		addbytetobuffer(0x7E, 0);

		//update new length
		*length=pos;
}


void convert_to_data(uint8_t *data, uint8_t *length)
{
	uint8_t pos=1;
	uint8_t buffer[255];

	//copy frame to new buffer
	for(uint8_t i=0;i<*length;i++)
	{
		buffer[i]=*(data+i);
	}

	uint8_t getnextdata(void)
	{
		uint8_t byte=buffer[pos];
		uint8_t nextbyte=buffer[pos+1];
		
		if((byte != 0x7D) && (byte != 0x7E))  //data
		{
			pos++;
			return byte;
		} 
		else if(((byte == 0x7D) || (byte == 0x7E))  && (nextbyte == 0x7D)) //esc/startbyte as data
		{
			pos+=2;
			return byte;
		}
		else return 0;
	}
	
	uint8_t i=0;
	for(;pos+1<*length;i++)
	{
		*(data+i)=getnextdata();
	}
	*length=i;
}

