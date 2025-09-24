#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED2.h"


int main()
{
	LED_Init();
	LED_On(ELED_1);
	
	OLED_Init();
	OLED_Fill(0xff);
	Delay_s(1);
	OLED_Clear();
	Delay_s(1);

	OLED_DisplayStr(2,0,"I2C Mode", EOLED_FONT_6);
	
	while(1)
	{
		Delay_s(1);
	};
	
}
