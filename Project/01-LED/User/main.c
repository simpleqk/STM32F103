#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LED.h"
#include "Key.h"

int main()
{
	uint8_t key;
	
	/*
	RCC->APB2ENR = 0x00000004; //enable IO port A clock
	GPIOA->CRH = 0x00000003; //output push-pull and 55MHz mode
	GPIOA->ODR = 0x0000000000; //open LED0
	////GPIOA->ODR = 0x0000010000; //close LED0
	*/
	LED_Init();
	
	Key_Init();
		
	while(1)
	{
		key = Key_GetState();
		switch(key)
		{
			case 1:
				LED_On(ELED_0);
				Delay_ms(100);
				break;
			case 2:
				LED_On(ELED_1);
				Delay_ms(100);
				break;
			case 3:
				LED_On(ELED_ALL);
				Delay_ms(100);
				break;
			default:
				LED_On(ELED_ALL);
				Delay_ms(500);
				LED_Off(ELED_ALL);
				Delay_ms(500);
				break;
		}
	};
	
}
