#include "stm32f10x.h"                  // Device header

#define DTIMER_X          (TIM1)  //advance timer / general timer
#define DPERIPH_TIMER_X   (RCC_APB2Periph_TIM1)  //RCC_APB2Periph_TIM1 / RCC_APB1Periph_TIM2 / RCC_APB1Periph_TIM3 / RCC_APB1Periph_TIM4
#define DGPIO_TIMER_X     (RCC_APB2Periph_GPIOA) //Timer1, RCC_APB2Periph_GPIOA, CH1 <-> PA8; CH2 <-> PA9; CH3 <-> PA10; CH4 <-> PA11;
#define DGPIO_PIN_TIMER_X (GPIO_Pin_8)           //Tuner2, RCC_APB2Periph_GPIOA, CH1 <-> PA0; CH2 <-> PA1; CH3 <-> PA2; CH4 <-> PA3;
#define DGPIO_GROUP       (GPIOA)                //Timer3, RCC_APB2Periph_GPIOA, CH1 <-> PA6; CH2 <-> PA7; CH3 <-> PB0; CH4 <-> PB1;
                                                 //Timer4, RCC_APB2Periph_GPIOB, CH1 <-> PB6; CH2 <-> PB7; CH3 <-> PB8; CH4 <-> PB9;

void Timer_PWM_Init(void)
{
	//start RCC
	RCC_APB2PeriphClockCmd(DPERIPH_TIMER_X, ENABLE);
	RCC_APB2PeriphClockCmd(DGPIO_TIMER_X, ENABLE);
	
	//Initializes the GPIOx peripheral
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //controled by inner timer
	GPIO_InitStructure.GPIO_Pin = DGPIO_PIN_TIMER_X; //Timer noremap channel pin
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DGPIO_GROUP, &GPIO_InitStructure);
	
	//use internal clock, the prescaler is clocked directly by internal clock
	TIM_InternalClockConfig(DTIMER_X);
	
	//Initializes the TIMx Time Base Unit peripheral
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 100; //10000 - 1; //ARR, Auto-Reload Register at the next update event; default 1s
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1; //PSC, 72MHz / 7200 = 10KHz
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(DTIMER_X, &TIM_TimeBaseInitStructure);
	
	
	//Initializes the timer output channelx for PWM
	//timer generate PWM pulse and output by IO pin (PB8/PB9)
	//PWM_Freq = CLK_PSC / (PSC + 1) / (ARR + 1)
	//PWM_Freq = 72MHz / 7200 / 10000 = 1Hz = 1s
	//PWM_Resolution = 1 / (ARR + 1) = 1 / 10000
	//PWM_DutyCircle = CCR / (ARR + 1)
	//PWM_DutyCircle = 2000 / 10000 = 20%
	TIM_OCInitTypeDef TIM_OCInitTypeStrtucture;
	TIM_OCStructInit(&TIM_OCInitTypeStrtucture);
	TIM_OCInitTypeStrtucture.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitTypeStrtucture.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitTypeStrtucture.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitTypeStrtucture.TIM_Pulse = 50; //5000; //CCR, will be loaded into the Capture Compare Register
	TIM_OC1Init(DTIMER_X, &TIM_OCInitTypeStrtucture); //CH1
	//TIM_OC3Init(DTIMER_X, &TIM_OCInitTypeStrtucture); //CH3
	
	//Enables or disables the TIM peripheral Main Outputs
	TIM_CtrlPWMOutputs(DTIMER_X, ENABLE);
	
	//start timer
	TIM_Cmd(DTIMER_X, ENABLE);
}

void Timer_PWM_Update_DutyCycle(uint16_t value)
{
	TIM_SetCompare1(DTIMER_X, value); //update CH1 CCR that change duty cycle
	//TIM_SetCompare3(DTIMER_X, value); //update CH3 CCR that change duty cycle
}

void Timer_PWM_SetPrescaler(uint16_t value)
{
	TIM_PrescalerConfig(DTIMER_X, value, TIM_PSCReloadMode_Immediate);
}
