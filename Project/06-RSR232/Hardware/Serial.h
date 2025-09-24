#ifndef __SERIAL_H_
#define __SERIAL_H_

#if (DUSE_MICROLIB)	
#include <stdio.h>
#endif//DUSE_MICROLIB

//USART1 communication function, support send and receive(loop or interupt)

void Serial_Init(uint32_t baudrate/*9600*/, uint8_t enableRxIT);

void Serial_SendByte(uint8_t data);
void Serial_SendArray(const uint8_t *array, uint16_t len);

int8_t Serial_ReceiveByte(uint8_t *byte);
int8_t Serial_ReceiveArray(uint8_t *byte, uint8_t *len);

#endif
