#include "stm32f10x.h"                  // Device header

//ADC1/2_IN0  -> PA0
//ADC1/2_IN1  -> PA1
//ADC1/2_IN2  -> PA2
//ADC1/2_IN3  -> PA3
//ADC1/2_IN4  -> PA4
//ADC1/2_IN5  -> PA5
//ADC1/2_IN6  -> PA6
//ADC1/2_IN7  -> PA7

//ADC1/2_IN8  -> PB0
//ADC1/2_IN9  -> PB1

//ADC1/2_IN10 -> PC0
//ADC1/2_IN11 -> PC1
//ADC1/2_IN12 -> PC2
//ADC1/2_IN13 -> PC3
//ADC1/2_IN14 -> PC4
//ADC1/2_IN15 -> PC5


void AD_Init(void)
{
	//Enables or disables the High Speed APB (APB2) peripheral clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	//Configures the ADC clock (ADCCLK)
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); // 72MHz(APB2-clock) / 6 = 12MHz <= 14MHz
	
	//Initializes the GPIOx peripheral
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//Configures for the selected ADC regular channel its corresponding rank in the sequencer and its sample time
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5); //cost 1/12MHz * (55.5 + 12.5) = 5.66us
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_55Cycles5);
	
	//Initializes the ADCx peripheral
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //ADC_DataAlign_Left
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //software trigger
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //ENABLE, continuos scan mode; DISABLE single scan mode
	ADC_InitStructure.ADC_ScanConvMode = DISABLE; //ENABLE, Scan (multichannels) mode; DISABLE Single (one channel) mode
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	//Enables or disables the specified ADC peripheral
	ADC_Cmd(ADC1, ENABLE);
	
	//Starts the selected ADC calibration process
	ADC_ResetCalibration(ADC1);
	while(RESET != ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(RESET != ADC_GetCalibrationStatus(ADC1));
}

uint16_t AD_GetValue(void/*ADC_Channel_x*/)
{
	//specify the selected channel to get conversion result
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_x, 1, ADC_SampleTime_55Cycles5);
	
	//start ADC for single scan mode
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	
	//wait convert
	while(SET != ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
	
	//Returns the last ADCx conversion result data for regular channel
	return ADC_GetConversionValue(ADC1);
	
	
}
