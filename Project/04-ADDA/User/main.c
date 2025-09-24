#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "AD.h"

int main()
{	
	LED_Init();
	LED_On(ELED_1);
	
	OLED_Init();
		
	OLED_Fill(0xff);
	Delay_s(1);
	OLED_Clear();
	Delay_s(1);
	
	OLED_DisplayCN(2,0);
	Delay_s(2);
	OLED_Clear();	
	
	AD_Init();
	
	OLED_DisplayStr(2,0, "AD Mode", EOLED_FONT_6);
	OLED_DisplayStr(4,0, "AD Value:", EOLED_FONT_6);
	
	
	while(1)
	{
		OLED_DisplayNum(4,10*6, AD_GetValue(), EOLED_FONT_6);

		Delay_ms(100);
	
		//Delay_s(10);
	};
	
}
