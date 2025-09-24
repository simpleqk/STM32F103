#include "stm32f10x.h"                  // Device header
#include "LED.h"

//PA8 connect the LED0
//PD2 connect the LED1
//0 - turn on LED
#define LED0 GPIO_Pin_8 //PA8
#define LED1 GPIO_Pin_2 //PD2

void LED_Init(void)
{
	//LED0
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; //push-pull
	GPIO_InitStruct.GPIO_Pin = LED0; //PA8
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_SetBits(GPIOA, LED0);
	
	//LED1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = LED1; //PD2
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	GPIO_SetBits(GPIOD, LED1);
}

void LED_On(enum LED_NO led)
{
	switch(led)
	{
		case ELED_0:
			GPIO_ResetBits(GPIOA, LED0); //turn on LED0
			break;
		case ELED_1:
			GPIO_ResetBits(GPIOD, LED1); //turn on LED1
			break;
		case ELED_ALL:
			GPIO_ResetBits(GPIOA, LED0); //turn on LED0
			GPIO_ResetBits(GPIOD, LED1); //turn on LED1
			break;
		default:
			break;
	}
}

void LED_Off(enum LED_NO led)
{
	switch(led)
	{
		case ELED_0:
			GPIO_SetBits(GPIOA, LED0); //turn off LED0
			break;
		case ELED_1:
			GPIO_SetBits(GPIOD, LED1); //turn off LED1
			break;
		case ELED_ALL:
			GPIO_SetBits(GPIOA, LED0); //turn off LED0
			GPIO_SetBits(GPIOD, LED1); //turn off LED1
			break;
		default:
			break;
	}
}
