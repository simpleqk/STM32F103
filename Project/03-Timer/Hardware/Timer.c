#include "stm32f10x.h"                  // Device header
#include "Timer.h"

#define DTIMER_X         (TIM6) //basic timer x
#define DPERIPH_TIMER_X  (RCC_APB1Periph_TIM6)
#define DTIMER_IRQ_X     (TIM6_IRQn)

static PFTIMER  g_pfTimerCallback;
static uint32_t g_argpf;
static uint16_t g_count;

void Timer_Init(PFTIMER pf, uint32_t argpf)
{
	g_pfTimerCallback = pf;
	g_argpf = argpf;
	
	//RCC
	RCC_APB1PeriphClockCmd(DPERIPH_TIMER_X, ENABLE);
	
	//use internal clock, the prescaler is clocked directly by internal clock
	TIM_InternalClockConfig(DTIMER_X);
	
	//Initializes the TIMx Time Base Unit peripheral
	TIM_TimeBaseInitTypeDef TIME_TimeBaseInitStructure;
	TIME_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIME_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIME_TimeBaseInitStructure.TIM_Period = 10000 - 1; //Auto-Reload Register at the next update event; 1s period
	TIME_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1; //time = ( 1 / 72MHz / (7200-1 + 1) ) * count;
	TIME_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(DTIMER_X, &TIME_TimeBaseInitStructure);
	
	
	//need clear the flat that update event to reload the Prescaler and the Repetition counter values immediately in function TIM_TimeBaseInit
	TIM_ClearFlag(DTIMER_X, TIM_FLAG_Update);
	
	//Enables or disables the specified TIM interrupts
	TIM_ITConfig(DTIMER_X, TIM_IT_Update, ENABLE);


	//Configures the priority grouping
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	//Initializes the NVIC peripheral
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = DTIMER_IRQ_X;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	//start timer
	TIM_Cmd(DTIMER_X, ENABLE);
}

//system interupt function
void TIM6_IRQHandler(void) //TIM2_IRQHandler / TIM3_IRQHandler
{
	if (SET == TIM_GetITStatus(DTIMER_X, TIM_IT_Update))
	{
		if (g_pfTimerCallback)
		{
			g_pfTimerCallback(g_argpf);
		}
		
		g_count++;
		
		TIM_ClearITPendingBit(DTIMER_X, TIM_IT_Update);
	}
}

void Timer_SetPrescaler(uint16_t value)
{
	TIM_PrescalerConfig(DTIMER_X, value, TIM_PSCReloadMode_Update);
}

uint16_t Timer_GetCount(void)
{
	//return TIM_GetCounter(DTIMER_X);
	return g_count;
}
