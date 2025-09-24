#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LED.h"
#include "Key.h"
#include "OLED.h"

int main()
{
	uint8_t key = 0;
	
	LED_Init();
	LED_On(ELED_1);

	Key_Init();
	
	OLED_Init();
	
	while(1)
	{
		OLED_Fill(0xff);
		Delay_s(2);
		OLED_Clear();
		Delay_s(2);
		
		OLED_DisplayBmp();
		Delay_s(4);
		OLED_Clear();
		
		OLED_DisplayCN(2,0);
		Delay_s(4);
		OLED_Clear();
		
		OLED_DisplayChar(1,54,'A', EOLED_FONT_6);
		Delay_s(1);
		OLED_DisplayChar(1,60,'B', EOLED_FONT_6);
		Delay_s(1);
		OLED_DisplayChar(1,66,'C', EOLED_FONT_6);
		Delay_s(1);
		
		OLED_DisplayStr(3,4,"Welcome OLED!", EOLED_FONT_8);
		Delay_s(1);
		
		OLED_DisplayNum(7, 1,   0, EOLED_FONT_6);
		OLED_DisplayNum(7, 20,  9876, EOLED_FONT_6);
		OLED_DisplayNum(7, 60, -1234, EOLED_FONT_6);
		Delay_s(1);
		
		key = Key_GetState();
		if(key > 0)
		{
			LED_On(ELED_0);
			OLED_DisplayStr(7, 110, "ON", EOLED_FONT_6);
		}
		else
		{
			LED_Off(ELED_0);
		}
		
		Delay_s(10);
	};
	
}
