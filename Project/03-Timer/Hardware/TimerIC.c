#include "stm32f10x.h"                  // Device header

#define DTIMER_X          (TIM3)  //general timer
#define DPERIPH_TIMER_X   (RCC_APB1Periph_TIM3)  //RCC_APB2Periph_TIM1 / RCC_APB1Periph_TIM2 / RCC_APB1Periph_TIM3 / RCC_APB1Periph_TIM4
#define DGPIO_TIMER_X     (RCC_APB2Periph_GPIOA) //Timer1, RCC_APB2Periph_GPIOA, CH1 <-> PA8; CH2 <-> PA9; CH3 <-> PA10; CH4 <-> PA11;
#define DGPIO_PIN_TIMER_X (GPIO_Pin_6)           //Tuner2, RCC_APB2Periph_GPIOA, CH1 <-> PA0; CH2 <-> PA1; CH3 <-> PA2; CH4 <-> PA3;
#define DGPIO_GROUP       (GPIOA)                //Timer3, RCC_APB2Periph_GPIOA, CH1 <-> PA6; CH2 <-> PA7; CH3 <-> PB0; CH4 <-> PB1;
                                                 //Timer4, RCC_APB2Periph_GPIOB, CH1 <-> PB6; CH2 <-> PB7; CH3 <-> PB8; CH4 <-> PB9;

void Timer_IC_Init(void)
{
	//start RCC
	RCC_APB1PeriphClockCmd(DPERIPH_TIMER_X, ENABLE);
	RCC_APB2PeriphClockCmd(DGPIO_TIMER_X, ENABLE);
	
	//Initializes the GPIOx peripheral
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //controled by inner timer
	GPIO_InitStructure.GPIO_Pin = DGPIO_PIN_TIMER_X;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DGPIO_GROUP, &GPIO_InitStructure);
	
	//use internal clock, the prescaler is clocked directly by internal clock
	TIM_InternalClockConfig(DTIMER_X);
	
	//Initializes the TIMx Time Base Unit peripheral
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;  //ARR, Auto-Reload Register at the next update event;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1; //PSC, 72MHz / 72 = 1MHz
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(DTIMER_X, &TIM_TimeBaseInitStructure);
	
	
	//Initializes the TIM peripheral for input capture
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICFilter = 0xF;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInit(DTIMER_X, &TIM_ICInitStructure);

	//Initializes the TIM another channel for duty cycle calculation
	//TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	//TIM_ICInitStructure.TIM_ICFilter = 0xF;
	//TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
	//TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	//TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_IndirectTI;
	//TIM_ICInit(DTIMER_X, &TIM_ICInitStructure);
	TIM_PWMIConfig(DTIMER_X, &TIM_ICInitStructure);
	
	//Selects the Input Trigger source
	TIM_SelectInputTrigger(DTIMER_X, TIM_TS_TI1FP1);
	
	//Selects the TIMx Slave Mode
	TIM_SelectSlaveMode(DTIMER_X, TIM_SlaveMode_Reset);
	
	//start timer
	TIM_Cmd(DTIMER_X, ENABLE);
}

uint32_t Timer_IC_GetFreq(void)
{
	//Freq = 1 / (1/PSC) * N
	//Freq = PSC / N
	
	return 100000 / TIM_GetCapture1(DTIMER_X) + 1;
}

uint32_t Timer_IC_GetDuty(void)
{
	//duty cycle = CCR2 / CCR1
	
	return (100 * TIM_GetCapture2(DTIMER_X) / TIM_GetCapture1(DTIMER_X)) + 1;
}
