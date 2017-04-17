#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "defines.h"

extern volatile uint8_t txbuffer[TXBUFFERSIZE];
extern volatile uint8_t txbuffer_rpos;
extern volatile uint8_t txbuffer_wpos;

extern volatile uint8_t rxbuffer[RXBUFFERSIZE];
extern volatile uint8_t rxbuffer_rpos;
extern volatile uint8_t rxbuffer_wpos;

typedef struct receive_message
{
	uint8_t status;
	uint8_t length;
	uint8_t data[RXBUFFERSIZE+1];
}receive_message;

#endif
