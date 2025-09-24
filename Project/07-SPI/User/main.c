#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "W25Q16.h"

void Display_Array(uint8_t row, uint8_t col, uint8_t *array, uint8_t size)
{
	uint8_t i;
		
	for (i=0; i<size; i++)
	{
		OLED_DisplayNum(row, col+i*(2*6+2), array[i], EOLED_FONT_6);
	}
}

void Display_W25Q16_IDs()
{
	uint8_t w25q16_pro[3]={0};
	
	OLED_Clear();
	OLED_DisplayStr(2,0,"SPI RW W25Q16", EOLED_FONT_6);

	W25Q16_ReadProperty(&w25q16_pro[0], &w25q16_pro[1], &w25q16_pro[2]);
	
	OLED_DisplayStr(4,0, "Manufacture ID: ", EOLED_FONT_6);
	OLED_DisplayNum(4,15*6, w25q16_pro[0], EOLED_FONT_6);
	
	OLED_DisplayStr(5,0, "Memory Type ID: ", EOLED_FONT_6);
	OLED_DisplayNum(5,15*6, w25q16_pro[1], EOLED_FONT_6);
	
	OLED_DisplayStr(6,0, "Capacity ID: ", EOLED_FONT_6);
	OLED_DisplayNum(6,12*6, w25q16_pro[2], EOLED_FONT_6);	
}

void Display_W25Q16_RW()
{
	uint8_t i,t;
	uint8_t w25q16_rd[7]={0};
	static uint8_t w25q16_wd[7]={0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7};
	uint32_t w25q16_sector_addr = 0x0000;
	uint32_t w25q16_page_addr = 0x0000;
	
	OLED_Clear();
	OLED_DisplayStr(2,0, "SPI RW W25Q16", EOLED_FONT_6);
	
	//read old data
	OLED_DisplayStr(4,0, "R0: ", EOLED_FONT_6);
	W25Q16_Read_Data(w25q16_page_addr, w25q16_rd, sizeof(w25q16_rd));
	Display_Array(4, 3*6, w25q16_rd, sizeof(w25q16_rd));
	
	//update data being to write
	t = 0;
	for (i=0; i<sizeof(w25q16_rd); i++)
	{
		t += w25q16_rd[i];
	}
	if (0 != t)
	{
		for (i=0; i<sizeof(w25q16_rd); i++)
		{
			w25q16_wd[i] = w25q16_rd[i];
		}
		
		t = w25q16_wd[0];
		for (i=1; i<sizeof(w25q16_wd); i++)
		{
			w25q16_wd[i-1] = w25q16_wd[i];
		}
		w25q16_wd[i-1] = t;
	}
	
	//write new data
	OLED_DisplayStr(5,0, "W: ", EOLED_FONT_6);
	W25Q16_Sector_Erase(w25q16_sector_addr);
	W25Q16_Page_Program(w25q16_page_addr, w25q16_wd, sizeof(w25q16_wd));
	Display_Array(5, 3*6, w25q16_wd, sizeof(w25q16_wd));
	
	//read old data
	OLED_DisplayStr(6,0, "R1: ", EOLED_FONT_6);
	W25Q16_Read_Data(w25q16_page_addr, w25q16_rd, sizeof(w25q16_rd));
	Display_Array(6, 3*6, w25q16_rd, sizeof(w25q16_rd));
}

int main()
{
	LED_Init();
	LED_On(ELED_1);
	
	W25Q16_Init();
	
	OLED_Init();
	OLED_Fill(0xff);
	Delay_s(1);
	OLED_Clear();
	Delay_s(1);
			
	
	while(1)
	{
		Display_W25Q16_IDs();
		
		Delay_s(1);
		
		Display_W25Q16_RW();
		
		Delay_s(3);
	};
	
}
