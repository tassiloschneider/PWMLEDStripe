#ifndef _SERIAL_STREAMING_H_
#define _SERIAL_STREAMING_H_

#include "defines.h"


void send_serial_data(uint8_t data[], uint8_t length);
void convert_to_frame(uint8_t *data, uint8_t *length);
void convert_to_data(uint8_t *data, uint8_t *length);
void checktxbuffer();
uint8_t USART_Receive( void );
uint8_t buffer(int8_t action, uint8_t byte, uint8_t pos);
uint16_t crc16(const uint8_t *data_p, uint8_t length);
struct receive_message check_serial_data(void);
struct receive_message fetch_valid_message(void);



#endif
