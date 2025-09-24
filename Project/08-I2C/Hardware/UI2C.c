#include "stm32f10x.h"                  // Device header


//GOIO Mapping
//I2C1_SCL  <->  PB6
//I2C1_SDA  <->  PB7

//I2C2_SCL  <->  PB10
//I2C2_SDA  <->  PB11

#ifdef DUSE_I2C1
#define I2CX I2CX
#define I2CX_PERIPH RCC_APB1Periph_I2C1
#define I2CX_SCL GPIO_Pin_6
#define I2CX_SDA GPIO_Pin_7
#else
#define I2CX I2C2
#define I2CX_PERIPH RCC_APB1Periph_I2C2
#define I2CX_SCL GPIO_Pin_10
#define I2CX_SDA GPIO_Pin_11
#endif//DUSE_I2C1


void UI2C_Init(void)
{
	RCC_APB1PeriphClockCmd(I2CX_PERIPH, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	//Initializes the GPIOx peripheral
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin = I2CX_SCL | I2CX_SDA;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//Initializes the I2Cx peripheral
	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_ClockSpeed = 200000; //<400kHz
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; //stm32 as slave
	I2C_InitStructure.I2C_OwnAddress1 = 0x00; //address stm32 as slave
	I2C_Init(I2CX, &I2C_InitStructure);
	
	I2C_Cmd(I2CX, ENABLE);
}

void UI2C_Write(uint8_t slave_address, uint8_t data_address, uint8_t *data, uint8_t len)
{
	uint8_t i;
	
	I2C_GenerateSTART(I2CX, ENABLE);
	while(SUCCESS != I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_MODE_SELECT));
	
	//address
	I2C_Send7bitAddress(I2CX, slave_address, I2C_Direction_Transmitter);
	while(SUCCESS != I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
	//data
	I2C_SendData(I2CX, data_address);
	while(SUCCESS != I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTING));
	
	for (i=0; i<len; i++)
	{
		I2C_SendData(I2CX, data[i]);
		
		if (i < len-1)
		{
			//send data continuely
			while(SUCCESS != I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTING));
		}
		else
		{	
			//send last data
			while(SUCCESS != I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
		}
	}
		
	I2C_GenerateSTOP(I2CX, ENABLE);
}

void UI2C_Read(uint8_t slave_address, uint8_t data_address, uint8_t *data, int len)
{
	uint8_t i;
	
	I2C_GenerateSTART(I2CX, ENABLE);
	while(SUCCESS != I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(I2CX, slave_address, I2C_Direction_Transmitter);
	while(SUCCESS != I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
	I2C_SendData(I2CX, data_address);
	while(SUCCESS != I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	
	//begin receive
	I2C_GenerateSTART(I2CX, ENABLE);
	while(SUCCESS != I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(I2CX, slave_address, I2C_Direction_Receiver);
	while(SUCCESS != I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
	for (i=0; i<len; i++)
	{
		if (len-1 == i)
		{
			//will receive the last data
			I2C_AcknowledgeConfig(I2CX, DISABLE); //Enables or disables the specified I2C acknowledge feature
			I2C_GenerateSTOP(I2CX, ENABLE);
		}

		while(SUCCESS != I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_BYTE_RECEIVED));
		data[i] = I2C_ReceiveData(I2CX);
	}
	
	//restore default ack function
	I2C_AcknowledgeConfig(I2CX, ENABLE);
}

