#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "UDMA.h"
#include "Serial.h"

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


int main()
{
	uint8_t i = 0;
	uint8_t sbyte = 'A';//0x03;
	//uint8_t rbyte = 0x00;
	uint8_t sarray[] = "Welcome STM Serial!";
	uint8_t rarray[10] = {0};
	uint8_t rlen = 0;
	
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

	OLED_DisplayStr(1,0, "USART Mode", EOLED_FONT_6);	

	
	Serial_Init(9600, 1);
	
#if (DUSE_MICROLIB)	
	printf("printf function test, %d", sbyte);
#endif	
	Delay_s(1);
		
	while(1)
	{
		//send
		OLED_DisplayStr(3, 0,   "Send: ", EOLED_FONT_6);
		OLED_DisplayStr(3, 6*6, "                    ", EOLED_FONT_6);
		Delay_s(1);
		
		Serial_SendByte(sbyte);
		OLED_DisplayNum(3, 6*6, sbyte, EOLED_FONT_6);		
		Delay_s(1);
		
		Serial_SendArray(sarray, sizeof(sarray)-1); //ignore string tail '\0'
		OLED_DisplayStr(3, 6*6, (const char*)sarray, EOLED_FONT_6);	
		Delay_s(1);
		
#if (DUSE_MICROLIB)		
		printf("你好STM!");
		Delay_s(1);
#endif		
		
		//receive	
//		if(0 == Serial_ReceiveByte(&rbyte))
//		{
//			OLED_DisplayStr(3, 0,   "Receive: ", EOLED_FONT_6);
//			OLED_DisplayStr(3, 9*6, "                    ", EOLED_FONT_6);
//			
//			OLED_DisplayNum(3, 9*6, rbyte, EOLED_FONT_6);
//			
//			Delay_s(1);
//		}
		
		rlen = sizeof(rarray);
		if(0 == Serial_ReceiveArray(rarray, &rlen))
		{
			OLED_DisplayStr(3, 0,    "ReceiveA: ", EOLED_FONT_6);
			OLED_DisplayStr(3, 10*6, "                    ", EOLED_FONT_6);
			
#if (DUSE_MICROLIB)			
			printf("STM receive array data (len=%d): ", rlen);
#endif			
			for (i=0; i<rlen; i++)
			{
#if (DUSE_MICROLIB)				
				printf("%c",  rarray[i]);
#endif				
				OLED_DisplayChar(3, (10+1+i)*6, rarray[i], EOLED_FONT_6);
			}
#if (DUSE_MICROLIB)			
			printf("\n");
#endif			
			Delay_s(1);		
		}
	};
	
}
