#include "stm32f10x.h"                  // Device header
#include "Key.h"
#include "Delay.h"

//PC1  connect KEY0
//PC13 connect KEY1
#define KEY0 GPIO_Pin_1
#define KEY1 GPIO_Pin_13

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	//input mode
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU; //pull-up
	GPIO_InitStruct.GPIO_Pin = KEY0 | KEY1;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOC,  &GPIO_InitStruct);
}

static uint8_t IsKeyPressed(uint16_t pin)
{
	uint8_t state = 0;
	
	if(0 == GPIO_ReadInputDataBit(GPIOC, pin))
	{
		Delay_ms(20);

		state  = (0 == GPIO_ReadInputDataBit(GPIOC, pin));
		state &= (0 == GPIO_ReadInputDataBit(GPIOC, pin));
		state &= (0 == GPIO_ReadInputDataBit(GPIOC, pin));

		////while(0 == GPIO_ReadInputDataBit(GPIOC, pin));

		Delay_ms(20);
	}
	
	return state;
}

uint8_t Key_GetState(void)
{
	uint8_t state = 0;
	
	if(1 == IsKeyPressed(KEY0))
	{
		state = 1;
	}
	
	if(1 == IsKeyPressed(KEY1))
	{
		state |= 2;
	}

	return state;
}
