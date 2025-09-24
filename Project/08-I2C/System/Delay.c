#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void Delay_us(uint32_t us)
{
	SysTick->LOAD = 72 * us; //set timer reload value
	SysTick->VAL = 0x00;     //set timer init value
	SysTick->CTRL = 0x00000005; //set timer source is HCLK, and start timer
	while( !(SysTick->CTRL & 0x00010000) ); //wait the counter equal to 0
	SysTick->CTRL = 0x00000004; //close timer
}

void Delay_ms(uint32_t ms)
{
	while(ms--)
	{
		Delay_us(1000);
	}
}

void Delay_s(uint32_t s)
{
	while(s--)
	{
		Delay_ms(1000);
	}
}
