#include "stm32f10x.h"                  // Device header

#define DTIMER_X          (TIM4)  //general timer
#define DPERIPH_TIMER_X   (RCC_APB1Periph_TIM4)  //RCC_APB2Periph_TIM1 / RCC_APB1Periph_TIM2 / RCC_APB1Periph_TIM3 / RCC_APB1Periph_TIM4
#define DGPIO_TIMER_X     (RCC_APB2Periph_GPIOB) //Timer1, RCC_APB2Periph_GPIOA, CH1 <-> PA8; CH2 <-> PA9; CH3 <-> PA10; CH4 <-> PA11;
#define DGPIO_PIN_TIMER_X (GPIO_Pin_6)           //Tuner2, RCC_APB2Periph_GPIOA, CH1 <-> PA0; CH2 <-> PA1; CH3 <-> PA2; CH4 <-> PA3;
#define DGPIO_GROUP       (GPIOB)                //Timer3, RCC_APB2Periph_GPIOA, CH1 <-> PA6; CH2 <-> PA7; CH3 <-> PB0; CH4 <-> PB1;
                                                 //Timer4, RCC_APB2Periph_GPIOB, CH1 <-> PB6; CH2 <-> PB7; CH3 <-> PB8; CH4 <-> PB9;

void Timer_Encoder_Init(void)
{
	//start RCC
	RCC_APB1PeriphClockCmd(DPERIPH_TIMER_X, ENABLE);
	RCC_APB2PeriphClockCmd(DGPIO_TIMER_X, ENABLE);

	//Initializes the GPIOx peripheral
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //GPIO_Mode_IN_FLOATING; //GPIO_Mode_IPU; //GPIO_Mode_IPD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //use timer4 ch1 and ch2 pin (PB6 and PB7) for count input pulse
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DGPIO_GROUP, &GPIO_InitStructure);
	
	//Initializes the TIMx Time Base Unit peripheral
	TIM_TimeBaseInitTypeDef TIME_TimeBaseInitStructure;
	TIME_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIME_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIME_TimeBaseInitStructure.TIM_Period = 65536 - 1; //Auto-Reload Register at the next update event;
	TIME_TimeBaseInitStructure.TIM_Prescaler = 1 - 1; //time = ( 1 / 72MHz / (7200-1 + 1) ) * count;
	TIME_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(DTIMER_X, &TIME_TimeBaseInitStructure);

	
	//Initializes the TIM peripheral for input capture
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICFilter = 0xF;
	//TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; //level normal, not invert.
	//TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInit(DTIMER_X, &TIM_ICInitStructure);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICFilter = 0xF;
	//TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; //level normal, not invert.
	//TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInit(DTIMER_X, &TIM_ICInitStructure);


	//Configures the TIMx Encoder Interface
	TIM_EncoderInterfaceConfig(DTIMER_X, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising); //TIM_ICPolarity_Falling
	
	//start timer
	TIM_Cmd(DTIMER_X, ENABLE);
}

uint16_t Timer_Encoder_GetCount(uint8_t bClear)
{
	uint16_t count = TIM_GetCounter(DTIMER_X);
	
	if (bClear)
	{
		TIM_SetCounter(DTIMER_X, 0);
	}
	
	return count;
}
