#ifndef __OLED_H_
#define __OLED_H_

enum OLED_FONT_SIZE
{
	EOLED_FONT_6 = 0,
	EOLED_FONT_8 = 1
};

void OLED_Init(void);

void OLED_Fill(unsigned char data);

void OLED_Clear(void);

void OLED_DisplayChar(uint8_t row, uint8_t col, char c, enum OLED_FONT_SIZE size);

void OLED_DisplayStr(uint8_t row, uint8_t col, const char *text, enum OLED_FONT_SIZE size);

void OLED_DisplayNum(uint8_t row, uint8_t col, int value, enum OLED_FONT_SIZE size);

void OLED_DisplayBmp(void);

void OLED_DisplayCN(uint8_t row, uint8_t col);

#endif
