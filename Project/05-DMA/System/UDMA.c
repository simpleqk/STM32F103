#include "stm32f10x.h"                  // Device header

static uint16_t g_size;

void UDMA_Init(uint32_t srcAddr, uint32_t dstAddr, uint16_t size)
{
	g_size = size;
	
	//Enables or disables the AHB peripheral clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	//Initializes the DMAy Channelx
	DMA_InitTypeDef DMA_InitStructure;
	//peripheral
	DMA_InitStructure.DMA_PeripheralBaseAddr = srcAddr;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte; //DMA_MemoryDataSize_HalfWord / DMA_MemoryDataSize_Word
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable; //DMA_PeripheralInc_Enable, address auto inc; DMA_PeripheralInc_Disable, address not auto inc
	//ram
	DMA_InitStructure.DMA_MemoryBaseAddr = dstAddr;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //address auto increasement
	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //DMA_DIR_PeripheralDST / DMA_DIR_PeripheralSRC
	DMA_InitStructure.DMA_BufferSize = size;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; //DMA_Mode_Circular  / DMA_Mode_Normal
	DMA_InitStructure.DMA_M2M = DMA_M2M_Enable; //DMA_M2M_Enable (software trigger) / DMA_M2M_Disable (hardware trigger)
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	//start DMA transmit data
	//DMA_Cmd(DMA1_Channel1, ENABLE);
	
	//while(SET != DMA_GetFlagStatus(DMA1_FLAG_TC1));
}

void UDMA_TransmitData(void)
{
	DMA_Cmd(DMA1_Channel1, DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel1, g_size);
	
	//restart DMA transmit data
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	while(SET != DMA_GetFlagStatus(DMA1_FLAG_TC1));	
	DMA_ClearFlag(DMA1_FLAG_TC1);
}
