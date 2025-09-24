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

static uint16_t g_adValueSize;

void AD_DMA_Init(uint16_t *adValue, uint16_t size, uint8_t autoflag)
{
	g_adValueSize = size;
	
	//Enables or disables the High Speed APB (APB2) peripheral clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//Enables or disables the AHB peripheral clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	//Configures the ADC clock (ADCCLK)
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); // 72MHz(APB2-clock) / 6 = 12MHz <= 14MHz
	
	//Initializes the GPIOx peripheral
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3
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
	ADC_InitStructure.ADC_ContinuousConvMode = (0==autoflag) ? DISABLE : ENABLE; //ENABLE, continuos scan mode; DISABLE single scan mode
	ADC_InitStructure.ADC_ScanConvMode = ENABLE; //ENABLE, Scan (multichannels) mode; DISABLE Single (one channel) mode
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	
		
	//Initializes the DMAy Channelx
	DMA_InitTypeDef DMA_InitStructure;
	//peripheral
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //DMA_PeripheralDataSize_Byte / DMA_PeripheralDataSize_HalfWord / DMA_PeripheralDataSize_Word
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //disable, address not auto increasement
	//ram
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)adValue;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //DMA_MemoryDataSize_Byte / DMA_MemoryDataSize_HalfWord / DMA_MemoryDataSize_Word
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //enable, address auto increasement
	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //DMA_DIR_PeripheralDST / DMA_DIR_PeripheralSRC
	DMA_InitStructure.DMA_BufferSize = size;
	DMA_InitStructure.DMA_Mode = (0==autoflag) ? DMA_Mode_Normal : DMA_Mode_Circular; //DMA_Mode_Circular  / DMA_Mode_Normal
	DMA_InitStructure.DMA_M2M = DMA_M2M_Enable; //DMA_M2M_Enable (software trigger) / DMA_M2M_Disable (hardware trigger)
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure); //ADC1 <-> Channel1


	//Enables or disables the specified DMAy Channelx
	DMA_Cmd(DMA1_Channel1, ENABLE);	
	//Enables or disables the specified ADC DMA request
	ADC_DMACmd(ADC1, ENABLE);
	//Enables or disables the specified ADC peripheral
	ADC_Cmd(ADC1, ENABLE);
	
	//Starts the selected ADC calibration process
	ADC_ResetCalibration(ADC1);
	while(RESET != ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(RESET != ADC_GetCalibrationStatus(ADC1));
	
	if (0 != autoflag)
	{
		//start ADC for single scan mode
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	}
}

void AD_DMA_GetValue(void)
{
	DMA_Cmd(DMA1_Channel1, DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel1, g_adValueSize);
	
	//restart DMA transmit data
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	//start ADC for single scan mode
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	
	//wait AD convert and DMA transmit
	while(SET != DMA_GetFlagStatus(DMA1_FLAG_TC1));	
	DMA_ClearFlag(DMA1_FLAG_TC1);
}
