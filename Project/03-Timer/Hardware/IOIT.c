#include "stm32f10x.h"                  // Device header
#include "LED.h"

static uint16_t g_count;

void IOIT_Init(void)
{
	//configure external interupt
	
	//1. configure RCC
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,  ENABLE);
	
	//2. Initializes the GPIOx peripheral
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //GPIO_Mode_IN_FLOATING; //GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //PA1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//3. Selects the GPIO pin used as EXTI Line
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);
	
	//4. Initializes the EXTI peripheral
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStructure);
	
	//5. configure NVIC
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
}

void EXTI1_IRQHandler(void)
{
	if (SET == EXTI_GetITStatus(EXTI_Line1))
	{
//		static uint8_t flag=0;
//		if (0 == (flag++ % 2))
//		{
//			LED_On(ELED_0);
//		}
//		else
//		{
//			LED_Off(ELED_0);
//		}
		
		g_count++;
		
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

uint16_t IOIT_GetCount(void)
{
	return g_count;
}
