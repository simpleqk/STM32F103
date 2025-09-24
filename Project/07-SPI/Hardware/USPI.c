#include "stm32f10x.h"                  // Device header

//GPIO alternate function mapping
//SP1_NSS  <-> PA4, Note the circuit diagram connects PA2 in my experimental environment
//SP1_SCK  <-> PA5
//SP1_MISO <-> PA6
//SP1_MOSI <-> PA7

//SP2
//SP3

//IO mode configure
//SPx_NSS  -> push-pull;
//SPx_SCK  -> Master, alternate push-pull; Slave, floating input;
//SPx_MISO -> Master, floating input / push-pull input; Slave, alternate push-pull(p2p) / open-drain(multi-slave);
//SPx_MOSI -> Master, alternate push-pull; Slave, floating input;

static void USPI_W_SS(uint8_t value)
{
	//software simulate NSS, 0 indicate the device was selected
	GPIO_WriteBit(GPIOA, GPIO_Pin_2, (BitAction)value);
}

void USPI_Init(void)
{
	//Enables or disables the High Speed APB (APB2) peripheral
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	//Initializes the GPIOx peripheral
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	//Initializes the SPIx peripheral
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master; //SPI_Mode_Master / SPI_Mode_Slave
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //SPI_Direction_2Lines_FullDuplex / SPI_Direction_2Lines_RxOnly / ...
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; //SPI_DataSize_8b / SPI_DataSize_16b
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; //SPI_FirstBit_MSB / SPI_FirstBit_LSB
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128; //FSCL / scaler. SPI_BaudRatePrescaler_2 / SPI_BaudRatePrescaler_4 / ...
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge; //Specifies the clock active edge for the bit capture. SPI_CPHA_1Edge / SPI_CPHA_2Edge
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low; //Specifies the serial clock steady state (SPI_Clock_Polarity). SPI_CPOL_Low / SPI_CPOL_High
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; //SPI_NSS_Soft / SPI_NSS_Hard
	SPI_InitStructure.SPI_CRCPolynomial = 7; //not use. for the CRC calculation
	SPI_Init(SPI1, &SPI_InitStructure);
	
	//Enables or disables the specified SPI peripheral
	SPI_Cmd(SPI1, ENABLE);
	
	USPI_W_SS(1);
}

uint8_t USPI_SwapByte(uint8_t byte)
{
	//wait state to send
	while (SET != SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
	
	//the time sequence driven by send operation, must first execute
	SPI_I2S_SendData(SPI1, byte); //at the same swapped slave data
	
	//wait state to read
	while (SET != SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
	
	return SPI_I2S_ReceiveData(SPI1);
}

void USPI_Start(void)
{
	USPI_W_SS(0);
}

void USPI_Stop(void)
{
	USPI_W_SS(1);
}
