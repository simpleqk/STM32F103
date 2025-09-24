#ifndef __USPI_H_
#define __USPI_H_

//SPI1 send/read data function

void USPI_Init(void);

void USPI_Start(void);
void USPI_Stop(void);

uint8_t USPI_SwapByte(uint8_t byte);

#endif
