#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LED.h"
#include "Key.h"
#include "OLED.h"
#include "IOIT.h"
#include "Timer.h"
#include "TimerPWM.h"
#include "TimerIC.h"
#include "TimerEncoder.h"

enum MODE {
	EMODE_TIMER_OI = 0, //Timer trigger innterupt to turn on/off LED
	EMODE_TIMER_OC = 1, //Timer output PWM pulse by output compare mode
	EMODE_TIMER_IC = 2, //Timer calc pulse period by input capture mode
	EMODE_TIMER_IE = 3, //Timer count is  driven by two input pulse
};

void TimerCallback(uint32_t arg)
{	
	static uint8_t flag=0;
	
	if (EMODE_TIMER_OI == arg)
	{	
		if (0 == (flag++ % 2))
		{
			LED_On(ELED_0);
		}
		else
		{
			LED_Off(ELED_0);
		}
	}
	else if (EMODE_TIMER_IE == arg)
	{
		//simulate period pulse on pin6 and pin7 for timer4
		
		flag++;
		if(flag > 4)
		{
			flag = 1;
		}
		
		if (0 == (flag % 2))
		{
			LED_On(ELED_0);
		}
		else
		{
			LED_Off(ELED_0);
		}
		
		
		if (1 == flag)
		{
			//PB6
			GPIO_ResetBits(GPIOB, GPIO_Pin_8); //PB8 <-> PB6
			//PB7
			GPIO_ResetBits(GPIOB, GPIO_Pin_9); //PB9  <-> PB7
		}
		else if (2 == flag)
		{
			//PB6
			GPIO_SetBits(GPIOB, GPIO_Pin_8); //PB8 <-> PB6
			//PB7
			GPIO_ResetBits(GPIOB, GPIO_Pin_9); //PB9  <-> PB7		
		}
		else if (3 == flag)
		{
			//PB6
			GPIO_SetBits(GPIOB, GPIO_Pin_8); //PB8 <-> PB6
			//PB7
			GPIO_SetBits(GPIOB, GPIO_Pin_9); //PB9  <-> PB7		
		}
		else if (4==flag)
		{
			//PB6
			GPIO_ResetBits(GPIOB, GPIO_Pin_8); //PB8 <-> PB6
			//PB7
			GPIO_SetBits(GPIOB, GPIO_Pin_9); //PB9  <-> PB7		
		}
	}
}

int main()
{
	enum MODE mode = EMODE_TIMER_IE;
	//uint8_t key = 0;
	uint16_t i;
	
	LED_Init();
	LED_On(ELED_1);

	//Key_Init();
	
	OLED_Init();
		
	OLED_Fill(0xff);
	Delay_s(1);
	OLED_Clear();
	Delay_s(1);
	
	OLED_DisplayCN(2,0);
	Delay_s(2);
	OLED_Clear();	
	
	switch(mode)
	{
		case EMODE_TIMER_OI:
			OLED_DisplayStr(2,0, "Timer Count Mode", EOLED_FONT_6);
			OLED_DisplayStr(4,0, "count:", EOLED_FONT_6);
			Timer_Init(TimerCallback, mode);
			break;
		case EMODE_TIMER_OC:
			OLED_DisplayStr(2,0, "Timer OC Mode", EOLED_FONT_6);
			OLED_DisplayStr(4,0, "count:", EOLED_FONT_6);
			IOIT_Init(); //PA8(pwm out) connect to PA1(in)
			Timer_PWM_Init();
			break;
		case EMODE_TIMER_IC:
			OLED_DisplayStr(2,0, "Timer IC Mode", EOLED_FONT_6);
			OLED_DisplayStr(4,0, "freq:      Hz", EOLED_FONT_6);
			OLED_DisplayStr(6,0, "duty:        ", EOLED_FONT_6);
			Timer_PWM_Init(); //PA8(pwm out) connect to PA6(IC)
			Timer_PWM_SetPrescaler(7200 - 1);
		    Timer_PWM_Update_DutyCycle(80);
			Timer_IC_Init();
			break;
		case EMODE_TIMER_IE:
			OLED_DisplayStr(2,0, "Timer Encocer Mode", EOLED_FONT_6);
			OLED_DisplayStr(4,0, "count:", EOLED_FONT_6);
			Timer_Encoder_Init();
			
			GPIO_InitTypeDef GPIO_InitStructure;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9; //PB8 <-> PB6, PB9  <-> PB7
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOB, &GPIO_InitStructure);
		
			Timer_Init(TimerCallback, mode);
			Timer_SetPrescaler(720 - 1);
			break;
		default:
			OLED_DisplayStr(3,0, "Unknown Mode", EOLED_FONT_6);
			break;
	}
	
	
	while(1)
	{
		switch(mode)
		{
			case EMODE_TIMER_OI:
				OLED_DisplayNum(4, 6*6, Timer_GetCount(), EOLED_FONT_6);
				break;
			case EMODE_TIMER_OC:
				OLED_DisplayNum(4, 6*6, IOIT_GetCount(), EOLED_FONT_6);
				for (i=0; i<=1000; i++)
				{
					Timer_PWM_Update_DutyCycle(i);
					Delay_us(50);
				}
				break;
			case EMODE_TIMER_IC:
				OLED_DisplayNum(4, 5*6, Timer_IC_GetFreq(), EOLED_FONT_6);
				OLED_DisplayNum(6, 5*6, Timer_IC_GetDuty(), EOLED_FONT_6);
				break;
			case EMODE_TIMER_IE:
				OLED_DisplayStr(4, 6*6, "                ", EOLED_FONT_6);
				OLED_DisplayNum(4, 6*6, Timer_Encoder_GetCount(0), EOLED_FONT_6);				
				break;
			default:
				break;
		}

		Delay_ms(100);
	
		//Delay_s(10);
	};
	
}
