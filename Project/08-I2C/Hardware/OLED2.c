#include "stm32f10x.h"                  // Device header
#include "fonts.h"
#include "UI2C.h"
#include "OLED2.h"

#define DUSE_I2C_HARDWARE

//dot font size
typedef struct tagFontSize
{
	uint8_t width;
	uint8_t height;

}FontSize;

//stm32F103RC MCU
//I2C interface
//PB10 connect SCL
//PB11 connect SDA
#define OLED_I2C             I2C2
#define OLED_I2C_SCL         GPIO_Pin_10
#define OLED_I2C_SDA         GPIO_Pin_11
#define OLED_I2C_W_ADDR      0x78 //slave adress:0 1 1 1 1 0 SA0 R/W; SA0=0/1; R=1, W=0;
#define OLED_I2C_R_ADDR      0x79 //slave adress:0 1 1 1 1 0 SA0 R/W; SA0=0/1; R=1, W=0;
#define OLED_CMD_MODE        0x00 //control byte: Co bit=0, D/C# bit=0 (following data byte as a command), 000000 bits
#define OLED_DATA_MODE       0x40 //control byte: Co bit=0, D/C# bit=1 (following data byte as a data), 000000 bits

#ifdef DUSE_I2C_HARDWARE //use stm32 pin I2C hardware function

static void OLED_Configure()
{
	UI2C_Init();
}

static void OLED_I2C_WriteByte(uint8_t address, uint8_t byte)
{
	UI2C_Write(OLED_I2C_W_ADDR, address, &byte, 1);
}

static void OLED_WriteCmd(uint8_t cmd)
{
	OLED_I2C_WriteByte(OLED_CMD_MODE, cmd);
}

static void OLED_WriteData(uint8_t data)
{
	OLED_I2C_WriteByte(OLED_DATA_MODE, data);
}

#else //use normal pin io function used to simulate I2C communication

#define OLED_I2C_W_SCL(x)  GPIO_WriteBit(GPIOB, OLED_I2C_SCL, (BitAction)x)
#define OLED_I2C_W_SDA(x)  GPIO_WriteBit(GPIOB, OLED_I2C_SDA, (BitAction)x)

static void OLED_Configure()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD; //note must be open-drain mode;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = OLED_I2C_SCL | OLED_I2C_SDA;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}

static void OLED_I2C_Start()
{
	OLED_I2C_W_SDA(1);
	OLED_I2C_W_SCL(1); //SDA level high -> low
	OLED_I2C_W_SDA(0);
	OLED_I2C_W_SCL(0);
}

static void OLED_I2C_Stop()
{
	OLED_I2C_W_SDA(0);
	OLED_I2C_W_SCL(1); //SDA level low -> high
	OLED_I2C_W_SDA(1);
}

static void OLED_I2C_WriteByte(uint8_t byte)
{
	uint8_t i;
	uint8_t b;
	
	OLED_I2C_W_SCL(0);
	
	for (i=0; i<8; i++)
	{
		b = (0 == (byte & (0x80>>i))) ? 0 : 1;
		OLED_I2C_W_SDA(b); //change data when SCL level is low
		OLED_I2C_W_SCL(1);
		OLED_I2C_W_SCL(0);
	}
	
	OLED_I2C_W_SDA(1);
	OLED_I2C_W_SCL(1); //for ack?
	OLED_I2C_W_SCL(0);
}

void OLED_WriteCmd(uint8_t cmd)
{
	OLED_I2C_Start();
	OLED_I2C_WriteByte(OLED_I2C_W_ADDR); //slave addr (oled)
	OLED_I2C_WriteByte(OLED_CMD_MODE); //control byte
	OLED_I2C_WriteByte(cmd); //data
	OLED_I2C_Stop();
}

void OLED_WriteData(uint8_t data)
{
	OLED_I2C_Start();
	OLED_I2C_WriteByte(OLED_I2C_W_ADDR);
	OLED_I2C_WriteByte(OLED_DATA_MODE);
	OLED_I2C_WriteByte(data);
	OLED_I2C_Stop();
}
#endif//DUSE_I2C_HARDWARE

//-----------------------------------------------------------------------------

void OLED_Init(void)
{
	OLED_Configure();
	
	//refer the command table in datasheet document	
	//fundamental function
	OLED_WriteCmd(0xAE);    // close display
	
    OLED_WriteCmd(0x81);    // set contrast control
    OLED_WriteCmd(0xCF);//7F
	
	OLED_WriteCmd(0xA4);    //entire display on. A4,output follows RAM display; A5,output ignores RAM display. 
	
    OLED_WriteCmd(0xA6);    // set normal display. A7 inverse display
	
	
	//addressing setting
	OLED_WriteCmd(0x00);    // set lower column start address for page addressing mode. 0000b reset.
    OLED_WriteCmd(0x10);    // set higher column start address for page addressing mode. 0000b reset.

    OLED_WriteCmd(0x20);    // set memory addressing mode
    OLED_WriteCmd(0x10);    // 00,Horizontal addressing mode; 01,Vertical addressing mode; 10,Page addressing mode (RESET); 11,Invalid
	
	OLED_WriteCmd(0xb0);	// Set page start address for page addressing mode,0-7
	

	//panel resolution & layout (hardware configure)
    OLED_WriteCmd(0x40);    // set start line of display
	
    OLED_WriteCmd(0xA1);    // set segment re-map, column address 127 is mapped to SEG0
	
    OLED_WriteCmd(0xA8);    // set multiplex ratio (1 to 64)
    OLED_WriteCmd(0x3F);
	
    OLED_WriteCmd(0xC8);    // scan COM output scan direction. remapped mode, scan from COM[N-1] to COM[0] 
	
    OLED_WriteCmd(0xD3);    // set display offset (vertical shift)
    OLED_WriteCmd(0x00);    // reset to 00h
	
    OLED_WriteCmd(0xDA);    // set COM pins hardware configuration
    OLED_WriteCmd(0x12);    // reset, alternate COM pin configure
	
	
	//timing & driving schema setting
    OLED_WriteCmd(0xD5);    // change the division factor of internal oscillator frequency
    OLED_WriteCmd(0x80);    // Reset

    OLED_WriteCmd(0xD9);    // set pre-charge period
    OLED_WriteCmd(0x22);

    OLED_WriteCmd(0xDB);    // set Vcomh deselect level
    OLED_WriteCmd(0x20);    // reset, ~0.77 x Vcc
	
	
	//charge pump command
    OLED_WriteCmd(0x8D);    // charge pump setting
	OLED_WriteCmd(0x14);    // enable charge pump during display on
	
	
	//fundamental function
    OLED_WriteCmd(0xAF);    // open display
}

