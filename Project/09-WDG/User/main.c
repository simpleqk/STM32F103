#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"

#define DUSE_IWDG (0)

int main()
{
	const char *title = 0;
	const char *content = 0;
	uint8_t rcc_flag;
	
	LED_Init();
	LED_On(ELED_1);
		
	OLED_Init();
	OLED_Fill(0xff);
	Delay_s(1);
	OLED_Clear();
	Delay_s(1);
	
#if (DUSE_IWDG)
	title = "IWDG Mode";
	content = "IWDGRST";
	rcc_flag = RCC_FLAG_IWDGRST;
#else
	title = "WWDG Mode";
	content = "WWDGRST";
	rcc_flag = RCC_FLAG_WWDGRST;
#endif//	
	OLED_DisplayStr(2,0, title, EOLED_FONT_6);
	
	if (SET == RCC_GetFlagStatus(rcc_flag))
	{
		OLED_DisplayStr(4,0, content, EOLED_FONT_6);
		Delay_s(1);
		OLED_DisplayStr(4,0, "       ", EOLED_FONT_6);
	}
	else
	{
		OLED_DisplayStr(4,0, "RST", EOLED_FONT_6);
		Delay_s(1);
		OLED_DisplayStr(4,0, "   ", EOLED_FONT_6);
	}
	
#if (DUSE_IWDG)	
	//Enables or disables write access to IWDG_PR and IWDG_RLR registers
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	//time = 1/40K * Prescaler * (RLR + 1);
	//1000ms = 0.025 * 16 * (RLR + 1) => RLR = 2499; //RLR: 0~0x0FFF
	IWDG_SetPrescaler(IWDG_Prescaler_16); //IWDG_Prescaler_4 / IWDG_Prescaler_8 / IWDG_Prescaler_16 /.../ IWDG_Prescaler_256
	IWDG_SetReload(2499);
	IWDG_ReloadCounter();
	IWDG_Enable();
	
#else
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
	
	//time_out = 1/36M * 4096 * Prescaler * (T[5:0] + 1);
	//50ms = 1/36000 * 4096 * 8 * (T[5:0] + 1) => T[5:0] = 54
	//time_window = 1/36M * 4096 * Prescaler * (T[5:0] - W[6:0]);
	//30ms = 1/36000 * 4096 * 8 * (54 - W[6:0]) => 21
	WWDG_SetPrescaler(WWDG_Prescaler_8); //WWDG counter clock = (PCLK1/4096)/2; WWDG_Prescaler_1 / WWDG_Prescaler_2 / WWDG_Prescaler_4 / WWDG_Prescaler_8
	
	WWDG_SetWindowValue(21 | 0x40); //30ms. Sets the WWDG->CFR counter value
	WWDG_Enable(54 | 0x40); //50ms. Sets the WWDG->CR counter value
	
#endif//DUSE_IWDG
	
	while(1)
	{
		OLED_DisplayStr(4,0, "Feed", EOLED_FONT_6);
#if (DUSE_IWDG)		
		Delay_ms(200);
#else	
		Delay_ms(20);
#endif//DUSE_IWDG	
		
		OLED_DisplayStr(4,0, "    ", EOLED_FONT_6);
#if (DUSE_IWDG)		
		Delay_ms(600);
#else	
		Delay_ms(20);
#endif//DUSE_IWDG	

#if (DUSE_IWDG)		
		IWDG_ReloadCounter();
#else	
		WWDG_SetCounter(54 | 0x40); //Sets the WWDG->CR counter value
#endif//DUSE_IWDG

	};	
}
