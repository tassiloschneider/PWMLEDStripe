#ifndef _DEFINES_H_
#define _DEFINES_H_

#ifndef F_CPU
#define F_CPU 8000000
#endif

#define ID 93
#define BAUD 2400
#define MYUBRR F_CPU/16/BAUD-1
#define MODULATOR_FREQ	36000
#define TIMER1_TOP F_CPU/MODULATOR_FREQ-1;
#define RXBUFFERSIZE 100
#define TXBUFFERSIZE 50

#define RXREADWITHOUTINCREMENT	-1
#define RXREADANDINCREMENT		-2
#define RXWRITEWITHOUTINCREMENT	-3
#define RXWRITEANDINCREMENT		-4

#define TXREADWITHOUTINCREMENT	0
#define TXREADANDINCREMENT		1
#define TXWRITEWITHOUTINCREMENT	2
#define TXWRITEANDINCREMENT		3

#include	<avr/io.h>
#include	<avr/interrupt.h>
#include 	<avr/pgmspace.h>
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<util/delay.h>
#include	"usart.h"
#include	"port.h"
#include 	"timer.h"
#include 	"serial_streaming.h"
#include 	"globals.h"





#endif