void OLED_Fill(unsigned char data)
{
	for (int page = 0; page < 8; page++)
    {
        OLED_WriteCmd(0xB0 + page);
        OLED_WriteCmd(0x00);        // column low addr
        OLED_WriteCmd(0x10);        // column heigh addr
        for (int i = 0; i < 128; i++)
        {
            OLED_WriteData(data);
        }
    }
}

void OLED_Clear(void)
{
	OLED_Fill(0x00);
}

//---------------------------------------------------------

static void OLED_SetCursor(uint8_t row, uint8_t col)
{
	OLED_WriteCmd(0xB0 + row); // set GDDRAM page start addr (page 0~7)
    OLED_WriteCmd(0x00 + (col & 0x0F)); // set col addr of low 4
    OLED_WriteCmd(0x10 + ((col >> 4) & 0x0F)); // set col addr of heigh 4
}

static FontSize OLED_GetFontSize(enum OLED_FONT_SIZE size)
{
	FontSize fs;
	
	switch(size)
	{
		case EOLED_FONT_6:
			fs.width  = sizeof(Font6x8[0]) / sizeof(Font6x8[0][0]);
			fs.height = 1;
			break;
		case EOLED_FONT_8:
			fs.width  = sizeof(Font8x16[0]) / sizeof(Font8x16[0][0]) / 2;
			fs.height = 2;
			break;
		default:
			fs.width  = sizeof(Font6x8[0]) / sizeof(Font6x8[0][0]);
			fs.height = 1;
			break;
	}

	return fs;
}

static void OLED_WriteFontData(uint8_t row, uint8_t col, char c, enum OLED_FONT_SIZE size)
{
	uint8_t i, j;
	FontSize fs;
	
	fs = OLED_GetFontSize(size);

	//row = (row>7) ? 7 : row;
	//col = (col>127) ? 127 : col;
	if ((row>7) || (col>127))
	{
		return;
	}
	
	OLED_SetCursor(row, col);
	
	switch(size)
	{
		case EOLED_FONT_6:
			for (i = 0; i < fs.width; i++)
			{
				OLED_WriteData(Font6x8[(uint8_t)c][i]);
			}
			break;
		case EOLED_FONT_8:
			for (j=0; j < fs.height; j++)
			{
				row += j;
				OLED_SetCursor(row, col);
				
				for (i = j*fs.width; i < fs.width*(j+1); i++)
				{
					OLED_WriteData(Font8x16[(uint8_t)c][i]);
				}			
			}
			break;
		default:
			for (i = 0; i < fs.width; i++)
			{
				OLED_WriteData(Font6x8[(uint8_t)c][i]);
			}
			break;
	}
}

void OLED_DisplayChar(uint8_t row, uint8_t col, char c, enum OLED_FONT_SIZE size)
{
	if (c < 32 || c > 126) c = '?';
	
    c -= 32;

	OLED_WriteFontData(row, col, c, size);
}

void OLED_DisplayStr(uint8_t row, uint8_t col, const char *text, enum OLED_FONT_SIZE size)
{
	const char *p = text;
	char  c;
	FontSize fs;
	
	fs = OLED_GetFontSize(size);
		
	while('\0' != *p)
	{
		c = (*p++) - 32;
		OLED_WriteFontData(row, col, c, size);
		
		col += fs.width;
		if (col > 127)
		{
			row += fs.height;
			col = 0;
		}
		if (row > 7)
		{
			//row = 7;
			break;
		}
	}
}

void OLED_DisplayNum(uint8_t row, uint8_t col, int value, enum OLED_FONT_SIZE size)
{
	char i, n, c;
	char strNum[24]={0};
	FontSize fs;
	
	fs = OLED_GetFontSize(size);
	
	c = (value >= 0) ? '+' : '-';
	OLED_DisplayChar(row, col, c, size);
	col = col + fs.width;
		
	if (0 == value)
	{
		OLED_DisplayChar(row, col, '0', size);
	}
	else
	{
		if (value < 0)
		{
			value = -value;
		}
		
		//extract every number
		i = 0;
		while(value > 0)
		{
			c = value%10;
			
			c += '0';
			if (i < sizeof(strNum))
			{
				strNum[i++] = c;
			}
						
			value /= 10;
		}
		
		//order number string in array
		n = i;
		for(i=0; i<n/2; i++)
		{
			c = strNum[i];
			strNum[i] = strNum[n-1 - i];
			strNum[n-1 - i] = c;
		}
		
		//display number string
		for(i=0; i<n; i++)
		{
			c = strNum[i];
			OLED_DisplayChar(row, col, c, size);

			col += fs.width;
		}
	}
}
