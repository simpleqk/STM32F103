#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "AD_DMA.h"
#include "UDMA.h"

#define DTEST_AD_DMA 0

uint8_t A[9] = {0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9};
uint8_t B[9] = {0};

void DisplayArray8b(uint8_t row, uint8_t col, const uint8_t *array, uint16_t size)
{
	uint16_t i;
	
	for (i=0; i<size; i++)
	{
		OLED_DisplayNum(row, col+i*(6+2)*2, array[i], EOLED_FONT_6);
	}	
}

void DisplayArray16b(uint8_t row, uint8_t col, const uint16_t *array, uint16_t size)
{
	uint16_t i;
	
	for (i=0; i<size; i++)
	{
		OLED_DisplayNum(row+i, col, array[i], EOLED_FONT_6);
	}	
}

void UpdateSrcData(uint8_t *array, uint16_t size)
{
	uint16_t i;
	
	for (i=0; i<size; i++)
	{
		array[i]++;
	}
}

int main()
{
#if(DTEST_AD_DMA)
	uint16_t nADValue[4]={0};
	uint8_t  nADAutoFlag=1;
#endif//DTEST_AD_DMA
	
	LED_Init();
	LED_On(ELED_1);
	
	OLED_Init();
		
	OLED_Fill(0xff);
	Delay_s(1);
	OLED_Clear();
	Delay_s(1);
	
	OLED_DisplayCN(2,0);
	Delay_s(2);
	OLED_Clear();	
	
#if(DTEST_AD_DMA)

	OLED_DisplayStr(1,0, "AD DMA Mode", EOLED_FONT_6);
	DisplayArray16b(3,0, nADValue, sizeof(nADValue)/sizeof(nADValue[0]));

	AD_DMA_Init(nADValue, sizeof(nADValue)/sizeof(nADValue[0]), nADAutoFlag);
	
#else

	OLED_DisplayStr(1,0, "DMA Mode", EOLED_FONT_6);	
	
	//test mem copy by DMA
	DisplayArray8b(3, 0, A, sizeof(A));
	DisplayArray8b(4, 0, B, sizeof(B));

	UDMA_Init((uint32_t)A, (uint32_t)B, sizeof(B));
	UDMA_TransmitData();
	
	DisplayArray8b(6, 0, A, sizeof(A));
	DisplayArray8b(7, 0, B, sizeof(B));	
	Delay_s(2);
	
	UpdateSrcData(A, sizeof(A));
	UDMA_TransmitData();
	DisplayArray8b(6, 0, A, sizeof(A));
	DisplayArray8b(7, 0, B, sizeof(B));
	Delay_s(3);
#endif//DTEST_AD_DMA

	while(1)
	{
#if(DTEST_AD_DMA)
		if (0==nADAutoFlag)
		{
			AD_DMA_GetValue();
		}
		DisplayArray16b(3,0, nADValue, sizeof(nADValue)/sizeof(nADValue[0]));
#endif//DTEST_AD_DMA
		
		Delay_s(1);
	};
	
}
